#include "MobileHost.h"

Define_Module(MobileHost);


void MobileHost::initialize()
{
    rd = check_and_cast<RoutingDaemon*>(getParentModule()->getSubmodule("routing"));
    rdGate = rd->gate("in");
    collectorGate = getParentModule()->getSubmodule("collector")->gate("in");

    nodeId   = par("NodeID_" );
    timeslot = par("timeslot");
    lambda   = par("lambda"  );

    packetsForSending = new vector<Packet*>();

    scheduleAt(simTime(), new cMessage("FOR_NEW_PACKET", FOR_NEW_PACKET));
}


void MobileHost::handleMessage(cMessage *msg)
{
    switch(msg->getKind()) {

       case DAY_START: {           // Сообщение о начале нового "дня"
           RegularRootLATP* regularMobility = getMobility();
           if (regularMobility && rd->getCurrentDay() > 1) regularMobility->makeNewRoot();
           delete msg;
           break;
       }

       case FOR_NEW_PACKET: {      // Сообщение о создании нового пакета
           if ( msg->isSelfMessage() ) {
              Packet* packet = createPacket();
              registerPacket(packet);
              scheduleAt(simTime() + timeslot * exponential(1/lambda), msg);
           }
           break;
       }

       case PACKET: {              // Пакет от другого узла. Если наш узел это пункт назначения, то пакет уничтожается, иначе посылается заявка на дальнейшую маршрутизацию
           Packet* packet = check_and_cast<Packet*>(msg);

           if (packet->getDestinationId() == nodeId) destroyPacket(packet);
           else registerPacket(packet);

           break;
       }

       case RESPONSE_FOR_REQUEST: { // Ответ на запрос о маршрутизации пакета. Посылаем заготовленный пакет указанному в ответе узлу
           Response* response = check_and_cast<Response*>(msg);
           Packet*   packetForRouting = response->getRequest()->getPacket();
           // удаляем пакет из локального буфера пакетов для отправки
           for(vector<Packet*>::iterator it = packetsForSending->begin(); it != packetsForSending->end(); ) {
              Packet* packet = (*it);
              if (packet == packetForRouting) {
                  it = packetsForSending->erase(it);
                  sendPacket(packet, response->getDestinationId());
                  break;
               } else ++it;
           }
           delete response->getRequest();
           delete response;
           break;
       }

       default: {
           ASSERT(false);   //unreachable statement
           break;
       }
    }
}


void MobileHost::finish()
{
    cout<<"\nPackets in buffer of node:"<<nodeId<<endl;
    for(vector<Packet*>::iterator it = packetsForSending->begin(); it != packetsForSending->end(); it++) {
        Packet* packet = (*it);
        packet->collectRemoved(nodeId, getMobility()->getLastPosition());

        //todo сделать сохранение статистики пакетов, недошедших до узлов назначения
        packet->printHistory();
    }
    cout<<endl;
}


Packet* MobileHost::createPacket()
{
    Packet* packet = new Packet(nodeId, generateTarget());
    packet->collectCreated(nodeId, getMobility()->getLastPosition());

    sendDirect(new NewPacketCreated(), collectorGate);

    return packet;
}


int MobileHost::generateTarget()
{
    int nodeIdTrg = nodeId;
    while (nodeIdTrg == nodeId)  nodeIdTrg = rand() %  rd->getNumHosts();
    return nodeIdTrg;
}


void MobileHost::registerPacket(Packet* packet)
{
    ASSERT(nodeId != packet->getDestinationId());

    packet->setReceivedTime(simTime());
    packet->collectRegistered(nodeId, getMobility()->getLastPosition());

    packetsForSending->push_back(packet);

    sendDirect(new Request(nodeId, packet->getDestinationId(), packet), rdGate);
}


void MobileHost::sendPacket(Packet* packet, int destinationId)
{
    ASSERT(nodeId != packet->getDestinationId());

    packet->setLastVisitedId(nodeId);
    packet->collectBeforeSend(nodeId, getMobility()->getLastPosition());

    cGate *dst = getParentModule()->getSubmodule("host", destinationId)->gate("in");
    sendDirect(packet, dst);
}


void MobileHost::destroyPacket(Packet* packet)
{
    ASSERT(nodeId == packet->getDestinationId());

    packet->setReceivedTime(simTime());
    packet->collectDelivered(nodeId, getMobility()->getLastPosition());
    simtime_t liveTime = packet->getLiveTime();

    //todo сделать сохранение статистики пакетов, дошедших до узлов назначения
    cout<<"\nPacket has reached its destination\n";
    packet->printHistory();

    delete packet;

    sendDirect(new PacketReceived(liveTime), collectorGate);
}

