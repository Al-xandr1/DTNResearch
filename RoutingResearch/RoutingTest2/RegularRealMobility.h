#ifndef __ROUTINGTEST2_REGULARREALMOBILITY_H_
#define __ROUTINGTEST2_REGULARREALMOBILITY_H_

#include <omnetpp.h>
#include "RealMobility.h"
#include <RoutingDaemon.h>

/**
 * Класс, описывающий ежедневную реальную мобильность
 */
class RegularRealMobility : public RealMobility {
protected:
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/
    virtual void handleMessage(cMessage *message);
    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/

 public:
    virtual void makeNewRoot();
};

#endif
