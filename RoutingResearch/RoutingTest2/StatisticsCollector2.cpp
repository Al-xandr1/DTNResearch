#include <StatisticsCollector2.h>

Define_Module(StatisticsCollector2);

void StatisticsCollector2::initialize()
{
    // Чтение данных из файлов
    const char* packetsHistoryDocName = par("packetsHistoryDoc");
    xml_parse_result packetsHistoryDocResult = packetsHistoryDoc.load_file(packetsHistoryDocName);
    cout << endl << "Result of loading packetsHistoryDoc (" << packetsHistoryDocName << "): " << packetsHistoryDocResult.description() << endl;

    const char* ictHistoryDocDocName = par("ictHistoryDoc");
    xml_parse_result ictHistoryDocResult = ictHistoryDoc.load_file(ictHistoryDocDocName);
    cout << endl << "Result of loading ictHistoryDoc (" << ictHistoryDocDocName << "): " << ictHistoryDocResult.description() << endl;

    const char* routeHistoryDocName = par("routeHistoryDoc");
    xml_parse_result routeHistoryDocResult = routeHistoryDoc.load_file(routeHistoryDocName);
    cout << endl << "Result of loading routeHistoryDoc (" << routeHistoryDocName << "): " << routeHistoryDocResult.description() << endl;

    createdPackets = 0;
    deliveredPackets = 0;

    lifeTimePDF = new cDoubleHistogram("LIFE-TIME-HISTOGRAM", 300);
    lifeTimePDF->setRange(0.0, 81000);

    ictPDF = new cDoubleHistogram("ICT-HISTOGRAM", 300);
    ictPDF->setRange(0.0, 45000);

    commonRoutesDurationPDF = NULL;

    routesDurationPDFbyNode = new vector<cDoubleHistogram*>();

    cout << "StatisticsCollector2: initialized" << endl;

    processPacketHistory();
    processICTHistory();
    processRouteHistory();

    cout << "StatisticsCollector2: statistics collected" << endl;
}


void StatisticsCollector2::processPacketHistory() {
    xml_node packetsHistory = packetsHistoryDoc.child("PACKETS-HISTORY");

    for (xml_node_iterator packetPT = packetsHistory.begin(); packetPT != packetsHistory.end(); ++packetPT) {
        xml_node packet = (*packetPT);

        // считывание тега SUMMARY
        xml_node summary = packet.child("SUMMARY");
        cStringTokenizer summaryTok(summary.child_value());
        vector<double> summaryVec = summaryTok.asDoubleVector();
        double sourceId         = summaryVec[0];
        double destinationId    = summaryVec[1];
        double creationTime     = summaryVec[2];
        double receivedTime     = summaryVec[3];

        // считывание тега HISTORY
        xml_node history = packet.child("HISTORY");
        bool created = false,
             removed = false,
             delivered = false;

        for (xml_node_iterator eventPT = history.begin(); eventPT != history.end(); ++eventPT) {
            xml_node event = (*eventPT);

            const char* eventName = event.name();

            if (strcmp(eventName, CREATED_EVENT) == 0) {
                ASSERT(!created);   //т.е. данное событие для пакета встречается только один раз
                createdPackets++;
                ASSERT(deliveredPackets <= createdPackets);
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
                if(deliveredPackets > createdPackets) cout<<"deliveredPackets="<<deliveredPackets<<", createdPackets="<<createdPackets<<endl;
                ASSERT(deliveredPackets <= createdPackets);

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
    xml_node ictHistory = ictHistoryDoc.child("ICT-HISTORY");
    cStringTokenizer tok(ictHistory.child_value());
    vector<double> ictValues = tok.asDoubleVector();
    for (unsigned int i=0; i<ictValues.size(); i++) {
        ASSERT(ictValues[i] >= 0);
        ictPDF->collect(ictValues[i]);
    }
}


void StatisticsCollector2::processRouteHistory() {
    ASSERT(routesDurationPDFbyNode->size() == 0);
    xml_node routeHistory = routeHistoryDoc.child("ROUTE-HISTORY");
    double maxDayDuration = routeHistory.attribute("maxDayDuration").as_double();

    commonRoutesDurationPDF = new cDoubleHistogram("COMMON-ROUTE-DURATION-HISTOGRAM", 10);
    commonRoutesDurationPDF->setRange(0.0, maxDayDuration + 1);

    for (xml_node_iterator nodePT = routeHistory.begin(); nodePT != routeHistory.end(); ++nodePT) {
        xml_node node = (*nodePT);
        int nodeId = node.attribute("nodeId").as_int();

        cDoubleHistogram* routeDurationHist = new cDoubleHistogram("ROUTE-DURATION-HISTOGRAM", 10);
        routeDurationHist->setRange(0.0, maxDayDuration + 1);
        routesDurationPDFbyNode->push_back(routeDurationHist);

        // ensures that nodeId match to the index of list
        ASSERT((*routesDurationPDFbyNode)[nodeId] == routeDurationHist);

        for (xml_node_iterator routePT = node.begin(); routePT != node.end(); ++routePT) {
            xml_node route = (*routePT);

            cStringTokenizer summaryTok(route.child_value());
            vector<double> routeInfo = summaryTok.asDoubleVector();
            const double day      = routeInfo[0];
            const double dayStart = routeInfo[1];
            const double dayEnd   = routeInfo[2];
            const double savedDuration = routeInfo[3];

            // для расчёта длительности дня используем начало и конец дня. savedDuration игнорируем
            double duration = dayEnd - dayStart;
            ASSERT(duration >= 0);
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
        (*out) << val*hist->getCellSize() << "  ";
    }
    (*out) << endl << prefix << "    </CDF-VALS> " << endl;
    (*out) << prefix << "    <CCDF-VALS> " << endl;
    for (int i = 0; i < hist->getNumCells(); i++) {
        double val = 0;
        for (int j = i+1; j < hist->getNumCells(); j++) val += hist->getCellPDF(j);
        (*out) << val*hist->getCellSize() << "  ";
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

    ASSERT(deliveredPackets <= createdPackets);
    double deliveredPercentage = (100.0 * deliveredPackets) / (1.0 * createdPackets);
    if (deliveredPercentage < 0 || deliveredPercentage > 100 || deliveredPercentage != deliveredPercentage)
        cout << endl << "\t!!! WARNING !!! deliveredPercentage = " << deliveredPercentage
        << ", deliveredPackets = " << deliveredPackets << ", createdPackets = " << createdPackets << endl << endl;
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
        out << val*lifeTimePDF->getCellSize() << "  ";
    }
    out << endl << "        </CDF-VALS> " << endl;
    out << "        <CCDF-VALS> " << endl;
    for (int i = 0; i < lifeTimePDF->getNumCells(); i++) {
        double val = 0;
        for (int j = i+1; j < lifeTimePDF->getNumCells(); j++) val += lifeTimePDF->getCellPDF(j);
        out << val*lifeTimePDF->getCellSize() << "  ";
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
        out << val*ictPDF->getCellSize() << "  ";
    }
    out << endl << "        </CDF-VALS> " << endl;
    out << "        <CCDF-VALS> " << endl;
    for (int i = 0; i < ictPDF->getNumCells(); i++) {
        double val = 0;
        for (int j = i+1; j < ictPDF->getNumCells(); j++) val += ictPDF->getCellPDF(j);
        out << val*ictPDF->getCellSize() << "  ";
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
