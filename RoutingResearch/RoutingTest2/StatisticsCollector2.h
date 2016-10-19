#ifndef STATISTICSCOLLECTOR2_H_
#define STATISTICSCOLLECTOR2_H_

#include <iostream>
#include <fstream>
#include <omnetpp.h>
#include <Messages.h>
#include "DevelopmentHelper.h"

using namespace std;

//todo ���� ����� ������ ����� �������� �����������. ����� ������������ ������� ����� ����������

class StatisticsCollector2 : public cSimpleModule {
private:
    ifstream* packetsHistoryFile;   // ���� � ����������� � ���� �������

    unsigned int createdPackes;
    unsigned int receivedPackets;

    cDoubleHistogram* lifeTimePDF;
    cDoubleHistogram* ictPDF;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
};

#endif /* STATISTICSCOLLECTOR2_H_ */
