#ifndef LEVYTESTER_H_
#define LEVYTESTER_H_

#include <omnetpp.h>
#include "Coord.h"
#include "Movement.h"

using namespace std;

class LevyTester : public cSimpleModule {
private:
    unsigned int selectionVolume;
    cDoubleHistogram* flightLengthPDF;
    cDoubleHistogram* velocityPDF;
    cDoubleHistogram* pausePDF;

public:
    LevyTester();
    virtual ~LevyTester();

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();

private:
    void writeAreaStatistics(ofstream* out);
    void writeHistogramStatistics(ofstream* out, char* tag, cDoubleHistogram* hist);
    vector<double>* toPDFVector(cDoubleHistogram* hist);
    vector<double>* toCDFVector(cDoubleHistogram* hist);
    vector<double>* toCCDFVector(cDoubleHistogram* hist);
};

#endif
