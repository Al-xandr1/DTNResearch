#ifndef MOBILEHOST_H
#define MOBILEHOST_H

#include <stdlib.h>
#include <math.h>
#include "INETDefs.h"
#include <RoutingDaemon.h>
#include "RegularRootLATP.h"
#include "SelfSimLATP.h"
#include <Messages.h>
#include <HistoryCollector.h>

using namespace std;

class RoutingDaemon;
class RegularRootLATP;

class MobileHost : public cSimpleModule
{
private:
    RoutingDaemon* rd;
    cGate* rdGate;

    int nodeId;

    double lambda;
    simtime_t timeslot;
    cMessage* newPacketMsg;

    vector<Packet*>* packetsForSending;

public:
    MobileHost(){};

    int getNodeId() {return nodeId;};
    vector<Packet*>* getPacketsForSending() {return packetsForSending;};

    MovingMobilityBase* getMobility();
    RegularRootLATP*    getRegularRootLATPMobility();
    SelfSimLATP*        getSelfSimLATPMobility();

    void startRoute();
    void ensureEndRoute();
    void endRoute();
    Packet* createPacket();
    int generateTarget();
    void registerPacket(Packet* packet);
    void sendPacket(Packet* packet, int destinationId);
    void destroyPacket(Packet* packet);

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
