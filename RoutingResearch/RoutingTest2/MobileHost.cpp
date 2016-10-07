#include "MobileHost.h"

Define_Module(MobileHost);


void  packetHistoryOut(Packet* packet)
{
    cout<<"Source:"<<packet->getSourceId()<<"\t Destination:"<<packet->getDestinationId()<<endl;
    cout<<"Creation time:"<<packet->getCreationTime()<<endl;
    cout<<"Routing history:\n";
    for(int i=0; i<packet->IDhistory.size(); i++)
        cout<<packet->IDhistory[i]<<"\t"<<packet->ArrivalHistory[i]<<"\t"<<packet->HeuristicHistory[i]<<endl;
}


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

       case DAY_START:            // Сообщение о начале нового "дня"
           RegularRootLATP* regularMobility;
           regularMobility = dynamic_cast<RegularRootLATP*>(getSubmodule("mobility"));
           if (regularMobility) regularMobility->makeNewRoot();
           delete msg;
           break;

       case FOR_NEW_PACKET:       // Сообщение о создании нового пакета
           if ( msg->isSelfMessage() ) {
              Packet* packet = createPacket();

              packet->IDhistory.push_back(nodeId);
              packet->ArrivalHistory.push_back(simTime());
              packet->HeuristicHistory.push_back((char*)"CREATION");

              registerPacket(packet);
              scheduleAt(simTime() + timeslot * exponential(1/lambda), msg);
           }
           break;

       case PACKET:               // Пакет от другого узла. Если наш узел это пункт назначения, то пакет уничтожается, иначе посылается заявка на дальнейшую маршрутизацию
           Packet* packet;
           packet = check_and_cast<Packet*>(msg);
           if (packet->getDestinationId() == nodeId) {

               packet->IDhistory.push_back(nodeId);
               packet->ArrivalHistory.push_back(simTime());
               packet->HeuristicHistory.push_back((char*)packet->getLastHeuristric());

               destroyPacket(packet);
           }
           else {
               packet->IDhistory.push_back(nodeId);
               packet->ArrivalHistory.push_back(simTime());
               packet->HeuristicHistory.push_back((char*)packet->getLastHeuristric());

               registerPacket(packet);
           }
           break;

       case RESPONSE_FOR_REQUEST: // Ответ на запрос о маршрутизации пакета. Посылаем заготовленный пакет указанному в ответе узлу
           Response* response;
           response = check_and_cast<Response*>(msg);
           Packet*   packetForRouting;
           packetForRouting = response->getRequest()->getPacket();
           // удаляем пакет из локального буфера пакетов для отправки
           for(vector<Packet*>::iterator it = packetsForSending->begin(); it != packetsForSending->end(); ) {
              Packet* packet = (*it);
              if (packet == packetForRouting) {
                     it = packetsForSending->erase(it);
                     packet->setLastVisitedId(nodeId);

                     cGate *dst = getParentModule()->getSubmodule("host", response->getDestinationId())->gate("in");
                     sendDirect(packet, dst);
                     break;
               } else ++it;
           }
           delete response->getRequest();
           delete response;
           break;

       default:
           ASSERT(false);   //unreachable statement
           break;
    }
}


void MobileHost::finish()
{
    cout<<"\nPackets in buffer of node:"<<nodeId<<endl;
    for(vector<Packet*>::iterator it = packetsForSending->begin(); it != packetsForSending->end(); it++) {
        Packet* packet = (*it);
        packetHistoryOut(packet);
    }
    cout<<endl;
}

Packet* MobileHost::createPacket()
{
    int nodeIdTrg = generateTarget();
    Packet* packet = new Packet(nodeId, nodeIdTrg);
    packet->setCreationTime(simTime());

    NewPacketCreated* newPacketCreated = new NewPacketCreated();
    sendDirect(newPacketCreated, collectorGate);

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

    packetsForSending->push_back(packet);

    Request* request = new Request(nodeId, packet->getDestinationId(), packet);
    sendDirect(request, rdGate);
}


void MobileHost::destroyPacket(Packet* packet) {
    ASSERT(nodeId == packet->getDestinationId());

    packet->setReceivedTime(simTime());
    simtime_t liveTime = packet->getLiveTime();

    cout<<"\nPacket has reached its destination\n";
    packetHistoryOut(packet);

    delete packet;

    PacketReceived* packetReceived = new PacketReceived(liveTime);
    sendDirect(packetReceived, collectorGate);
}

