#ifndef ROUTINGDEAMON_H
#define ROUTINGDEAMON_H

#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "INETDefs.h"

using namespace std;

class RoutingDaemon : public cSimpleModule {
public:
    cGate* in;

    static int numHosts;
    static double interconnectionRadius;
    static bool** connections;
    static simtime_t** connectStart;
    static simtime_t** connectLost;

public:
    RoutingDaemon() {};

    int getNumHosts() {return numHosts;};

    virtual void initialize();

    virtual void handleMessage(cMessage *msg);
};

#endif
