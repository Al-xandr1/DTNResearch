#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "INETDefs.h"

#include <IMobility.h>
#include <clistener.h>

#include <LevyHotSpotsLATP.h>

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

    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, bool b);
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, long l);
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, unsigned long l);
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, double d);
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, const SimTime &t);
    virtual void  receiveSignal (cComponent *source, simsignal_t signalID, const char *s);

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

    static RoutingDaemon instance;
    static int numHosts;
    static double interconnectionRadius;

public:
    RoutingDaemon();
    virtual void initialize();
};

RoutingDaemon RoutingDaemon::instance;
int RoutingDaemon::numHosts;
double RoutingDaemon::interconnectionRadius;

//---------------------------------------------RD_Listener-------------------------------------------------------------
RD_Listener::RD_Listener()
{
    NodeId = -1;
    position = Coord::ZERO;

    for (int i=0; i<RoutingDaemon::numHosts; i++) nodePositions.push_back(Coord::ZERO);

    // Создаём нижнетреугольную матрицу
    connections = new int*[RoutingDaemon::numHosts];
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        connections[i] = new int[i+1];
        connections[i][i] = -1;
        for (int j=0; j<i; j++) {
            connections[i][j] = 0;
        }
    }
}

void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, cObject *obj)
{
     if (signalID == mobilityStateChangedSignal) {
          LevyHotSpotsLATP *src = check_and_cast<LevyHotSpotsLATP*>(source);
          NodeId = src->getNodeID();
          position = src->getCurrentPosition();

          checkReceivedData(); // for debugging
          processReceivedData();

          log(); // for debugging
     }
}

void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, bool b)          { std::cout << "not supported"; }
void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, long l)          { std::cout << "not supported"; }
void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, unsigned long l) { std::cout << "not supported"; }
void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, double d)        { std::cout << "not supported"; }
void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, const SimTime &t){ std::cout << "not supported"; }
void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, const char *s)   { std::cout << "not supported"; }

void RD_Listener::checkReceivedData()
{
    if (NodeId < 0 || NodeId >= RoutingDaemon::numHosts) {
        std::cout << "NodeId=" << NodeId << ", numHosts=" << RoutingDaemon::numHosts;
        exit(-987);
    }
}

void RD_Listener::processReceivedData()
{
    nodePositions[NodeId] = position;

    for (int j=0; j<NodeId; j++) connections[NodeId][j] = isConnected(NodeId, j);
    for (int i=NodeId+1; i<RoutingDaemon::numHosts; i++) connections[i][NodeId] = isConnected(i, NodeId);
}

bool RD_Listener::isConnected(int node1, int node2)
{
    Coord position1 = nodePositions[node1];
    Coord position2 = nodePositions[node2];
    double distance = position1.distance(position2);

    return distance < RoutingDaemon::interconnectionRadius;
}

void RD_Listener::log()
{
    std::cout << "All positions: " << endl;
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        std::cout << "NodeId=" << i << "  x="<< nodePositions[i].x <<" y="<< nodePositions[i].y <<endl;
    }

    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        for (int j=0; j<=i; j++) {
            std::cout << connections[i][j] << "  ";
        }
        std::cout << endl;
    }

    std::cout << endl;
}


//--------------------------------------------RoutingDaemon------------------------------------------------------------
Define_Module(RoutingDaemon);

RoutingDaemon::RoutingDaemon() {
}

void RoutingDaemon::initialize()
{
    if (instance == NULL) {
        instance = this;
    } else {
        std::cout << "Duplicate instance creation RoutingDaemon" << endl;
        exit(-789);
    }

    interconnectionRadius = par("interconnectionRadius");
    numHosts = par("numHosts");
    listener = new RD_Listener();
    subscribe(mobilityStateChangedSignal, listener);
}

