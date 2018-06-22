#include "MobileHost.h"

Define_Module(MobileHost);

void MobileHost::initialize() {
    rd = check_and_cast<RoutingDaemon*>(getParentModule()->getSubmodule("routing"));
    rdGate = rd->gate("in");

    nodeId       = par("NodeID_" );
    timeslot     = par("timeslot");
    lambda       = par("lambda"  );
    newPacketMsg = NULL;

    packetsForSending = new vector<Packet*>();
}


void MobileHost::handleMessage(cMessage *msg) {
    switch(msg->getKind()) {

       case DAY_START: {           // ��������� � ������ ������ "���"
           startRoute();
           delete msg;
           break;
       }

       case FOR_NEW_PACKET: {      // ��������� � �������� ������ ������
           // ����� ���� "����" �� �� ����� ������������ ������
           ASSERT(isTurnedOn());

           if ( msg->isSelfMessage() ) {
               if (lambda > 0) {
                   Packet* packet = createPacket();
                   registerPacket(packet);
                   scheduleAt(simTime() + timeslot * exponential(1/lambda), msg);
               }
           } else {
               ASSERT(false); //unreachable statement
           };
           break;
       }

       case PACKET: {              // ����� �� ������� ����. ���� ��� ���� ��� ����� ����������, �� ����� ������������, ����� ���������� ������ �� ���������� �������������
           // ����� ���� "����" ��� �� ������ ��������� ������
           // todo ASSERT(isTurnedOn());

           Packet* packet = check_and_cast<Packet*>(msg);
           if (packet->getDestinationId() == nodeId) destroyPacket(packet);
           else registerPacket(packet);
           break;
       }

       case RESPONSE_FOR_REQUEST: { // ����� �� ������ � ������������� ������. �������� ������������� ����� ���������� � ������ ����
           Response* response = check_and_cast<Response*>(msg);
           Request*  request = response->getRequest();

           // todo if (isTurnedOn()) {
           bool wasSend = false;
           // ������� ����� �� ���������� ������ ������� ��� ��������
           Packet*   packetForRouting = request->getPacket();
           for(vector<Packet*>::iterator it = packetsForSending->begin(); it != packetsForSending->end(); ) {
               Packet* packet = (*it);
               if (packet == packetForRouting) {
                   wasSend = sendPacket(packet, response->getDestinationId());
                   if (wasSend) it = packetsForSending->erase(it);
                   break;
               } else ++it;
           }
           ASSERT(wasSend); // todo remove
           if (wasSend) {
               delete request;
               delete response;
           } else {
               //todo �������, ��������� ���� ��-�� �������� ��������� updateInterval. Todo ��������� �����...
               take(request);
               sendDirect(request, rdGate);
               delete response;
           }

           //} else {
           //    // ����� ���� "����" ��� �� ������ ��������� �������� ��������� �� �������������
           //    // ������ � ���� �������� ��������� updateInterval ���������� � "���" ��� ����� �� ����� �� RDListener'�.
           //
           //    // ������� ����� "���������": ������������ ������ �� �������������, �.�. �� ����� ���������� ������. Todo ��������� �����...
           //    take(request);
           //    sendDirect(request, rdGate);
           //    delete response;
           //}
           break;
       }

       case END_ROUTE: {
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


void MobileHost::finish() {
    for(vector<Packet*>::iterator it = packetsForSending->begin(); it != packetsForSending->end(); it++) {
        Packet* packet = (*it);
        HistoryCollector::insertRowRemoved(packet, nodeId, getMobility()->getCurrentPosition());
        HistoryCollector::collectPacket(packet);
        myDelete(packet);
    }
    myDelete(packetsForSending);

    //��� ��� ��� ��������� �������� ����� �������� �����, � ����� ��� ����� ������������� ���������
    ensureEndRoute();
    if (nodeId == rd->getNumHosts()-1) HistoryCollector::finish();
}


MovingMobilityBase*        MobileHost::getMobility()                   {return (MovingMobilityBase*)getSubmodule("mobility");}
RegularRootLATP*           MobileHost::getRegularRootLATPMobility()    {return dynamic_cast<RegularRootLATP*>(getSubmodule("mobility"));}
RegularSelfSimLATP*        MobileHost::getRegularSelfSimLATPMobility() {return dynamic_cast<RegularSelfSimLATP*>(getSubmodule("mobility"));}
RegularRealMobility*       MobileHost::getRegularRealMobility()        {return dynamic_cast<RegularRealMobility*>(getSubmodule("mobility"));}
RegularSimpleLevyMobility* MobileHost::getRegularSimpleLevyMobility()  {return dynamic_cast<RegularSimpleLevyMobility*>(getSubmodule("mobility"));}


void MobileHost::startRoute() {
    ASSERT(rd->getCurrentDay() >= 1);

    turnOn();

    // ������������ ��� "�����" ��� �����������, ����� ����� ������ ������
    sendDirect(new cMessage("MOBILITY_START", MOBILITY_START), getSubmodule("mobility")->gate("in"));
}


void MobileHost::ensureEndRoute() {
    if (isTurnedOn()) endRoute();
}


void MobileHost::endRoute() {
    ASSERT(rd->getCurrentDay() >= 1);

    HistoryCollector::insertRouteInfo(nodeId, rd->getCurrentDay(), rd->getStartTimeOfCurrentDay(), simTime());

    turnOff();

    //���������� ����������� � "����������" ���� (���������� ����� ������)
    if (getRegularRootLATPMobility()) getRegularRootLATPMobility()->nodeTurnedOff();
    else if (getRegularSelfSimLATPMobility()) getRegularSelfSimLATPMobility()->nodeTurnedOff();
    else if (getRegularRealMobility()) getRegularRealMobility()->nodeTurnedOff();
    else if (getRegularSimpleLevyMobility()) getRegularSimpleLevyMobility()->nodeTurnedOff();
    else ASSERT(false); //unreachable statement
}


void MobileHost::turnOn() {
    ASSERT(!newPacketMsg);
    // ��������� ��������� �������
    newPacketMsg = new cMessage("FOR_NEW_PACKET", FOR_NEW_PACKET);
    scheduleAt(simTime(), newPacketMsg);
}


void MobileHost::turnOff() {
    ASSERT(newPacketMsg);
    // ���������� ��������� �������
    cMessage* canceled = cancelEvent(newPacketMsg);
    ASSERT(canceled == newPacketMsg);
    myDelete(newPacketMsg);
}


bool MobileHost::isTurnedOn() {
    return newPacketMsg != NULL;
}


Packet* MobileHost::createPacket() {
    Packet* packet = new Packet(nodeId, generateTarget());
    HistoryCollector::insertRowCreated(packet, nodeId, getMobility()->getCurrentPosition());
    return packet;
}


int MobileHost::generateTarget() {
    int nodeIdTrg = nodeId;
    while (nodeIdTrg == nodeId)  nodeIdTrg = rand() %  rd->getNumHosts();
    return nodeIdTrg;
}


void MobileHost::registerPacket(Packet* packet) {
    ASSERT(nodeId != packet->getDestinationId());

    packet->setReceivedTime(simTime());
    HistoryCollector::insertRowRegistered(packet, nodeId, getMobility()->getCurrentPosition());

    packetsForSending->push_back(packet);

    sendDirect(new Request(nodeId, packet->getDestinationId(), packet), rdGate);
}


bool MobileHost::sendPacket(Packet* packet, int destinationId) {
    ASSERT(nodeId != packet->getDestinationId());

    // �������, ��������� ���� ��-�� �������� ��������� updateInterval. Todo ��������� �����...
    // todo if (!check_and_cast<MobileHost*>(getParentModule()->getSubmodule("host", destinationId))->isTurnedOn()) return false;

    packet->setLastVisitedId(nodeId);
    HistoryCollector::insertRowBeforeSend(packet, nodeId, getMobility()->getCurrentPosition());

    cGate *dst = getParentModule()->getSubmodule("host", destinationId)->gate("in");
    sendDirect(packet, dst);

    return true;
}


void MobileHost::destroyPacket(Packet* packet) {
    ASSERT(nodeId == packet->getDestinationId());

    packet->setReceivedTime(simTime());
    HistoryCollector::insertRowDelivered(packet, nodeId, getMobility()->getCurrentPosition());
    HistoryCollector::collectPacket(packet);
    delete packet;
}

