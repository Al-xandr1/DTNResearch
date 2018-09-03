#ifndef LEVYTESTER_H_
#define LEVYTESTER_H_

#include <omnetpp.h>
#include "Coord.h"
#include "Movement.h"

using namespace std;

class LevyTester : public cSimpleModule {
public:
    LevyTester();
    virtual ~LevyTester();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
