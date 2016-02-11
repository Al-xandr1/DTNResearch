
#include "StatisticsCollector.h"

Define_Module(StatisticsCollector);

void StatisticsCollector::initialize()
{
    createdPackes = 0;
    recievedPackets = 0;
    timeOfCollection = getParentModule()->par("timeOfCollection");
}

void StatisticsCollector::handleMessage(cMessage *msg)
{
    if (msg->getKind() == PACKET) {//����� �� ���������� ����
        Packet* packet = check_and_cast<Packet*>(msg);

        recievedPackets++;

        //todo ������� ���� ����������
        cout << recievedPackets << endl;
        cout << "recievedPackets = " << recievedPackets << endl;

        simtime_t liveTime = packet->getLiveTime();
        cout << "liveTime = " << liveTime << endl;

        delete packet;


    } else if (msg->getKind() == REQUEST_FOR_ROUTING) {//������ �� ������������
        Request* request = check_and_cast<Request*>(msg);

        createdPackes++;
        cout << "createdPackes = " << createdPackes << endl;
        //������������ RoutingDaemon'��

    } else if (msg->getKind() == ICT_INFO) {//���� ���������� �� ICT
        ICTMessage* ictMsg = check_and_cast<ICTMessage*>(msg);

        //todo ���� ���������� �� ict ��� ���� (i,j)=(j,i)
        cout << "ictMsg->getICT() = " << ictMsg->getICT() << endl;

        delete ictMsg;

    }
    else {
        exit(-555);
    }
}

void StatisticsCollector::finish() {

}
