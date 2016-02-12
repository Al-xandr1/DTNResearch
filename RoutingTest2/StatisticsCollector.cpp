
#include "StatisticsCollector.h"

Define_Module(StatisticsCollector);

void StatisticsCollector::initialize()
{
    createdPackes = 0;
    recievedPackets = 0;
    timeOfCollection = getParentModule()->par("timeOfCollection");
    rdGate = getParentModule()->getSubmodule("routing")->gate("in");
}

void StatisticsCollector::handleMessage(cMessage *msg)
{
    if (msg->getKind() == PACKET) {//пакет от мобильного узла
        Packet* packet = check_and_cast<Packet*>(msg);

        recievedPackets++;
        cout << "recievedPackets = " << recievedPackets << endl;

        //todo сделать сбор статистики
        simtime_t liveTime = packet->getLiveTime();
        cout << "liveTime = " << liveTime << endl;

        delete packet;


    } else if (msg->getKind() == REQUEST_FOR_ROUTING) {//заявка на маршуризацию
        createdPackes++;
        cout << "createdPackes = " << createdPackes << endl;
        sendDirect(msg, rdGate);


    } else if (msg->getKind() == ICT_INFO) {//сбор статистики по ICT
        ICTMessage* ictMsg = check_and_cast<ICTMessage*>(msg);

        //todo сбор статистики по ict для всех (i,j)=(j,i)
        cout << "ictMsg->getICT() = " << ictMsg->getICT() << endl;

        delete ictMsg;

    }
    else {
        exit(-555);
    }
}

void StatisticsCollector::finish() {

}
