#ifndef ROUTINGDEAMON_H
#define ROUTINGDEAMON_H

#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "INETDefs.h"

#include <IMobility.h>
#include <clistener.h>

#include <LevyHotSpotsLATP.h>
#include <MobileHost.h>

using namespace std;

//---------------------------------------------RD_Listener-------------------------------------------------------------

static simsignal_t mobilityStateChangedSignal = cComponent::registerSignal("mobilityStateChanged");

class RD_Listener : public cIListener {
protected:
    int NodeId;
    Coord position;

    vector<Coord> nodePositions;
    int** connections;

public:
    RD_Listener();

    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, bool b)              { cout << "not supported"; };
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, long l)              { cout << "not supported"; };
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, unsigned long l)     { cout << "not supported"; };
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, double d)            { cout << "not supported"; };
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, const SimTime &t)    { cout << "not supported"; };
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, const char *s)       { cout << "not supported"; };

    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, cObject *obj);

    void checkReceivedData();
    void processReceivedData();
    bool isConnected(int node1, int node2);

    void log();
};

//--------------------------------------------RoutingDaemon------------------------------------------------------------

class RoutingDaemon : public cModule {
public:
    RD_Listener *listener;

    static RoutingDaemon* instance;
    static int numHosts;
    static double interconnectionRadius;

public:
    RoutingDaemon() {};

    int getNumHosts() {return numHosts;};

    virtual void initialize();
};

#endif
