#include "MobileHost.h"

Define_Module(MobileHost);

void MobileHost::initialize()
{
    rd = check_and_cast<RoutingDaemon*>(getParentModule()->getSubmodule("routing"));
    rdGate = rd->gate("in");

    nodeId       = par("NodeID_" );
    timeslot     = par("timeslot");
    lambda       = par("lambda"  );
    newPacketMsg = NULL;

    packetsForSending = new vector<Packet*>();
}


void MobileHost::handleMessage(cMessage *msg)
{
    switch(msg->getKind()) {

       case DAY_START: {           // Сообщение о начале нового "дня"
           startRoute();
           delete msg;
           break;
       }

       case FOR_NEW_PACKET: {      // Сообщение о создании нового пакета
           if ( msg->isSelfMessage() ) {
              Packet* packet = createPacket();
              registerPacket(packet);
              scheduleAt(simTime() + timeslot * exponential(1/lambda), msg);
           } else {
               ASSERT(false); //unreachable statement
           };
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
    for(vector<Packet*>::iterator it = packetsForSending->begin(); it != packetsForSending->end(); it++) {
        Packet* packet = (*it);
        HistoryCollector::insertRowRemoved(packet, nodeId, getMobility()->getCurrentPosition());
        HistoryCollector::collectRemovedPacket(packet);
        //todo made erasing & packet deletion
    }

    //так как при окончании маршрута сразу стартует новый, в конце его нужно принудительно закончить
    if (newPacketMsg) endRoute();
    if (nodeId == rd->getNumHosts()-1) HistoryCollector::finish();
}


MovingMobilityBase* MobileHost::getMobility()                {return (MovingMobilityBase*)getSubmodule("mobility");}
RegularRootLATP*    MobileHost::getRegularRootLATPMobility() {return dynamic_cast<RegularRootLATP*>(getSubmodule("mobility"));}
SelfSimLATP*        MobileHost::getSelfSimLATPMobility()     {return dynamic_cast<SelfSimLATP*>(getSubmodule("mobility"));}


void MobileHost::startRoute()
{
//    ASSERT(!newPacketMsg);
//    ASSERT(rd->getCurrentDay() >= 1);

    RegularRootLATP* regularMobility = getRegularRootLATPMobility();
    //для первого дня маршрут построен при инициализации мобильности
    if (regularMobility && rd->getCurrentDay() > 1) regularMobility->makeNewRoot();

    // включение генерации пакетов
    newPacketMsg = new cMessage("FOR_NEW_PACKET", FOR_NEW_PACKET);
    scheduleAt(simTime(), newPacketMsg);
}


void MobileHost::endRoute()
{
//    ASSERT(newPacketMsg);
//    ASSERT(rd->getCurrentDay() >= 1);

    HistoryCollector::insertRouteInfo(nodeId, rd->getCurrentDay(), rd->getStartTimeOfCurrentDay(), simTime());

    // отключение генерации пакетов
    cMessage* canceled = cancelEvent(newPacketMsg);
//    ASSERT(canceled == newPacketMsg);
    delete newPacketMsg;
    newPacketMsg = NULL;
}


Packet* MobileHost::createPacket()
{
    Packet* packet = new Packet(nodeId, generateTarget());
    HistoryCollector::insertRowCreated(packet, nodeId, getMobility()->getCurrentPosition());
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
//    ASSERT(nodeId != packet->getDestinationId());

    packet->setReceivedTime(simTime());
    HistoryCollector::insertRowRegistered(packet, nodeId, getMobility()->getCurrentPosition());

    packetsForSending->push_back(packet);

    sendDirect(new Request(nodeId, packet->getDestinationId(), packet), rdGate);
}


void MobileHost::sendPacket(Packet* packet, int destinationId)
{
//    ASSERT(nodeId != packet->getDestinationId());

    packet->setLastVisitedId(nodeId);
    HistoryCollector::insertRowBeforeSend(packet, nodeId, getMobility()->getCurrentPosition());

    cGate *dst = getParentModule()->getSubmodule("host", destinationId)->gate("in");
    sendDirect(packet, dst);
}


void MobileHost::destroyPacket(Packet* packet)
{
//    ASSERT(nodeId == packet->getDestinationId());

    packet->setReceivedTime(simTime());
    HistoryCollector::insertRowDelivered(packet, nodeId, getMobility()->getCurrentPosition());
    HistoryCollector::collectDeliveredPacket(packet);
    delete packet;
}

