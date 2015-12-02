#ifndef LEVY_HSLATP_H
#define LEVY_HSLATP_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"

#include "DevelopmentHelper.h"
#include "LeviStatic.h"
#include "HotSpotsCollection.h"

class LevyHotSpotsLATP : public LineSegmentsMobilityBase
{
  protected:
    bool nextMoveIsWait;

    LeviJump  *jump;
    LeviPause *pause;
    double kForSpeed;
    double roForSpeed;

    double powA;

    bool movementsFinished;    // показывает окончил ли пользователь движение или нет

    // текущая локация
    Coord currentHSMin, currentHSMax, currentHSCenter;

    HotSpotsCollection* hsc;
    unsigned int currentHSindex;

    HSDistanceMatrix* hsd;

    //statistics collection
    simtime_t waitTime;
    std::vector<simtime_t> inTimes;
    std::vector<simtime_t> outTimes;
    std::vector<double> xCoordinates;
    std::vector<double> yCoordinates;

  protected:
    virtual int numInitStages() const { return 3; }
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setInitialPosition();

    void generateNextPosition(Coord& targetPosition, simtime_t& nextChange);
    virtual bool findNextHotSpot();       // ищем новую локацию и устанавливаем её новые границы и центр

    virtual void move();                  /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void finish();

    void collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y);
    bool isCorrectCoordinates(double x, double y);
    void saveStatistics();

  public:
    LevyHotSpotsLATP();

    Coord getLastPosition()      {return this->lastPosition;};
    Coord getConstraintAreaMin() {return this->constraintAreaMin;};
    Coord getConstraintAreaMax() {return this->constraintAreaMax;};
};

#endif
