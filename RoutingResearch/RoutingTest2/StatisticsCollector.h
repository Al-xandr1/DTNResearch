
#ifndef __ROUTINGTEST2_STATISTICSCOLLECTOR_H_
#define __ROUTINGTEST2_STATISTICSCOLLECTOR_H_

#include <iostream>
#include <fstream>

#include <omnetpp.h>

#include <Messages.h>
#include "DevelopmentHelper.h"

using namespace std;

//todo этот класс должен стать отдельно запускаемым. Будет обрабатывать входные файлы статистики

class StatisticsCollector : public cSimpleModule
{
private:
    cGate* rdGate;

    unsigned int createdPackes;
    unsigned int receivedPackets;

    cDoubleHistogram* lifeTimePDF;
    cDoubleHistogram* ictPDF;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif
