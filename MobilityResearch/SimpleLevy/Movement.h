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
    Movement(cComponent* com, double maxPermittedDistance) {
        if (com->hasPar("ciJ") && com->hasPar("aliJ") && com->hasPar("deltaXJ") && com->hasPar("joinJ")) {
            this->leviJump = new LeviJump(
                            com->par("ciJ").doubleValue(),
                            com->par("aliJ").doubleValue(),
                            com->par("deltaXJ").doubleValue(),
                            com->par("joinJ").doubleValue());
            cout << "Movement:leviJump is initialized!" << endl;
        } else {
            cout << "Movement:leviJump is NOT initialized! Not all parameters are specified" << endl;
        }

        if (com->hasPar("ciP") && com->hasPar("aliP") && com->hasPar("deltaXP") && com->hasPar("joinP")) {
            this->levyPause = new LeviPause(
                            com->par("ciP").doubleValue(),
                            com->par("aliP").doubleValue(),
                            com->par("deltaXP").doubleValue(),
                            com->par("joinP").doubleValue());
            cout << "Movement:levyPause is initialized!" << endl;
        } else {
            cout << "Movement:levyPause is NOT initialized! Not all parameters are specified" << endl;
        }

        this->maxPermittedDistance = maxPermittedDistance;

        if (  !(com->hasPar("kForSpeed_1") && com->hasPar("roForSpeed_1") && com->hasPar("distanceThreshold")
             && com->hasPar("kForSpeed_2") && com->hasPar("roForSpeed_2"))) {
            cout << "It is necessary to specify ALL parameters for speed function" << endl; exit(-2121);
        }
        this->kForSpeed_1 = com->par("kForSpeed_1").doubleValue();
        this->roForSpeed_1 = com->par("roForSpeed_1").doubleValue();
        this->distanceThreshold = com->par("distanceThreshold").doubleValue();
        this->kForSpeed_2 = com->par("kForSpeed_2").doubleValue();
        this->roForSpeed_2 = com->par("roForSpeed_2").doubleValue();

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

    bool genPause(string where);
    bool genFlight(string where);

    void setWaitTime(const simtime_t waitTime);
    const simtime_t getWaitTime()   {return waitTime;};

    void setDistance(const double dist, string where);
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
