#ifndef MOBILEHOST_H
#define MOBILEHOST_H

#include <stdlib.h>
#include <math.h>
#include "INETDefs.h"
#include <RoutingDaemon.h>
#include "RegularRealMobility.h"
#include "RegularRootLATP.h"
#include "RegularSelfSimLATP.h"
#include "RegularSimpleLevyMobility.h"
#include <HistoryCollector.h>
#include <Messages.h>

using namespace std;

class RoutingDaemon;
class RegularRootLATP;
class RegularSelfSimLATP;
class RegularRealMobility;
class RegularSimpleLevyMobility;

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

    MovingMobilityBase*        getMobility();
    RegularRootLATP*           getRegularRootLATPMobility();
    RegularSelfSimLATP*        getRegularSelfSimLATPMobility();
    RegularRealMobility*       getRegularRealMobility();
    RegularSimpleLevyMobility* getRegularSimpleLevyMobility();

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
