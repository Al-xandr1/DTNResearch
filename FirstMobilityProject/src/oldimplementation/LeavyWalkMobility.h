/*
 * LeavyWalkMobility.h
 *
 *  Created on: 02 окт. 2014 г.
 *      Author: Alexander
 */

#ifndef LEAVYWALKMOBILITY_H_
#define LEAVYWALKMOBILITY_H_

#include "LineSegmentsMobilityBase.h"
#include "FWMath.h"

#include <omnetpp.h>
#include <fstream>
#include <string>
#include <list>
#include "DevelopmentHelper.cpp"

class LeavyWalkMobility : public LineSegmentsMobilityBase {

private:
    //module parameters
    double c_lenght;
    double c_pause;
    double alpha;   // [0.5;1.5]
    double betta;

    double tauL;    //for truncation
    double tauP;    //for truncation

    double k;       //for speed
    double ro;      //for speed

    //model variables
    int stageOfFlight; // 0 - setting of flight's lenght;
                        // 1 - setting of flight pause in the end of flight

    double direction, lenght;
    simtime_t flightTime, pauseTime;

    //statistics collection
    char *statisticFileName;
    ofstream *out;

private:
    double generateDirection();
    simtime_t generateFlightTime(double flyLenght);
    double generateLenght();
    simtime_t generatePauseTime();
    double generateLeavyRandomValue(double param1, double param2);

    bool isCorrectLeavyValue(simtime_t value, simtime_t trucateFactor);
    bool isCorrectLeavyValue(double value, double trucateFactor);

    void generateNextStep();
    void setFlight();
    void setPause();

    void initializeStatisticsFile();
    void collectStatistics(double x, double y, simtime_t appearenceTime,
            simtime_t waitTime, simtime_t flightTime);
    void closeStatisticsFile();

    virtual void finish();

protected:
    virtual void initialize(int stage);
    virtual void setTargetPosition();

public:
    LeavyWalkMobility();
    virtual ~LeavyWalkMobility();
};

#endif /* LEAVYWALKMOBILITY_H_ */
