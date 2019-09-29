#ifndef __ROUTINGTEST2_REGULARSIMPLELEVYMOBILITY_H_
#define __ROUTINGTEST2_REGULARSIMPLELEVYMOBILITY_H_

#include <omnetpp.h>
#include "SimpleLevyMobility.h"
#include <RoutingDaemon.h>

/**
 * Regular (daily) Levy mobility on the single area
 */
class RegularSimpleLevyMobility : public SimpleLevyMobility {
protected:
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/
    virtual void handleMessage(cMessage *message);
    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    void endRoute();
    virtual void makeNewRoot();

public:
    void nodeTurnedOff();
};

#endif
