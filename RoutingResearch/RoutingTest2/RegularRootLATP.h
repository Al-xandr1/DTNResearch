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

    vector<HotSpotData*>*      firstRoot;               // сформированный вектор (эталона) маршрута с информацией, загруженной из файлов *.hts
    vector<unsigned int>*      firstRootSnumber;        // сформированный вектор (эталона) с индексами локаций в структуре HotSpotsCollection
    vector<int>*               firstRootCounter;        // сформированный вектор (эталона) со счЄтчиками посещений локаций
    vector<int>*               firstRootWptsPerVisit;   // сформированный вектор (эталона) среднего количества путевых точек на локацию маршрута

    HotSpotData*               homeHS;                  // перва€ локаци€ маршрута, она же последн€€

    vector<HotSpotData*>*      currentRoot;             // сформированный вектор (текущий) маршрута с информацией, загруженной из файлов *.hts
    vector<unsigned int>*      currentRootSnumber;      // сформированный вектор (текущий) с индексами локаций в структуре HotSpotsCollection
    vector<int>*               currentRootCounter;      // сформированный вектор (текущий) со счЄтчиками посещений локаций
    vector<int>*               currentRootWptsPerVisit; // сформированный вектор (текущий) среднего количества путевых точек на локацию маршрута

    unsigned int curRootIndex;                          // индекс текущей локации в текущем маршруте
    unsigned int currentHSWaypointNum;                  // количество оставшихс€ путевых точек в текущей локации
    bool useWaypointCounter;                            // флаг, показывающий нужно использовать счЄтчик путевых точек на локацию или нет

    bool      isLProbReady;
    double**  LocalProbMatrix;

    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/
    virtual void handleMessage(cMessage * message);
    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual bool findNextHotSpot();
    virtual void setCurrentHSbordersWith(HotSpotData* hsi);
    virtual bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound = false);

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
