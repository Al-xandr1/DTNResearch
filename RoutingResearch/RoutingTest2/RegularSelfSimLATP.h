#ifndef REGULARSELFSIMLATP_H_
#define REGULARSELFSIMLATP_H_

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <windows.h>
#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"
#include "cmessage.h"
#include "DevelopmentHelper.h"
#include "LeviStatic.h"
#include "HotSpotsCollection.h"
#include "RootsCollection.h"
#include "SelfSimLATP.h"
#include "MobileHost.h"
#include "Messages.h"

using namespace std;

class RegularSelfSimLATP : public SelfSimLATP {

protected:
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/
    virtual void handleMessage(cMessage *message);
    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    void endRoute();
    virtual void makeRoot();

public:
    void nodeTurnedOff();
};

#endif
