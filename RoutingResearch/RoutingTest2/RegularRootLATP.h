#ifndef REGROOT_LATP_H
#define REGROOT_LATP_H

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
#include "LevyHotSpotsLATP.h"
#include "MobileHost.h"

#include "Messages.h"

using namespace std;

class RegularRootLATP : public LevyHotSpotsLATP
{
  protected:

    RootsCollection* rc;

    double rootPersistence;

    vector<HotSpotData*>* firstRoot;
    vector<unsigned int>*      firstRootSnumber;
    vector<int>*               firstRootCounter;

    HotSpotData*          homeHS;             // перва€ локаци€ маршрута, она же последн€€

    vector<HotSpotData*>* currentRoot;
    vector<unsigned int>*      currentRootSnumber;
    vector<int>*               currentRootCounter;

    unsigned int curRootIndex;

    bool      isLProbReady;
    double**  LocalProbMatrix;

    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/
    virtual void handleMessage(cMessage * message);
    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual bool findNextHotSpot();
    virtual bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange);

  public:
    RegularRootLATP();
    void loadFirstRoot();
    void printFirstRoot();
    void printCurrentRoot();
    void makeLocalProbMatrix(double powA);
    void deleteLocalProbMatrix();
    bool isRootFinished();
    void makeNewRoot();
};

#endif
