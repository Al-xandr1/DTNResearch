#include <StatisticsCollector2.h>

Define_Module(StatisticsCollector2);

#define gDEBP(A,B) (A->getDocumentElementByPath(A,B))->getNodeValue()

//todo обобщить имена файлов. Сделать удобное использование в HistoryCollector & StatisticsCollector2
//todo переместить указанные классы в отдельный пакет

void StatisticsCollector2::initialize()
{
    //packetsHistoryFile = new ifstream(buildFullName((char*) "outTrace", (char*) "packetsHistory.xml"));
    //ictHistoryFile = new ifstream(buildFullName((char*) "outTrace", (char*) "ictHistory.xml"));

    packetsHistoryDoc = par("packetsHistoryDoc");
    ictHistoryDoc = par("ictHistoryDoc");

    createdPackes = 0;
    deliveredPackets = 0;

    lifeTimePDF = new cDoubleHistogram("lifeTimePDF", 300);
    lifeTimePDF->setRangeAutoUpper(0.0, 1000, 1.3);

    ictPDF = new cDoubleHistogram("ictPDF", 300);
    ictPDF->setRangeAutoUpper(0.0, 300, 1.3);

    cout << "StatisticsCollector2: initialized" << endl;

    processPacketHistory();
    processICTHistory();

    cout << "StatisticsCollector2: statistics collected" << endl;
}


void StatisticsCollector2::processPacketHistory() {
    cXMLElementList packetTags = packetsHistoryDoc->getChildren();

    for(vector<cXMLElement*>::iterator packetPT = packetTags.begin(); packetPT != packetTags.end(); packetPT++) {
        cXMLElement* packet = (*packetPT);

        // считывание тега SUMMARY
        cXMLElement* summary = packet->getElementByPath("./SUMMARY");
        cStringTokenizer summaryTok(summary->getNodeValue());
        vector<double> summaryVec = summaryTok.asDoubleVector();
        double sourceId         = summaryVec[0];
        double destinationId    = summaryVec[1];
        double creationTime     = summaryVec[2];
        double receivedTime     = summaryVec[3];

        // считывание тега HISTORY
        cXMLElement* history = packet->getElementByPath("./HISTORY");
        bool created = false,
             removed = false,
             delivered = false;

        cXMLElementList events = history->getChildren();
        for(vector<cXMLElement*>::iterator eventPT = events.begin(); eventPT != events.end(); eventPT++) {
            cXMLElement* event = (*eventPT);

            const char* eventName = event->getTagName();

            if (strcmp(eventName, CREATED_EVENT) == 0) {
                ASSERT(!created);   //т.е. данное событие для пакета встречается только один раз
                createdPackes++;
                created = true;     //т.е. данное событие для пакета уже учли

            } else if (strcmp(eventName, REGISTERED_EVENT) == 0) {
                //nothing yet

            } else if (strcmp(eventName, BEFORE_SEND_EVENT) == 0) {
                //nothing yet

            } else if (strcmp(eventName, REMOVED_EVENT) == 0) {
                ASSERT(!removed && !delivered);
                removed = true;
                //nothing yet

            } else if (strcmp(eventName, DELIVERED_EVENT) == 0) {
                ASSERT(!removed && !delivered);
                delivered = true;
                deliveredPackets++;

                // время жизни учитываем только у доставленных
                lifeTimePDF->collect(receivedTime - creationTime);

            } else {
                cout << "Unknown name of event: " << eventName << endl;
                ASSERT(false);        //unreachable statement
            }
        }
    }
}


void StatisticsCollector2::processICTHistory() {
    cStringTokenizer tok(ictHistoryDoc->getNodeValue());
    vector<double> ictValues = tok.asDoubleVector();
    for (unsigned int i=0; i<ictValues.size(); i++) {
        ASSERT(ictValues[i] >= 0);
        ictPDF->collect(ictValues[i]);
    }
}


void StatisticsCollector2::finish() {
    if (!lifeTimePDF->isTransformed()) lifeTimePDF->transform();
    if (!ictPDF->isTransformed()) ictPDF->transform();

    ofstream out(buildFullName((char*) "outTrace", (char*) "statistics.xml"));
    out << "<?xml version=\'1.0' ?>" << endl << endl;
    out << "<STATISTICS>" << endl;

    double deliveredPercentage = (1.0 * deliveredPackets) / (1.0 * createdPackes) * 100;
    out << "    <DELIVERED-PACKETS> " << deliveredPercentage << " </DELIVERED-PACKETS>" << endl << endl;


    out << "    <LIFE-TIME-HISTOGRAM> " << endl;
    out << "        <COLLECTED> " << lifeTimePDF->getCount() << " </COLLECTED>" << endl;
    out << "        <MIN> " << lifeTimePDF->getMin() << " </MIN>" << endl;
    out << "        <MEAN> " << lifeTimePDF->getMean() << " </MEAN>" << endl;
    out << "        <MAX> " << lifeTimePDF->getMax() << " </MAX>" << endl;
    out << "        <VARIANCE> " << lifeTimePDF->getVariance() << " </VARIANCE>" << endl;
    out << "        <OVERFLOW-CELL> " << lifeTimePDF->getOverflowCell() << " </OVERFLOW-CELL>" << endl;
    out << "        <UNDERFLOW-CELL> " << lifeTimePDF->getUnderflowCell() << " </UNDERFLOW-CELL>" << endl;

    out << "        <CELLS> " << lifeTimePDF->getNumCells() << " </CELLS>"<< endl;
    out << "        <CELL-WIDTH> " << lifeTimePDF->getCellSize() << " </CELL-WIDTH>"<< endl;
    out << "        <LEFT-BOUND> " << lifeTimePDF->getCellInfo(0).lower << " </LEFT-BOUND>"<< endl;
    out << "        <RIGHT-BOUND> " << lifeTimePDF->getCellInfo(lifeTimePDF->getNumCells()-1).upper<< " </RIGHT-BOUND>"<< endl;
    out << "        <PDF-VALS> " << endl;
    for(int i = 0; i < lifeTimePDF->getNumCells(); i++) out << lifeTimePDF->getCellPDF(i) << "  ";
    out << endl << "        </PDF-VALS> " << endl;
    out << "        <CDF-VALS> " << endl;
    for (int i = 0; i < lifeTimePDF->getNumCells(); i++) {
        double val = 0;
        for (int j = 0; j <= i; j++) val += lifeTimePDF->getCellPDF(j);
        out << val << "  ";
    }
    out << endl << "        </CDF-VALS> " << endl;
    out << "        <CCDF-VALS> " << endl;
    for (int i = 0; i < lifeTimePDF->getNumCells(); i++) {
        double val = 0;
        for (int j = i+1; j < lifeTimePDF->getNumCells(); j++) val += lifeTimePDF->getCellPDF(j);
        out << val << "  ";
    }
    out << endl << "        </CCDF-VALS> " << endl;
    out << "    </LIFE-TIME-HISTOGRAM> " << endl << endl;


    out << "    <ICT-PDF-HISTOGRAM> " << endl;
    out << "        <COLLECTED> " << ictPDF->getCount() << " </COLLECTED>" << endl;
    out << "        <MIN> " << ictPDF->getMin() << " </MIN>" << endl;
    out << "        <MEAN> " << ictPDF->getMean() << " </MEAN>" << endl;
    out << "        <MAX> " << ictPDF->getMax() << " </MAX>" << endl;
    out << "        <VARIANCE> " << ictPDF->getVariance() << " </VARIANCE>" << endl;
    out << "        <OVERFLOW-CELL> " << ictPDF->getOverflowCell() << " </OVERFLOW-CELL>" << endl;
    out << "        <UNDERFLOW-CELL> " << ictPDF->getUnderflowCell() << " </UNDERFLOW-CELL>" << endl;

    out << "        <CELLS> " << ictPDF->getNumCells() << " </CELLS>"<< endl;
    out << "        <CELL-WIDTH> " << ictPDF->getCellSize() << " </CELL-WIDTH>"<< endl;
    out << "        <LEFT-BOUND> " << ictPDF->getCellInfo(0).lower << " </LEFT-BOUND>" << endl;
    out << "        <RIGHT-BOUND> " << ictPDF->getCellInfo(ictPDF->getNumCells()-1).upper << " </RIGHT-BOUND>"<< endl;

    out << "        <PDF-VALS> " << endl;
    for (int i = 0; i < ictPDF->getNumCells(); i++) out << ictPDF->getCellPDF(i) << "  ";
    out << endl << "        </PDF-VALS> " << endl;
    out << "        <CDF-VALS> " << endl;
    for (int i = 0; i < ictPDF->getNumCells(); i++) {
        double val = 0;
        for (int j = 0; j <= i; j++) val += ictPDF->getCellPDF(j);
        out << val << "  ";
    }
    out << endl << "        </CDF-VALS> " << endl;
    out << "        <CCDF-VALS> " << endl;
    for (int i = 0; i < ictPDF->getNumCells(); i++) {
        double val = 0;
        for (int j = i+1; j < ictPDF->getNumCells(); j++) val += ictPDF->getCellPDF(j);
        out << val << "  ";
    }
    out << endl << "        </CCDF-VALS> " << endl;
    out << "    </ICT-PDF-HISTOGRAM> " << endl;


    out << "</STATISTICS>" << endl;

    cout << "StatisticsCollector2 finished" << endl;
}
