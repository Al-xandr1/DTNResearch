#ifndef LEVY_MOBILITY_H
#define LEVY_MOBILITY_H

#include <fstream>
#include <string>
#include <math.h>

#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"
#include "Movement.h"
#include "MovementHistory.h"
#include "DevelopmentHelper.h"

#include "LeviStatic.h"

/**
 * Simple Levy mobility on the single area
 */
class SimpleLevyMobility : public LineSegmentsMobilityBase {
protected:
    int NodeID;

    bool isPause;
    long step;
    bool movementsFinished;    // показывает окончил ли пользователь движение или нет

    Movement *movement;

    // текущая локация
    Coord currentHSMin, currentHSMax, currentHSCenter;
    int currentHSindex;

    //statistics collection
    MovementHistory* mvnHistory;

protected:
    virtual int numInitStages() const { return 3; }
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setInitialPosition();

    bool isHotSpotEmpty();
    bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange);
    virtual bool findNextHotSpot() { return true; }  // ищем новую локацию и устанавливаем её новые границы и центр

    virtual void finish();

    void collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y);
    void saveStatistics();

    bool isCorrectCoordinates(double x, double y);
    void log();

public:
    SimpleLevyMobility();

    int getNodeID()              {return this->NodeID;}
    Coord getLastPosition()      {return this->lastPosition;};
    Coord getConstraintAreaMin() {return this->constraintAreaMin;};
    Coord getConstraintAreaMax() {return this->constraintAreaMax;};
};

#endif
