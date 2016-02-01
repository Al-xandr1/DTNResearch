#ifndef MOBILEHOST_H
#define MOBILEHOST_H

#include <stdlib.h>
#include <math.h>

#include "INETDefs.h"

#include <RoutingDaemon.h>
#include <Messages.h>

using namespace std;

class RoutingDaemon;

class MobileHost : public cSimpleModule
{
private:
    RoutingDaemon* rd;

    int nodeId;

    double lambda;
    simtime_t timeslot;

    vector<Packet*>* packetsForSending;

public:
    MobileHost(){};

    int getNodeId() {return nodeId;};
    vector<Packet*>* getPacketsForSending() {return packetsForSending;};
    void sendPacketTo(Packet* packet, int nodeId);

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
