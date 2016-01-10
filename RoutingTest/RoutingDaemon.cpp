#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "INETDefs.h"

#include <IMobility.h>
#include <clistener.h>

#include <LevyHotSpotsLATP.h>

static simsignal_t mobilityStateChangedSignal = cComponent::registerSignal("mobilityStateChanged");

class RD_Listener : public cIListener {
protected:
    int NodeID;
    Coord newposition;
public:
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, bool b);
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, long l);
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, unsigned long l);
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, double d);
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, const SimTime &t);
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, const char *s);

    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, cObject *obj);

    void processReceivedData();
};


void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, cObject *obj)
{
     if (signalID == mobilityStateChangedSignal) {
          IMobility *mobility = check_and_cast<IMobility*>(obj);
          newposition = mobility->getCurrentPosition();

          LevyHotSpotsLATP *src = check_and_cast<LevyHotSpotsLATP*>(source);
          NodeID = src->getNodeID();
          processReceivedData();
     }
}

void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, bool b)          { std::cout << "not supported"; }
void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, long l)          { std::cout << "not supported"; }
void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, unsigned long l) { std::cout << "not supported"; }
void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, double d)        { std::cout << "not supported"; }
void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, const SimTime &t){ std::cout << "not supported"; }
void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, const char *s)   { std::cout << "not supported"; }


void RD_Listener::processReceivedData()
{
   std::cout << "NodeID: " << NodeID << "  x="<< newposition.x <<" y="<< newposition.y <<endl;
}


class RoutingDaemon : public cModule {
private:
    RD_Listener *listener;

public:
    RoutingDaemon();
};

Define_Module(RoutingDaemon);

RoutingDaemon::RoutingDaemon()
{
    listener = new RD_Listener();
    subscribe( mobilityStateChangedSignal, listener);
}



