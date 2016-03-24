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

    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    virtual bool findNextHotSpot();       // ���� ����� ������� � ������������� � ����� ������� � �����

  public:
    RegularRootLATP();
    void loadFirstRoot();
    void printFirstRoot();
    void printCurrentRoot();
    void makeLocalProbMatrix(double powA);

    void makeNewRoot();
};

#endif
