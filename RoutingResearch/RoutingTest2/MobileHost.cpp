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

       case DAY_START: {           // ��������� � ������ ������ "���"
           startRoute();
           delete msg;
           break;
       }

       case FOR_NEW_PACKET: {      // ��������� � �������� ������ ������
           if ( msg->isSelfMessage() ) {
              Packet* packet = createPacket();
              registerPacket(packet);
              if (lambda>0) scheduleAt(simTime() + timeslot * exponential(1/lambda), msg);
           } else {
               ASSERT(false); //unreachable statement
           };
           break;
       }

       case PACKET: {              // ����� �� ������� ����. ���� ��� ���� ��� ����� ����������, �� ����� ������������, ����� ���������� ������ �� ���������� �������������
           Packet* packet = check_and_cast<Packet*>(msg);

           if (packet->getDestinationId() == nodeId) destroyPacket(packet);
           else registerPacket(packet);

           break;
       }

       case RESPONSE_FOR_REQUEST: { // ����� �� ������ � ������������� ������. �������� ������������� ����� ���������� � ������ ����
           Response* response = check_and_cast<Response*>(msg);
           Packet*   packetForRouting = response->getRequest()->getPacket();
           // ������� ����� �� ���������� ������ ������� ��� ��������
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

       case ROUTE_ENDED: { // ��������� ���������� �� ����������� RealMobility
           endRoute();
           delete msg;
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
        HistoryCollector::collectPacket(packet);
        //todo made erasing & packet deletion
    }

    //��� ��� ��� ��������� �������� ����� �������� �����, � ����� ��� ����� ������������� ���������
    ensureEndRoute();
    if (nodeId == rd->getNumHosts()-1) HistoryCollector::finish();
}


MovingMobilityBase* MobileHost::getMobility()                {return (MovingMobilityBase*)getSubmodule("mobility");}
RegularRootLATP*    MobileHost::getRegularRootLATPMobility() {return dynamic_cast<RegularRootLATP*>(getSubmodule("mobility"));}
SelfSimLATP*        MobileHost::getSelfSimLATPMobility()     {return dynamic_cast<SelfSimLATP*>(getSubmodule("mobility"));}


void MobileHost::startRoute()
{
    ASSERT(!newPacketMsg);
    ASSERT(rd->getCurrentDay() >= 1);

    RegularRootLATP* regularMobility = getRegularRootLATPMobility();
    // ��� ������� ��� ������� �������� ��� ������������� �����������
    if (regularMobility && rd->getCurrentDay() > 1) regularMobility->makeNewRoot();
    // ������������ ��� "�����" ��� �����������, ����� ����� ������ ������
    sendDirect(new cMessage("Start mobility", MOBILITY_START), getSubmodule("mobility")->gate("in"));

    // ��������� ��������� �������
    newPacketMsg = new cMessage("FOR_NEW_PACKET", FOR_NEW_PACKET);
    scheduleAt(simTime(), newPacketMsg);
}


void MobileHost::ensureEndRoute() {
    if (newPacketMsg) endRoute();
}


void MobileHost::endRoute()
{
    ASSERT(newPacketMsg);
    ASSERT(rd->getCurrentDay() >= 1);

    HistoryCollector::insertRouteInfo(nodeId, rd->getCurrentDay(), rd->getStartTimeOfCurrentDay(), simTime());

    // ���������� ��������� �������
    cMessage* canceled = cancelEvent(newPacketMsg);
    ASSERT(canceled == newPacketMsg);
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
    ASSERT(nodeId != packet->getDestinationId());

    packet->setReceivedTime(simTime());
    HistoryCollector::insertRowRegistered(packet, nodeId, getMobility()->getCurrentPosition());

    packetsForSending->push_back(packet);

    sendDirect(new Request(nodeId, packet->getDestinationId(), packet), rdGate);
}


void MobileHost::sendPacket(Packet* packet, int destinationId)
{
    ASSERT(nodeId != packet->getDestinationId());

    packet->setLastVisitedId(nodeId);
    HistoryCollector::insertRowBeforeSend(packet, nodeId, getMobility()->getCurrentPosition());

    cGate *dst = getParentModule()->getSubmodule("host", destinationId)->gate("in");
    sendDirect(packet, dst);
}


void MobileHost::destroyPacket(Packet* packet)
{
    ASSERT(nodeId == packet->getDestinationId());

    packet->setReceivedTime(simTime());
    HistoryCollector::insertRowDelivered(packet, nodeId, getMobility()->getCurrentPosition());
    HistoryCollector::collectPacket(packet);
    delete packet;
}

