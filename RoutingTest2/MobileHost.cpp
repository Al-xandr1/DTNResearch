#include "MobileHost.h"

Define_Module(MobileHost);

#define FOR_NEW_PACKET 0        // ��������� � �������� ������ ������
#define ESTABLISED_CONNECTION 1 // ��������� � ����������
#define PACKET 2                // ����������� �������� ������
#define REQUEST_FOR_ROUTING 3   // ������ �� �������������


void MobileHost::initialize()
{
    rd = check_and_cast<RoutingDaemon*>(getParentModule()->getSubmodule("routing"));

    nodeId = par("indexOfNode");
    timeslot = par("timeslot");
    lambda   = par("lambda");

    packetsForSending = new vector<Packet*>();

    cMessage* msg = new cMessage();
    msg->setKind(FOR_NEW_PACKET);
    scheduleAt(simTime(), msg);
};

void MobileHost::handleMessage(cMessage *msg)
{
    if (msg->getKind() == FOR_NEW_PACKET) {// ��������� � �������� ������ ������

        int nodeIdTrg = (int) round(uniform(0, (double) rd->getNumHosts()));

        Packet* packet = new Packet(nodeId, nodeIdTrg);
        packet->setKind(PACKET);
        packetsForSending->push_back(packet);

//        Request* request = new Request(nodeId, nodeIdTrg);
//        packet->setKind(3);

        scheduleAt(simTime() + timeslot * exponential(1/lambda), msg);


    } else if (msg->getKind() == ESTABLISED_CONNECTION) {//��������� � ����������
        ConnectionMessage* connectionMessage = check_and_cast<ConnectionMessage*>(msg);

        vector<int>* connectedIds = connectionMessage->getConnectedTargetIds();
        //TODO ��������� ��� ������������ ������ ��������� ����� � ���������
        //for (int i = 0; i < connectedIds->size(); i++) {
        //}

        delete connectionMessage;

    } else if (msg->getKind() == PACKET) {//����� �� ������� ����
        Packet* packet = check_and_cast<Packet*>(msg);
        if (packet->getNodeIdTrg() != nodeId) {
            EV << "Wrong packet sending: nodeId = " << nodeId
                    << ", packet->getNodeIdSrc()" << packet->getNodeIdSrc()
                    << ", packet->getNodeIdTrg()" << packet->getNodeIdTrg() << endl;
            cout << "Wrong packet sending: nodeId = " << nodeId
                    << ", packet->getNodeIdSrc()" << packet->getNodeIdSrc()
                    << ", packet->getNodeIdTrg()" << packet->getNodeIdTrg() << endl;
        }
        delete msg;


    } else {
        EV << "unknown type of message" << endl;
        cout << "unknown type of message" << endl;

        delete msg;
    }
}

void MobileHost::sendPacketTo(Packet* packet, int nodeId) {
    cGate *dst = getParentModule()->getSubmodule("host", nodeId)->gate("in");
    sendDirect(packet, dst);
}
