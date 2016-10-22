#ifndef MOBILEHOST_H
#define MOBILEHOST_H

#include <stdlib.h>
#include <math.h>
#include "INETDefs.h"
#include <RoutingDaemon.h>
#include "RegularRootLATP.h"
#include <Messages.h>
#include <HistoryCollector.h>

using namespace std;

class RoutingDaemon;

class MobileHost : public cSimpleModule
{
private:
    RoutingDaemon* rd;
    cGate* rdGate;

    int nodeId;

    double lambda;
    simtime_t timeslot;

    vector<Packet*>* packetsForSending;

public:
    MobileHost(){};

    int getNodeId() {return nodeId;};
    vector<Packet*>* getPacketsForSending() {return packetsForSending;};
    RegularRootLATP* getMobility() {return dynamic_cast<RegularRootLATP*>(getSubmodule("mobility"));}

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
