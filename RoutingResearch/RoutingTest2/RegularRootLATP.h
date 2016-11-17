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

    vector<HotSpotShortInfo*>* firstRoot;
    vector<unsigned int>*      firstRootSnumber;
    vector<int>*               firstRootCounter;

    HotSpotShortInfo*          homeHS;             // перва€ локаци€ маршрута, она же последн€€

    vector<HotSpotShortInfo*>* currentRoot;
    vector<unsigned int>*      currentRootSnumber;
    vector<int>*               currentRootCounter;

    unsigned int curRootIndex;

    bool      isLProbReady;
    double**  LocalProbMatrix;

    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    virtual bool findNextHotSpot();       // ищем новую локацию и устанавливаем еЄ новые границы и центр

  public:
    RegularRootLATP();
    void loadFirstRoot();
    void printFirstRoot();
    void printCurrentRoot();
    void makeLocalProbMatrix(double powA);
    void deleteLocalProbMatrix();
    virtual bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange);

    bool isRootFinished();

    void makeNewRoot();
};

#endif
