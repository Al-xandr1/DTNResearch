#ifndef MOVEMENTS_H_
#define MOVEMENTS_H_

#include <iostream>
#include "Coord.h"
#include "LeviStatic.h"
#include "DevelopmentHelper.h"

class Movement {
private:
    double kForSpeed;
    double roForSpeed;
    double maxPermittedDistance;

    LeviJump* leviJump;
    LeviPause* levyPause;

    simtime_t waitTime;
    double distance;
    double angle;
    double speed;
    simtime_t travelTime;

public:
    Movement(const double kForSpeed, const double roForSpeed, const double maxPermittedDistance, LeviJump* leviJump, LeviPause* levyPause) {
        this->kForSpeed =  kForSpeed;
        this->roForSpeed = roForSpeed;
        this->maxPermittedDistance = maxPermittedDistance;

        this->leviJump = leviJump;
        this->levyPause = levyPause;

        this->waitTime = 0;
        this->distance = -1;
        this->angle = -1;
        this->speed = -1;
        this->travelTime = 0;
    }
    virtual ~Movement() {
        myDelete(this->leviJump);
        myDelete(this->levyPause);
    };

    const double getKForSpeed()     {return kForSpeed;};
    const double getRoForSpeed()    {return roForSpeed;};

    bool genPause(const char* where);
    bool genFlight(const char* where);

    void setWaitTime(const simtime_t waitTime);
    const simtime_t getWaitTime()   {return waitTime;};

    void setDistance(const double dist);
    const double getDistance()      {return distance;};

    const double getAngle()         {return angle;};
    const double getSpeed()         {return speed;};
    const simtime_t getTravelTime() {return travelTime;};

    const Coord getDeltaVector();
    void log();
};

#endif /* MOVEMENTS_H_ */
