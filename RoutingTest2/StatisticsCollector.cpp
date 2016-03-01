
#include "StatisticsCollector.h"

Define_Module(StatisticsCollector);

void StatisticsCollector::initialize()
{
    createdPackes = 0;
    receivedPackets = 0;
    timeOfCollection = getParentModule()->par("timeOfCollection");
    rdGate = getParentModule()->getSubmodule("routing")->gate("in");

    lifeTimePDF = new cDoubleHistogram();
    lifeTimePDF->setCellSize(100);
    lifeTimePDF->setRangeAutoUpper(0, 100, 5);
    ictPDF = new cDoubleHistogram();
    ictPDF->setCellSize(100);
    ictPDF->setRangeAutoUpper(0, 100, 5);
}

void StatisticsCollector::handleMessage(cMessage *msg)
{
    if (msg->getKind() == NEW_PACKET_CREATED) {//новый пакет создан
        createdPackes++;
        if (createdPackes % 100 == 0) cout << "Created Packes = " << createdPackes << endl;
        delete msg;


    } else if (msg->getKind() == PACKET_RECEIVED) {//пакет получен узлом и удалён
        PacketReceived* packetReceived = check_and_cast<PacketReceived*>(msg);

        receivedPackets++;
        if (receivedPackets % 100 == 0) cout << "Delivered Packets = " << receivedPackets << endl;

        simtime_t liveTime = packetReceived->getLiveTime();
        lifeTimePDF->collect(liveTime);

        delete packetReceived;


    } else if (msg->getKind() == ICT_INFO) {//сбор статистики по ICT
        ICTMessage* ictMsg = check_and_cast<ICTMessage*>(msg);
        if (ictMsg->getICT() < 0) {cout << "ictMsg->getICT() = " << ictMsg->getICT() << endl; exit(-234); }
        ictPDF->collect(ictMsg->getICT());

        delete ictMsg;

    } else {
        exit(-555);
    }
}

void StatisticsCollector::finish() {
    if (!lifeTimePDF->isTransformed()) lifeTimePDF->transform();
    if (!ictPDF->isTransformed()) ictPDF->transform();

    ofstream out(buildFullName("outTrace", "ictstatistics.xml"));
    out << "<?xml version=\'1.0' ?>" << endl << endl;
    out << "<STATISTICS>" << endl;

    double deliveredPercentage = (1.0 * receivedPackets) / (1.0 * createdPackes) * 100;
    out << "    <DELIVERED-PACKETS> " << deliveredPercentage << " </DELIVERED-PACKETS>" << endl;


    out << "    <LIFE-TIME-PDF> " << endl;
    out << "        <COLLECTED> " << lifeTimePDF->getCount() << " </COLLECTED>" << endl;
    out << "        <MIN> " << lifeTimePDF->getMin() << " </MIN>" << endl;
    out << "        <MEAN> " << lifeTimePDF->getMean() << " </MEAN>" << endl;
    out << "        <MAX> " << lifeTimePDF->getMax() << " </MAX>" << endl;
    out << "        <VARIANCE> " << lifeTimePDF->getVariance() << " </VARIANCE>" << endl;
    out << "        <BASEPOINT> " << lifeTimePDF->getBasepoint(0) << " </BASEPOINT>"<< endl;
    out << "        <CELL-SIZE> " << lifeTimePDF->getCellSize() << " </CELL-SIZE>"<< endl;
    out << "        <NUM-CELLS> " << lifeTimePDF->getNumCells() << " </NUM-CELLS>"<< endl;
    out << "        <VALUES> " << endl;
    for(int i = 0; i < lifeTimePDF->getNumCells(); i++) out << lifeTimePDF->getCellPDF(i) << "  ";
    out << endl << "        </VALUES> " << endl;
    out << "        <OVERFLOW-CELL> " << lifeTimePDF->getOverflowCell() << " </OVERFLOW-CELL>" << endl;
    out << "        <UNDERFLOW-CELL> " << lifeTimePDF->getUnderflowCell() << " </UNDERFLOW-CELL>" << endl;
    out << "    </LIFE-TIME-PDF> " << endl << endl;


    out << "    <LIFE-TIME-CDF> " << endl;
    out << "        <CELL-SIZE> " << lifeTimePDF->getCellSize() << " </CELL-SIZE>"<< endl;
    out << "        <NUM-CELLS> " << lifeTimePDF->getNumCells() << " </NUM-CELLS>"<< endl;
    out << "        <VALUES> " << endl;
    for (int i = 0; i < lifeTimePDF->getNumCells(); i++) {
        double val = 0;
        for (int j = 0; j <= i; j++) val += lifeTimePDF->getCellPDF(j);
        out << val << "  ";
    }
    out << endl << "        </VALUES> " << endl;
    out << "    </LIFE-TIME-CDF> " << endl << endl;


    out << "    <LIFE-TIME-CCDF> " << endl;
    out << "        <CELL-SIZE> " << lifeTimePDF->getCellSize() << " </CELL-SIZE>"<< endl;
    out << "        <NUM-CELLS> " << lifeTimePDF->getNumCells() << " </NUM-CELLS>"<< endl;
    out << "        <VALUES> " << endl;
    for (int i = 0; i < lifeTimePDF->getNumCells(); i++) {
        double val = 0;
        for (int j = i+1; j < lifeTimePDF->getNumCells(); j++) val += lifeTimePDF->getCellPDF(j);
        out << val << "  ";
    }
    out << endl << "        </VALUES> " << endl;
    out << "    </LIFE-TIME-CCDF> " << endl << endl << endl;


    out << "    <ICT-PDF> " << endl;
    out << "        <COLLECTED> " << ictPDF->getCount() << " </COLLECTED>" << endl;
    out << "        <MIN> " << ictPDF->getMin() << " </MIN>" << endl;
    out << "        <MEAN> " << ictPDF->getMean() << " </MEAN>" << endl;
    out << "        <MAX> " << ictPDF->getMax() << " </MAX>" << endl;
    out << "        <VARIANCE> " << ictPDF->getVariance() << " </VARIANCE>" << endl;
    out << "        <BASEPOINT> " << ictPDF->getBasepoint(0) << " </BASEPOINT>"<< endl;
    out << "        <CELL-SIZE> " << ictPDF->getCellSize() << " </CELL-SIZE>"<< endl;
    out << "        <NUM-CELLS> " << ictPDF->getNumCells() << " </NUM-CELLS>"<< endl;
    out << "        <VALUES> " << endl;
    for (int i = 0; i < ictPDF->getNumCells(); i++) out << ictPDF->getCellPDF(i) << "  ";
    out << endl << "        </VALUES> " << endl;
    out << "        <OVERFLOW-CELL> " << ictPDF->getOverflowCell() << " </OVERFLOW-CELL>" << endl;
    out << "        <UNDERFLOW-CELL> " << ictPDF->getUnderflowCell() << " </UNDERFLOW-CELL>" << endl;
    out << "    </ICT-PDF> " << endl << endl;


    out << "    <ICT-CDF> " << endl;
    out << "        <CELL-SIZE> " << ictPDF->getCellSize() << " </CELL-SIZE>"<< endl;
    out << "        <NUM-CELLS> " << ictPDF->getNumCells() << " </NUM-CELLS>"<< endl;
    out << "        <VALUES> " << endl;
    for (int i = 0; i < ictPDF->getNumCells(); i++) {
        double val = 0;
        for (int j = 0; j <= i; j++) val += ictPDF->getCellPDF(j);
        out << val << "  ";
    }
    out << endl << "        </VALUES> " << endl;
    out << "    </ICT-CDF> " << endl << endl;


    out << "    <ICT-CCDF> " << endl;
    out << "        <CELL-SIZE> " << ictPDF->getCellSize() << " </CELL-SIZE>"<< endl;
    out << "        <NUM-CELLS> " << ictPDF->getNumCells() << " </NUM-CELLS>"<< endl;
    out << "        <VALUES> " << endl;
    for (int i = 0; i < ictPDF->getNumCells(); i++) {
        double val = 0;
        for (int j = i+1; j < ictPDF->getNumCells(); j++) val += ictPDF->getCellPDF(j);
        out << val << "  ";
    }
    out << endl << "        </VALUES> " << endl;
    out << "    </ICT-CCDF> " << endl;


    out << "<STATISTICS>" << endl;
}
