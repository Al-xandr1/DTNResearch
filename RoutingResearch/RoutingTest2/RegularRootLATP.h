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
#include "LeviStaticOLD.h"
#include "HotSpotsCollection.h"
#include "RootsCollection.h"
#include "LevyHotSpotsLATP.h"
#include "MobileHost.h"
#include "Messages.h"
#include "RootsPersistenceAndStatistics.h"
#include "GenerationRootsStrategy.h"

using namespace std;

class RegularRootLATP : public LevyHotSpotsLATP
{
  protected:

    RootsCollection* rc;
    RootsPersistenceAndStatistics* rootStatistics;      // ссылка на модуль с общей персистентностью и статистикой
    double rootPersistence;                             // коэффициент персистентности, для мобильности КОНКРЕТНОГО узла
    GenerationRootsStrategy* rootGenerator;             // генератор новых маршрутов (сейчас ЛИБО по персистентности, ЛИБО по статистикам

    vector<HotSpotData*>*      firstRoot;               // сформированный вектор (эталона) маршрута с информацией, загруженной из файлов *.hts
    vector<unsigned int>*      firstRootSnumber;        // сформированный вектор (эталона) с индексами локаций в структуре HotSpotsCollection
    vector<int>*               firstRootCounter;        // сформированный вектор (эталона) со счётчиками посещений локаций
    vector<int>*               firstRootWptsPerVisit;   // сформированный вектор (эталона) среднего количества путевых точек на локацию маршрута

    HotSpotData*               homeHS;                  // первая локация маршрута, она же последняя

    vector<unsigned int>*      currentRootActualTrack;  // фактическая последовательность локаций при прохождении маршрута
    vector<HotSpotData*>*      currentRoot;             // сформированный вектор (текущий) маршрута с информацией, загруженной из файлов *.hts
    vector<unsigned int>*      currentRootSnumber;      // сформированный вектор (текущий) с индексами локаций в структуре HotSpotsCollection
    vector<int>*               currentRootCounter;      // сформированный вектор (текущий) со счётчиками посещений локаций
    vector<int>*               currentRootWptsPerVisit; // сформированный вектор (текущий) среднего количества путевых точек на локацию маршрута
    vector<int>*               currentRootCounterSAVED; // после каждого создания маршрута сюда сохраняется currentRootCounter для дальнейшего вычисления фактически пройденного

    unsigned int curRootIndex;                          // индекс текущей локации в текущем маршруте
    unsigned int currentHSWaypointNum;                  // количество оставшихся путевых точек в текущей локации
    bool useWaypointCounter;                            // флаг, показывающий нужно использовать счётчик путевых точек на локацию или нет

    bool      isLProbReady;
    double**  LocalProbMatrix;

    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/
    virtual void handleMessage(cMessage * message);
    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual bool findNextHotSpot();
    void setCurRootIndex(unsigned int curRootIndex, bool writeIndexToTrack);
    virtual bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound = false);

  public:
    RegularRootLATP();
    void loadFirstRoot();
    void printFirstRoot();
    void printCurrentRoot();
    void printRoot(
            const char* lable,
            vector<HotSpotData*>* root,
            vector<unsigned int>* rootSnumber,
            vector<int>* rootCounter,
            vector<int>* rootWptsPerVisit);
    void makeLocalProbMatrix(double powA);
    void deleteLocalProbMatrix();
    bool isRootFinished();
    void makeNewRoot();
};

#endif
