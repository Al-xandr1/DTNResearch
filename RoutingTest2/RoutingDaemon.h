#ifndef ROUTINGDEAMON_H
#define ROUTINGDEAMON_H

#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "INETDefs.h"
#include "MobileHost.h"
#include "RDListener.h"
#include "Messages.h"

using namespace std;

static simsignal_t mobilityStateChangedSignal = cComponent::registerSignal("mobilityStateChanged");

class RoutingDaemon : public cSimpleModule {
private:
    cGate* collectorGate;

    unsigned int currentDay;
    double dayDuration;

public:
    cGate* in;

    static int numHosts;
    static double interconnectionRadius;
    static bool** connections;
    static simtime_t** connectStart;
    static simtime_t** connectLost;
    static vector<Request*>* requests;
    static RoutingDaemon* instance;

public:
    RoutingDaemon() {
        collectorGate = NULL;
        in = NULL;
        dayDuration = -1;
        currentDay = 0;
    }

    int getNumHosts() {return numHosts;}

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    simtime_t getLostConnectionTime(int nodeId1, int nodeId2);
    simtime_t getStartConnectionTime(int nodeId1, int nodeId2);
    bool isConnected(int nodeId1, int nodeId2);
    bool processIfCan(Request* request);
    void process(int nodeId, Request* request);
    void calculateICT(int i, int j, simtime_t oldStart, simtime_t oldLost, simtime_t newStart);
    void connectionsChanged();

    void log();
};

#endif
