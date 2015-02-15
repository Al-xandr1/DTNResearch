#ifndef LEVY_MOBILITY_H
#define LEVY_MOBILITY_H

#include "INETDefs.h"

#include "LineSegmentsMobilityBase.h"

#include "LeviStatic.h"
#include <fstream>
#include <string>
#include "DevelopmentHelper.cpp"

class LevyMobility : public LineSegmentsMobilityBase
{
  protected:
    bool nextMoveIsWait;

    LeviJump  *jump;
    LeviPause *pause;
    double kForSpeed;
    double roForSpeed;

    //statistics collection
    std::vector<simtime_t> times;
    std::vector<double> xCoordinates;
    std::vector<double> yCoordinates;

  protected:
    virtual int numInitStages() const { return 3; }

    /** @brief Initializes mobility model parameters.*/
    virtual void initialize(int stage);

    /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setTargetPosition();

    /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void move();

    virtual void finish();

    void collectStatistics(simtime_t appearenceTime, double x, double y);

    void saveStatistics();

  public:
    LevyMobility();
};

#endif
