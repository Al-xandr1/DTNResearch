#include "MobileHost.h"

Define_Module(MobileHost);


void MobileHost::initialize()
{
    rd = check_and_cast<RoutingDaemon*>(getParentModule()->getSubmodule("routing"));
    rdGate = rd->gate("in");
    collectorGate = getParentModule()->getSubmodule("collector")->gate("in");

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
    if (msg->getKind() == FOR_NEW_PACKET) {// сообщение о создании нового пакета
        Packet* packet = createPacket();
        registerPacket(packet);

        scheduleAt(simTime() + timeslot * exponential(1/lambda), msg);


    } else if (msg->getKind() == ESTABLISED_CONNECTION) {//сообщение о соединении
        ConnectionMessage* connectionMessage = check_and_cast<ConnectionMessage*>(msg);

        // отправить все накопившиеся пакеты узлам, с которыми появилось соединение
        vector<int>* connectedIds = connectionMessage->getConnectedTargetIds();
        for (unsigned int i = 0; i < connectedIds->size(); i++) {
            int id = (*connectedIds)[i];
            for(vector<Packet*>::iterator it = packetsForSending->begin(); it != packetsForSending->end(); ) {
                Packet* packet = (*it);
                if (packet->getNodeIdTrg() == id) {
                    it = packetsForSending->erase(it);

                    cGate *dst = getParentModule()->getSubmodule("host", id)->gate("in");
                    sendDirect(packet, dst);
                } else {
                    ++it;
                }
            }
        }

        delete connectionMessage;

    } else if (msg->getKind() == PACKET) {//пакет от другого узла
        Packet* packet = check_and_cast<Packet*>(msg);
        if (packet->getNodeIdTrg() != nodeId) {//пакет транзитный
            exit(-654);// only for testing. Remove

            cout << "MobileHost: Transit packet: nodeId = " << nodeId
                    << ", packet->getNodeIdSrc() = " << packet->getNodeIdSrc()
                    << ", packet->getNodeIdTrg() = " << packet->getNodeIdTrg() << endl;

            registerPacket(packet);

        } else {
//            cout << "MobileHost: Received packet: nodeId = " << nodeId
//                    << ", packet->getNodeIdSrc() = " << packet->getNodeIdSrc()
//                    << ", packet->getNodeIdTrg() = " << packet->getNodeIdTrg() << endl;

            destroyPacket(packet);
        }


    } else {
        exit(-333);
    }
}

Packet* MobileHost::createPacket() {
    int nodeIdTrg = generateTarget();
    Packet* packet = new Packet(nodeId, nodeIdTrg);
    packet->setCreationTime(simTime());
    packet->setKind(PACKET);
}

int MobileHost::generateTarget() {
    int nodeIdTrg = nodeId;
    while (nodeIdTrg == nodeId) {
        nodeIdTrg = (int) round(uniform(0, (double) (rd->getNumHosts() - 1)));
    }

    return nodeIdTrg;
}

void MobileHost::registerPacket(Packet* packet) {
    packetsForSending->push_back(packet);

    Request* request = new Request(nodeId, packet->getNodeIdTrg());
    request->setKind(REQUEST_FOR_ROUTING);
    sendDirect(request, rdGate);

    cMessage* newPacketCreated = new cMessage();
    newPacketCreated->setKind(NEW_PACKET_CREATED);
    sendDirect(newPacketCreated, collectorGate);

    if (nodeId == request->getNodeIdTrg()) exit(-129); // for debugging
}

void MobileHost::destroyPacket(Packet* packet) {
    packet->setReceivedTime(simTime());
    simtime_t liveTime = packet->getLiveTime();
    delete packet;

    PacketReceived* packetReceived = new PacketReceived(liveTime);
    packetReceived->setKind(PACKET_RECEIVED);
    sendDirect(packetReceived, collectorGate);

    if (nodeId != packet->getNodeIdTrg()) exit(-130); // for debugging
}
