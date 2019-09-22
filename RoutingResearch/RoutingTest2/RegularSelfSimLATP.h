#ifndef REGULARSELFSIMLATP_H_
#define REGULARSELFSIMLATP_H_

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <windows.h>
#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"
#include "cmessage.h"
#include "DevelopmentHelper.h"
#include "LeviStatic.h"
#include "HotSpotsCollection.h"
#include "RootsCollection.h"
#include "SelfSimLATP.h"
#include "MobileHost.h"
#include "Messages.h"

using namespace std;

class RegularSelfSimLATP : public SelfSimLATP {

private:
    //statistics collection for ONE day (for repeat)
    MovementHistory* mvnHistoryForRepeat;
    bool repetitionOfTraceEnabled;

    simtime_t timeOffset;
    double distance;
    double speed;
    simtime_t travelTime;

protected:
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/
    virtual void handleMessage(cMessage *message); /** @brief Overridden from SelfSimLATP.*/
    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual bool generatePause(simtime_t &nextChange); /** @brief Overridden from SelfSimLATP.*/
    virtual bool generateNextPosition(Coord &targetPosition, simtime_t &nextChange); /** @brief Overridden from SelfSimLATP.*/
    void endRoute();
    virtual void makeRoot(); /** @brief Overridden from SelfSimLATP.*/
    MovementHistory* buildFirstTrace();
    long calculateStep();

public:
    RegularSelfSimLATP();
    void nodeTurnedOff();
};

#endif
