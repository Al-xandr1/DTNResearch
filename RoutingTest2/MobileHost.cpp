#include <iostream>
#include <stdlib.h>
#include <string>
#include <math.h>

#include "INETDefs.h"

#include <IMobility.h>
#include <clistener.h>

#include <LevyHotSpotsLATP.h>


class MobileHost : public cSimpleModule
{
private:
    int packet;
    int indexOfNode;


public:
    MobileHost(){};

    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};


MobileHost::initialize()
{
    packet = 5;
    indexOfNode = par("indexOfNode");
}
