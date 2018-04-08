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
    bool         useCountOfDaysForStat;

    vector<int>* neighbors;

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

    // PROPHET data -----------------------------------------------------------
    static double**    P_prophet;
    static simtime_t*  U_prophet;
    static unsigned**  C_prophet;
    // PROPHET parameters -----------------------------------------------------
    static double P_encounter_max;
    static double P_encounter_first;
    static double P_first_threshold;
    static double alpha_prophet;
    static double beta_prophet;
    static double gamma_prophet;
    static double delta_prophet;
    static double I_typ;
    static int several;
    // ------------------------------------------------------------------------

public:
    RoutingDaemon() {
        routingHeuristics      = NULL;
        currentDay             = 0;
        startTimeOfCurrentDay  = 0;
        finishTimeOfCurrentDay = 0;
        dayDuration            = -1;
        countOfDays            = 0;
        useCountOfDaysForStat  = FALSE;
        neighbors              = NULL;
    }

    unsigned int getCurrentDay() {
        // Костыль. Нумерация дней начинаеся с 1, а тут мв обращаемся до его проставления в 1
        if (currentDay == 0) return 1;
        return currentDay;
    }
    int          getNumHosts()              {return numHosts;}
    simtime_t    getStartTimeOfCurrentDay() {return startTimeOfCurrentDay;}
    double       getDayDuration()           {return dayDuration;}
    unsigned int getCountOfDays()           {return countOfDays;}
    bool         getUseCountOfDaysForStat() {return useCountOfDaysForStat;}

    void matricesCreation();
    void matricesInitialization();
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

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

    // PROPHET methods ------------------------------------------------------------------
    void PROPHET_connection_starts(int node1, int node2);
    void PROPHET_connection_ends(int node1, int node2);
    void PROPHET_timer_processing();
    void PROPHET_aging_P(int node1, int node2, simtime_t lastUpdate);
    void PROPHET_growing_P(int node1, int node2, simtime_t lastContact);
    double PROPHET_transitivity_P(int node1, int node2, int node3);
    // ----------------------------------------------------------------------------------

    //-------------- for debug ---------------
    void log();
};

#endif
