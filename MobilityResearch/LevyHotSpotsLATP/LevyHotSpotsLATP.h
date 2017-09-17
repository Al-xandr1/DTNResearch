#ifndef LEVY_HSLATP_H
#define LEVY_HSLATP_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <windows.h>

#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"

#include "DevelopmentHelper.h"
#include "LeviStaticOLD.h"
#include "RootsCollection.h"
#include "HotSpotsCollection.h"

class LevyHotSpotsLATP : public LineSegmentsMobilityBase
{
  protected:
    int NodeID;

    bool isPause;
    long step;

    LeviJumpOLD  *jump;
    LeviPauseOLD *pause;
    double kForSpeed;
    double roForSpeed;

    double angle;
    double distance;
    double speed;
    Coord deltaVector;
    simtime_t travelTime;

    double powA;

    bool movementsFinished;    // показывает окончил ли пользователь движение или нет

    // текущая локация
    Coord currentHSMin, currentHSMax, currentHSCenter;

    HotSpotsCollection* hsc;
    int currentHSindex;

    HSDistanceMatrix* hsd;

    //statistics collection
    char *wpFileName;
    char *trFileName;
    simtime_t waitTime;
    std::vector<simtime_t> inTimes;
    std::vector<simtime_t> outTimes;
    std::vector<double> xCoordinates;
    std::vector<double> yCoordinates;

  protected:
    virtual int  numInitStages() const { return 3; }
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setInitialPosition();

    bool isHotSpotEmpty();
    virtual bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound = false);
    bool findNextHotSpotAndTargetPosition();
    virtual bool findNextHotSpot();       // ищем новую локацию и устанавливаем её новые границы и центр
    virtual void setCurrentHSbordersWith(HotSpotData* hsi);

    virtual void finish()        {saveStatistics();};

    void collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y);
    void saveStatistics();

    bool isCorrectCoordinates(double x, double y);
    void log();

  public:
    LevyHotSpotsLATP();
    int getNodeID()              {return this->NodeID;};

    Coord getConstraintAreaMin() {return this->constraintAreaMin;};
    Coord getConstraintAreaMax() {return this->constraintAreaMax;};
};

#endif
