#ifndef MOBILEHOST_H
#define MOBILEHOST_H

#include <stdlib.h>
#include <math.h>

#include "INETDefs.h"

#include <RoutingDaemon.h>
#include "RegularRootLATP.h"
#include <Messages.h>

using namespace std;

class RoutingDaemon;

class MobileHost : public cSimpleModule
{
private:
    RoutingDaemon* rd;
    cGate* rdGate;
    cGate* collectorGate;

    int nodeId;

    double lambda;
    simtime_t timeslot;

    vector<Packet*>* packetsForSending;

public:
    MobileHost(){};

    int getNodeId() {return nodeId;};
    vector<Packet*>* getPacketsForSending() {return packetsForSending;};

    Packet* createPacket();
    int generateTarget();
    void registerPacket(Packet* packet);
    void destroyPacket(Packet* packet);

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif