#include "MobileHost.h"

Define_Module(MobileHost);

void MobileHost::initialize()
{
    nodeId = par("indexOfNode");
    timeslot = par("timeslot");
    lambda   = par("lambda");

    packetsForSending = new vector<Packet*>();

    cMessage* msg = new cMessage();
    msg->setKind(0);
    scheduleAt(simTime(), msg);
};

void MobileHost::handleMessage(cMessage *msg)
{
    // msg->isSelfMessage() ������ ������������, ��� ��� � ������ RD_Listener::processReceivedData
    // ������� ��������� �������� ��� host->sendDirect(msg, host);
    if (msg->getKind() == 0) {// ��������� � �������� ������ ������
        RoutingDaemon* routing = check_and_cast<RoutingDaemon*>(getParentModule()->getSubmodule("routing"));

        int nodeIdTrg = (int) round(uniform(0, (double) routing->getNumHosts()));
        Packet* packet = new Packet(nodeId, nodeIdTrg);
        packet->setKind(2);

        //TODO ���������� ��������� � �� ��������� ���� �������
        packetsForSending->push_back(packet);

        scheduleAt(simTime() + timeslot * exponential(1/lambda), msg);


    } else if (msg->getKind() == 1) {//��������� � ����������
        ConnectionMessage* connectionMessage = check_and_cast<ConnectionMessage*>(msg);

        vector<int>* connectedIds = connectionMessage->getConnectedTargetIds();
        //TODO ��������� ��� ������������ ������ ��������� ����� � ���������
        //for (int i = 0; i < connectedIds->size(); i++) {
        //}

        delete connectionMessage;

    } else if (msg->getKind() == 2) {//����� �� ������� ����
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
