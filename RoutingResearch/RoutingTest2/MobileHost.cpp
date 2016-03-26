#include "MobileHost.h"

Define_Module(MobileHost);


void MobileHost::initialize()
{
    rd = check_and_cast<RoutingDaemon*>(getParentModule()->getSubmodule("routing"));
    rdGate = rd->gate("in");
    collectorGate = getParentModule()->getSubmodule("collector")->gate("in");

    nodeId = par("NodeID_");
    timeslot = par("timeslot");
    lambda   = par("lambda");

    packetsForSending = new vector<Packet*>();

    scheduleAt(simTime(), new cMessage("FOR_NEW_PACKET", FOR_NEW_PACKET));
};

void MobileHost::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage() && msg->getKind() == FOR_NEW_PACKET) {
        // Сообщение о создании нового пакета
        Packet* packet = createPacket();
        registerPacket(packet);

        scheduleAt(simTime() + timeslot * exponential(1/lambda), msg);


    } else if (msg->getKind() == RESPONSE_FOR_REQUEST) {
        // Ответ на запрос о маршрутизации пакета. Посылаем заготовлденный пакет указанному в ответе узлу
        Response* response = check_and_cast<Response*>(msg);

        Packet* packetForRouting = response->getRequest()->getPacket();
        // удаляем пакет из локального буфера пакетов для отправки
        for(vector<Packet*>::iterator it = packetsForSending->begin(); it != packetsForSending->end(); ) {
            Packet* packet = (*it);
            if (packet == packetForRouting) {
                it = packetsForSending->erase(it);
                packet->setLastVisitedId(nodeId);

                cGate *dst = getParentModule()->getSubmodule("host", response->getDestinationId())->gate("in");
                sendDirect(packet, dst);
                break;
            } else {
                ++it;
            }
        }
        delete response->getRequest();
        delete response;


    } else if (msg->getKind() == PACKET) {
        // Пакет от другого узла. Если это пунк назначения, то пакет уничтожается, иначе посылается заявка на дальнейшую маршрутизацию
        Packet* packet = check_and_cast<Packet*>(msg);
        if (packet->getDestinationId() != nodeId) {//пакет транзитный
            //            cout << "MobileHost: Transit packet: nodeId = " << nodeId
            //                    << ", packet->getNodeIdSrc() = " << packet->getNodeIdSrc()
            //                    << ", packet->getNodeIdTrg() = " << packet->getNodeIdTrg() << endl;
            registerPacket(packet);

        } else {
            //            cout << "MobileHost: Received packet: nodeId = " << nodeId
            //                    << ", packet->getNodeIdSrc() = " << packet->getNodeIdSrc()
            //                    << ", packet->getNodeIdTrg() = " << packet->getNodeIdTrg() << endl;
            destroyPacket(packet);
        }


    } else if (msg->getKind() == DAY_START) {
        // Сообщение о начале нового "дня"
        //cout << "Day started for node: " << nodeId << endl;
        RegularRootLATP* regularMobility = dynamic_cast<RegularRootLATP*>(getSubmodule("mobility"));
        if (regularMobility) regularMobility->makeNewRoot();
        delete msg;


    } else {
        exit(-333);
    }
}

Packet* MobileHost::createPacket() {
    int nodeIdTrg = generateTarget();
    Packet* packet = new Packet(nodeId, nodeIdTrg);
    packet->setCreationTime(simTime());

    NewPacketCreated* newPacketCreated = new NewPacketCreated();
    sendDirect(newPacketCreated, collectorGate);

    return packet;
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

    Request* request = new Request(nodeId, packet->getDestinationId(), packet);
    sendDirect(request, rdGate);

    //if (nodeId == request->getNodeIdTrg()) exit(-129); // for debugging
}

void MobileHost::destroyPacket(Packet* packet) {
    packet->setReceivedTime(simTime());
    simtime_t liveTime = packet->getLiveTime();
    delete packet;

    PacketReceived* packetReceived = new PacketReceived(liveTime);
    sendDirect(packetReceived, collectorGate);

    //if (nodeId != packet->getNodeIdTrg()) exit(-130); // for debugging
}
