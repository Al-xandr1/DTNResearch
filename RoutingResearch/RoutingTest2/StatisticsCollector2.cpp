#include <StatisticsCollector2.h>

Define_Module(StatisticsCollector2);

void StatisticsCollector2::initialize()
{
    packetsHistoryFile = new ifstream(buildFullName((char*) "outTrace", (char*) "packetsHistory.xml"));

    createdPackes = 0;
    receivedPackets = 0;

    lifeTimePDF = new cDoubleHistogram("lifeTimePDF", 300);
    lifeTimePDF->setRangeAutoUpper(0.0, 1000, 1.3);

    ictPDF = new cDoubleHistogram("ictPDF", 300);
    ictPDF->setRangeAutoUpper(0.0, 300, 1.3);

    cout << "StatisticsCollector2 initialized" << endl;
}

void StatisticsCollector2::handleMessage(cMessage *msg)
{
    switch (msg->getKind()) {

        case NEW_PACKET_CREATED: {    //новый пакет создан
            createdPackes++;
            delete msg;
            break;
        }

        case PACKET_RECEIVED: {      //пакет получен узлом и удалён
            PacketReceived* packetReceived = check_and_cast<PacketReceived*>(msg);

            receivedPackets++;
            lifeTimePDF->collect(packetReceived->getLiveTime());

            delete packetReceived;
            break;
        }

        case ICT_INFO: {             //сбор статистики по ICT
            ICTMessage* ictMsg = check_and_cast<ICTMessage*>(msg);
            ASSERT(ictMsg->getICT() >= 0);
            ictPDF->collect(ictMsg->getICT());

            delete ictMsg;
            break;
        }

        default: {
            ASSERT(false);           //unreachable statement
            break;
        }
    }
}

void StatisticsCollector2::finish() {
    if (!lifeTimePDF->isTransformed()) lifeTimePDF->transform();
    if (!ictPDF->isTransformed()) ictPDF->transform();

    ofstream out(buildFullName((char*) "outTrace", (char*) "statistics.xml"));
    out << "<?xml version=\'1.0' ?>" << endl << endl;
    out << "<STATISTICS>" << endl;

    double deliveredPercentage = (1.0 * receivedPackets) / (1.0 * createdPackes) * 100;
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
