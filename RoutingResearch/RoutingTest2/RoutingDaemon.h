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
    vector<RoutingHeuristic*>* routingHeuristics;

    unsigned int currentDay;
    simtime_t    startTimeOfCurrentDay;
    simtime_t    finishTimeOfCurrentDay;
    unsigned int countOfDays;
    bool         useCODForStat;

    vector<int>*  neighbors;

public:
    static double      dayDuration;
    static int         numHosts;
    static double      interconnectionRadius;
    static bool**      connections;
    static simtime_t** connectStart;
    static simtime_t** connectLost;
    static simtime_t** sumOfConnectDuration;
    static vector<simtime_t**>* connectivityPerDay;
    static vector<Request*>*    requests;
    static RoutingDaemon*       instance;

public:
    RoutingDaemon() {
        routingHeuristics = NULL;
        currentDay        = 0;
        startTimeOfCurrentDay  = 0;
        finishTimeOfCurrentDay = 0;
        dayDuration       = -1;
        countOfDays       = 0;
        useCODForStat     = FALSE;
        neighbors         = NULL;
    }

    int          getNumHosts()      {return numHosts;}
    unsigned int getCurrentDay()    {return currentDay;}
    simtime_t    getStartTimeOfCurrentDay() {return startTimeOfCurrentDay;}
    double       getDayDuration()   {return dayDuration;}
    unsigned int getCountOfDays()   {return countOfDays;}
    bool         getUseCODForStat() {return useCODForStat;}

    void matricesCreation();
    void matricesInitialization();
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);

    bool canCollectStatistics();
    void processNewDay();
    void connectionsChanged();
    bool processIfCan(Request* request);

    void      calculateICT( int nodeId1, int nodeId2 );
    simtime_t computeTotalConnectivity( int nodeId1, int nodeId2 );
    simtime_t getConnectivity( int index, int i, int j );
    simtime_t getLostConnectionTime( int nodeId1, int nodeId2 );
    simtime_t getStartConnectionTime( int nodeId1, int nodeId2 );
    bool      isConnected( int nodeId1, int nodeId2 );

    //-------------- for debug ---------------
    void log();
};

#endif
