#ifndef REAL_MOBILITY_H
#define REAL_MOBILITY_H

#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"
#include "LeviStatic.h"
#include <fstream>
#include <string>
#include "math.h"
#include "Messages.h"
#include "DevelopmentHelper.h"
#include "MovementHistory.h"
#include "TracesCollection.h"


class RealMobility : public LineSegmentsMobilityBase
{
  protected:
    int NodeID;

    long step;

    simtime_t timeOffset;
    double distance;
    double speed;
    simtime_t travelTime;

    bool movementsFinished;    // показывает окончил ли пользователь движение или нет

    TracesCollection* traces;
    vector<TracePoint>* currentTrace;

    //statistics collection
    MovementHistory* mvnHistory;

  protected:
    virtual int numInitStages() const { return 3; }
    virtual void initialize(int stage);
    virtual void setTargetPosition();
    virtual void setInitialPosition();
    bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange);
    virtual void finish()        {saveStatistics();};
    void collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y);
    void saveStatistics();
    void log(string log);
    void log();

  public:
    RealMobility();

    virtual void makeNewRoot();

    int   getNodeID()            {return this->NodeID;};
    Coord getLastPosition()      {return this->lastPosition;};
    Coord getConstraintAreaMin() {return this->constraintAreaMin;};
    Coord getConstraintAreaMax() {return this->constraintAreaMax;};
};

#endif
