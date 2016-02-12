
#ifndef __ROUTINGTEST2_STATISTICSCOLLECTOR_H_
#define __ROUTINGTEST2_STATISTICSCOLLECTOR_H_

#include <omnetpp.h>

#include <Messages.h>


class StatisticsCollector : public cSimpleModule
{
private:
    cGate* rdGate;

    unsigned int createdPackes;
    unsigned int recievedPackets;
    simtime_t timeOfCollection;


protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
