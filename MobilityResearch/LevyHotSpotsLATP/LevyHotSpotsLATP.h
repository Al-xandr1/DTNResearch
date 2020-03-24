#ifndef LEVY_HSLATP2_H
#define LEVY_HSLATP2_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include <windows.h>

#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"

#include "DevelopmentHelper.h"
#include "RootsCollection.h"
#include "Movement.h"
#include "MovementHistory.h"
#include "HotSpotsCollection2.h"

class LevyHotSpotsLATP2 : public LineSegmentsMobilityBase {
  protected:
    int NodeID;

    bool isPause;
    long step;
    bool movementsFinished;    // показывает окончил ли пользователь движение или нет
    long countOfFirstSkippedLongFlight;
    double powA;

    Movement *movement;
    simtime_t waitTime;

    // текущая локация
    double currentHSAngle;        // !!!
    Coord currentHSMin, currentHSMax, currentHSCenter;

    HotSpotsCollection2* hsc;     // !!!
    int currentHSindex;

    HSDistanceMatrix* hsd;

    //statistics collection
    MovementHistory* mvnHistory;

  protected:
    virtual int  numInitStages() const { return 3; }
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setInitialPosition();
    void setWaitTime(simtime_t time) {
        ASSERT(time > 0);
        this->waitTime = time;
    }

    simtime_t getWaitTime() {
        ASSERT(waitTime > 0);
        return waitTime;
    };

    bool getIsPause()   { return this->isPause; };
    long getStep()      { return this->step; };
    void decreaseStep() { this->step--; };
    MovementHistory* getMovementHistory() { return this->mvnHistory; };

    bool isHotSpotEmpty();

    virtual bool generatePause(simtime_t &nextChange);
    virtual bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound = false);

    bool findNextHotSpotAndTargetPosition();
    virtual bool findNextHotSpot();       // ищем новую локацию и устанавливаем её новые границы и центр
    virtual void setCurrentHSbordersWith(HotSpotData* hsi);

    virtual void finish()        {saveStatistics();};

    virtual void collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y);
    void saveStatistics();

    bool isCorrectCoordinates(double x, double y);
    void log(string log);
    void log();

  public:
    LevyHotSpotsLATP2();
    int getNodeID()              {return this->NodeID;};

    Coord getConstraintAreaMin() {return this->constraintAreaMin;};
    Coord getConstraintAreaMax() {return this->constraintAreaMax;};
};

#endif
