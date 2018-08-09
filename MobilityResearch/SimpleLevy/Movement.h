#ifndef MOVEMENTS_H_
#define MOVEMENTS_H_

#include <iostream>
#include "Coord.h"
#include "LeviStatic.h"
#include "DevelopmentHelper.h"

class Movement {
private:
    double kForSpeed_1;
    double roForSpeed_1;
    double distanceThreshold;
    double kForSpeed_2;
    double roForSpeed_2;

    double maxPermittedDistance;

    LeviJump* leviJump;
    LeviPause* levyPause;

    simtime_t waitTime;
    double distance;
    double angle;
    double speed;
    simtime_t travelTime;

public:
    Movement(const double kForSpeed_1,
             const double roForSpeed_1,
             const double distanceThreshold,
             const double kForSpeed_2,
             const double roForSpeed_2,
             const double maxPermittedDistance,
             LeviJump* leviJump,
             LeviPause* levyPause) {
        this->kForSpeed_1 =  kForSpeed_1;
        this->roForSpeed_1 = roForSpeed_1;
        this->distanceThreshold = distanceThreshold;
        this->kForSpeed_2 =  kForSpeed_2;
        this->roForSpeed_2 = roForSpeed_2;

        this->maxPermittedDistance = maxPermittedDistance;

        this->leviJump = leviJump;
        this->levyPause = levyPause;

        this->waitTime = 0;
        this->distance = -1;
        this->angle = -1;
        this->speed = -1;
        this->travelTime = 0;

        log();
    }
    virtual ~Movement() {
        myDelete(this->leviJump);
        myDelete(this->levyPause);
    };

    const double getKForSpeed_1()      {return kForSpeed_1;};
    const double getRoForSpeed_1()     {return roForSpeed_1;};
    const double getDistanceThreshold(){return distanceThreshold;};
    const double getKForSpeed_2()      {return kForSpeed_2;};
    const double getRoForSpeed_2()     {return roForSpeed_2;};

    bool genPause(const char* where);
    bool genFlight(const char* where);

    void setWaitTime(const simtime_t waitTime);
    const simtime_t getWaitTime()   {return waitTime;};

    void setDistance(const double dist, const char* where);
    const double getDistance()      {return distance;};

    const double getAngle()         {return angle;};
    const double getSpeed()         {return speed;};
    const simtime_t getTravelTime() {return travelTime;};

    const Coord getDeltaVector();
    void log();

private:
    void computeSpeed();
};

#endif /* MOVEMENTS_H_ */
