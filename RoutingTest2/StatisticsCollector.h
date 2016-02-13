
#ifndef __ROUTINGTEST2_STATISTICSCOLLECTOR_H_
#define __ROUTINGTEST2_STATISTICSCOLLECTOR_H_

#include <omnetpp.h>

#include <Messages.h>


class StatisticsCollector : public cSimpleModule
{
private:
    cGate* rdGate;

    unsigned int createdPackes;
    unsigned int receivedPackets;
    simtime_t timeOfCollection;

    cDoubleHistogram* lifeTimePDF;
    cDoubleHistogram* ictPDF;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
