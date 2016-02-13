
#include "StatisticsCollector.h"

Define_Module(StatisticsCollector);

void StatisticsCollector::initialize()
{
    createdPackes = 0;
    receivedPackets = 0;
    timeOfCollection = getParentModule()->par("timeOfCollection");
    rdGate = getParentModule()->getSubmodule("routing")->gate("in");

    lifeTimePDF = new cDoubleHistogram();
    ictPDF = new cDoubleHistogram();
}

void StatisticsCollector::handleMessage(cMessage *msg)
{
    if (msg->getKind() == PACKET) {//пакет от мобильного узла
        Packet* packet = check_and_cast<Packet*>(msg);

        receivedPackets++;

        if (receivedPackets % 100 == 0) cout << "Delivered Packets = " << receivedPackets << endl;

        simtime_t liveTime = packet->getLiveTime();
        lifeTimePDF->collect(liveTime);

        delete packet;


    } else if (msg->getKind() == REQUEST_FOR_ROUTING) {//заявка на маршуризацию
        createdPackes++;
        if (createdPackes % 100 == 0) cout << "Created Packes = " << createdPackes << endl;
        sendDirect(msg, rdGate);


    } else if (msg->getKind() == ICT_INFO) {//сбор статистики по ICT
        ICTMessage* ictMsg = check_and_cast<ICTMessage*>(msg);
        ictPDF->collect(ictMsg->getICT());

        delete ictMsg;

    }
    else {
        exit(-555);
    }
}

void StatisticsCollector::finish() {
    double deliveredPercentage = (1.0 * receivedPackets) / (1.0 * createdPackes) * 100;

    cout << "DeliveredPercentage = " << deliveredPercentage << " %" << endl << endl;

    cout << "Packet's life time PDF:" << endl;
    for (int i=0; i < lifeTimePDF->getNumCells(); i++) {
        cout << lifeTimePDF->getCellPDF(i) << "  ";
        if (i % 30 == 0) cout << endl;
    }
    cout << endl;

    cout << "ICT PDF:" << endl;
    for (int i=0; i < ictPDF->getNumCells(); i++) {
        cout << ictPDF->getCellPDF(i) << "  ";
        if (i % 30 == 0) cout << endl;
    }
    cout << endl;
}
