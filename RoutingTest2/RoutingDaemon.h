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
#include "RoutingHeuristic.h"

using namespace std;

static simsignal_t mobilityStateChangedSignal = cComponent::registerSignal("mobilityStateChanged");

class RoutingHeuristic;

class RoutingDaemon : public cSimpleModule {
private:
    cGate* collectorGate;

    vector<RoutingHeuristic*>* routingHeuristics;

    unsigned int currentDay;
    simtime_t startTimeOfCurrentDay;
    simtime_t finishTimeOfCurrentDay;
    double dayDuration;
    int countOfDays;

public:
    static int numHosts;
    static double interconnectionRadius;
    static bool** connections;
    static simtime_t** connectStart;
    static simtime_t** connectLost;
    static vector<simtime_t**>* connectivityPerDay;
    static vector<Request*>* requests;
    static RoutingDaemon* instance;

public:
    RoutingDaemon() {
        collectorGate = NULL;
        routingHeuristics = NULL;
        currentDay = 0;
        startTimeOfCurrentDay = 0;
        finishTimeOfCurrentDay = 0;
        dayDuration = -1;
        countOfDays = 0;
    }

    int getNumHosts() {return numHosts;}
    unsigned int getCurrentDay() {return currentDay;}
    simtime_t getStartTimeOfCurrentDay() {return startTimeOfCurrentDay;}
    double getDayDuration() { return dayDuration;}
    int getCountOfDays() { return countOfDays;}

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    void processNewDay();
    void connectionsChanged();
    bool processIfCan(Request* request);

    void calculateICT(int nodeId1, int nodeId2);
    void accumulateConnectivity(int nodeId1, int nodeId2);
    simtime_t computeTotalConnectivity(int nodeId1, int nodeId2);
    simtime_t getConnectivity(int index, int i, int j);
    simtime_t getLostConnectionTime(int nodeId1, int nodeId2);
    simtime_t getStartConnectionTime(int nodeId1, int nodeId2);
    bool isConnected(int nodeId1, int nodeId2);

    //-------------- for debug ---------------
    void log();
};

#endif
