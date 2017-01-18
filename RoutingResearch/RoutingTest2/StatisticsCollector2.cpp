#include <StatisticsCollector2.h>

Define_Module(StatisticsCollector2);

void StatisticsCollector2::initialize()
{
    //todo сделать напрямую из файлов отсюда
    //ifstream* packetsHistoryFile = new ifstream(buildFullName(OUT_DIR, PACKETS_HIST));
    //ifstream* ictHistoryFile = new ifstream(buildFullName(OUT_DIR, ICT_HIST));
    //ifstream* routeInfoHistoryFile = new ifstream(buildFullName(OUT_DIR, RT_HIST));

    packetsHistoryDoc = par("packetsHistoryDoc");
    ictHistoryDoc = par("ictHistoryDoc");
    routeHistoryDoc = par("routeHistoryDoc");

    createdPackes = 0;
    deliveredPackets = 0;

    lifeTimePDF = new cDoubleHistogram("LIFE-TIME-HISTOGRAM", 300);
    lifeTimePDF->setRangeAutoUpper(0.0, 1000, 1.3);

    ictPDF = new cDoubleHistogram("ICT-HISTOGRAM", 300);
    ictPDF->setRangeAutoUpper(0.0, 300, 1.3);

    commonRoutesDurationPDF = NULL;

    routesDurationPDFbyNode = new vector<cDoubleHistogram*>();

    cout << "StatisticsCollector2: initialized" << endl;

    processPacketHistory();
    processICTHistory();
    processRouteHistory();

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
//                ASSERT(!created);   //т.е. данное событие для пакета встречается только один раз
                createdPackes++;
                created = true;     //т.е. данное событие для пакета уже учли

            } else if (strcmp(eventName, REGISTERED_EVENT) == 0) {
                //nothing yet

            } else if (strcmp(eventName, BEFORE_SEND_EVENT) == 0) {
                //nothing yet

            } else if (strcmp(eventName, REMOVED_EVENT) == 0) {
//                ASSERT(!removed && !delivered);
                removed = true;
                //nothing yet

            } else if (strcmp(eventName, DELIVERED_EVENT) == 0) {
//                ASSERT(!removed && !delivered);
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
//        ASSERT(ictValues[i] >= 0);
        ictPDF->collect(ictValues[i]);
    }
}


void StatisticsCollector2::processRouteHistory() {
    ASSERT(routesDurationPDFbyNode->size() == 0);
    double maxDayDuration = atof(routeHistoryDoc->getAttribute("maxDayDuration"));

    commonRoutesDurationPDF = new cDoubleHistogram("COMMON-ROUTE-DURATION-HISTOGRAM", 10);
    commonRoutesDurationPDF->setRange(0.0, maxDayDuration + 1);

    cXMLElementList nodes = routeHistoryDoc->getChildren();
    for(vector<cXMLElement*>::iterator nodePT = nodes.begin(); nodePT != nodes.end(); nodePT++) {
         cXMLElement* node = (*nodePT);
         int nodeId = atoi(node->getAttribute((char*) "nodeId"));

         cDoubleHistogram* routeDurationHist = new cDoubleHistogram("ROUTE-DURATION-HISTOGRAM", 10);
         routeDurationHist->setRange(0.0, maxDayDuration + 1);
         routesDurationPDFbyNode->push_back(routeDurationHist);

         // ensures that nodeId match to the index of list
         ASSERT((*routesDurationPDFbyNode)[nodeId] == routeDurationHist);

         cXMLElementList rotes = node->getChildren();
         for(vector<cXMLElement*>::iterator routePT = rotes.begin(); routePT != rotes.end(); routePT++) {
             cXMLElement* route = (*routePT);

             cStringTokenizer summaryTok(route->getNodeValue());
             vector<double> routeInfo = summaryTok.asDoubleVector();
             const double day      = routeInfo[0];
             const double dayStart = routeInfo[1];
             const double dayEnd   = routeInfo[2];
             const double savedDuration = routeInfo[3];

             // для расчёта длительности дня используем начало и конец дня. savedDuration игнорируем
             double duration = dayEnd - dayStart;
             ASSERT(duration > 0);
             routeDurationHist->collect(duration);
             commonRoutesDurationPDF->collect(duration);
         }
    }
}

//todo сделать запись ВСЕХ гистограмм через этот метод
void StatisticsCollector2::write(cDoubleHistogram* hist, ofstream* out, int level) {
    ASSERT(hist);
    char* prefix = new char[256];
    for (int i=0; i<level; i++) {
        if (i == 0) {
            prefix = strcpy(prefix, "    ");
        } else {
            prefix = strcat(prefix, "    ");
        }
    }

    (*out) << prefix << "<"<<hist->getName()<<"> "<< endl;
    (*out) << prefix << "    <COLLECTED> " << hist->getCount() << " </COLLECTED>" << endl;
    (*out) << prefix << "    <MIN> " << hist->getMin() << " </MIN>" << endl;
    (*out) << prefix << "    <MEAN> " << hist->getMean() << " </MEAN>" << endl;
    (*out) << prefix << "    <MAX> " << hist->getMax() << " </MAX>" << endl;
    (*out) << prefix << "    <VARIANCE> " << hist->getVariance() << " </VARIANCE>" << endl;
    (*out) << prefix << "    <OVERFLOW-CELL> " << hist->getOverflowCell() << " </OVERFLOW-CELL>" << endl;
    (*out) << prefix << "    <UNDERFLOW-CELL> " << hist->getUnderflowCell() << " </UNDERFLOW-CELL>" << endl;

    (*out) << prefix << "    <CELLS> " << hist->getNumCells() << " </CELLS>"<< endl;
    (*out) << prefix << "    <CELL-WIDTH> " << hist->getCellSize() << " </CELL-WIDTH>"<< endl;
    (*out) << prefix << "    <LEFT-BOUND> " << hist->getCellInfo(0).lower << " </LEFT-BOUND>" << endl;
    (*out) << prefix << "    <RIGHT-BOUND> " << hist->getCellInfo(hist->getNumCells()-1).upper << " </RIGHT-BOUND>"<< endl;

    (*out) << prefix << "    <PDF-VALS> " << endl;
    for (int i = 0; i < hist->getNumCells(); i++) (*out) << hist->getCellPDF(i) << "  ";
    (*out) << endl << prefix << "    </PDF-VALS> " << endl;
    (*out) << prefix << "    <CDF-VALS> " << endl;
    for (int i = 0; i < hist->getNumCells(); i++) {
        double val = 0;
        for (int j = 0; j <= i; j++) val += hist->getCellPDF(j);
        (*out) << val << "  ";
    }
    (*out) << endl << prefix << "    </CDF-VALS> " << endl;
    (*out) << prefix << "    <CCDF-VALS> " << endl;
    for (int i = 0; i < hist->getNumCells(); i++) {
        double val = 0;
        for (int j = i+1; j < hist->getNumCells(); j++) val += hist->getCellPDF(j);
        (*out) << val << "  ";
    }
    (*out) << endl << prefix << "    </CCDF-VALS> " << endl;
    (*out) << prefix << "</"<<hist->getName()<<"> " << endl;

    delete[] prefix;
}


void StatisticsCollector2::finish() {
    ASSERT(lifeTimePDF);
    ASSERT(ictPDF);
    ASSERT(commonRoutesDurationPDF);

    if (!lifeTimePDF->isTransformed()) lifeTimePDF->transform();
    if (!ictPDF->isTransformed()) ictPDF->transform();

    ofstream out(buildFullName(OUT_DIR, STAT_FILE));
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


    out << "    <ICT-HISTOGRAM> " << endl;
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
    out << "    </ICT-HISTOGRAM> " << endl << endl;


    write(commonRoutesDurationPDF, &out, 1);
    out << endl;


    out << "    <NODES-INFO> " << endl;
    for (unsigned int nodeId=0; nodeId<routesDurationPDFbyNode->size(); nodeId++) {
        out << "        <NODE nodeId=\""<<nodeId<<"\"> " << endl;
        write((*routesDurationPDFbyNode)[nodeId], &out, 3);
        out << "        </NODE> " << endl;
    }
    out << "    </NODES-INFO> " << endl;

    out << "</STATISTICS>" << endl;

    delete ictPDF;
    delete lifeTimePDF;
    for (unsigned int i=0; i<routesDurationPDFbyNode->size(); i++) delete (*routesDurationPDFbyNode)[i];
    delete routesDurationPDFbyNode;

    cout << "StatisticsCollector2 finished" << endl;
}
