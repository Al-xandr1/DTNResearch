#ifndef REGROOT_LATP_H
#define REGROOT_LATP_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>

#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"
#include "cmessage.h"

#include "DevelopmentHelper.h"
#include "LeviStatic.h"
#include "HotSpotsCollection.h"
#include "RootsCollection.h"
#include "LevyHotSpotsLATP.h"

using namespace std;

class RegularRootLATP : public LevyHotSpotsLATP
{
  protected:

    RootsCollection* rc;

    vector<HotSpotShortInfo*>* firstRoot;
    vector<unsigned int>* firstRootSnumber;
    vector<int>* firstRootCounter;

    vector<HotSpotShortInfo*>* currentRoot;
    vector<unsigned int>* currentRootSnumber;
    vector<int>* currentRootCounter;

    unsigned int curRootIndex;

    bool isLProbReady;
    double**  LocalProbMatrix;

    double dayDuration;

    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setInitialPosition();

    virtual bool findNextHotSpot();       // ищем новую локацию и устанавливаем её новые границы и центр

    virtual void move();                  /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void finish();

    virtual void handleMessage(cMessage* msg);

    void makeNewRoot();

  public:
    RegularRootLATP();
    void loadFirstRoot();
    void PrintFirstRoot();
    void PrintCurrentRoot();
    void makeLocalProbMatrix(double powA);

};

#endif
