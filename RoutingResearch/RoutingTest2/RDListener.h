#ifndef RDLISTENER_H
#define RDLISTENER_H

#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>
#include "INETDefs.h"
#include <clistener.h>
#include "RoutingDaemon.h"
#include <LevyHotSpotsLATP.h>
#include "SelfSimLATP.h"
#include <MobileHost.h>
#include "DevelopmentHelper.h"

using namespace std;

class RoutingDaemon;

class RD_Listener : public cIListener {
protected:
    RoutingDaemon* rd;

    int NodeId;
    Coord position;

    vector<Coord> nodePositions;

public:
    RD_Listener(RoutingDaemon* rd);

    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, bool b)              { cout << "not supported"; };
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, long l)              { cout << "not supported"; };
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, unsigned long l)     { cout << "not supported"; };
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, double d)            { cout << "not supported"; };
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, const SimTime &t)    { cout << "not supported"; };
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, const char *s)       { cout << "not supported"; };

    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, cObject *obj);

    bool processReceivedData();
    bool isConnected(int node1, int node2);

    //-------------- for debug ---------------
    bool checkReceivedData();
    void log();
};

#endif
