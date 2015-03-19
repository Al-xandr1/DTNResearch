#ifndef LEVY_MOBILITY_H
#define LEVY_MOBILITY_H

#include "INETDefs.h"

#include "LineSegmentsMobilityBase.h"

#include "LeviStatic.h"
#include <fstream>
#include <string>
#include "DevelopmentHelper.h"
#include "HotSpot.h"

class LevyMobility : public LineSegmentsMobilityBase
{
  protected:
    bool nextMoveIsWait;

    LeviJump  *jump;
    LeviPause *pause;
    double kForSpeed;
    double roForSpeed;

    //for hot spots
    bool useHotSpots;                   // включает/выключает использование горячих точек
    bool useLATPalgorithm;              // true - если нужно успользовать LATP алгоритм, false - иначе
    vector<HotSpot*>* allHotSpots;      // все кластеры
    vector<HotSpot*>* visitedHotSpots;   // посещённые кластеры
    double** distMatrix;                // матрица дистанций
    HotSpot* currentHotSpot;            // текущий кластер
    uint currentIndexHS;                // индекс текущего кластера в структуре allHotSpots
    double powA;                        // показатель степени в диапазоне от 0 до бесконечности


    //statistics collection
    std::vector<simtime_t> times;
    std::vector<double> xCoordinates;
    std::vector<double> yCoordinates;

  protected:
    virtual int numInitStages() const { return 3; }

    /** @brief Initializes mobility model parameters.*/
    virtual void initialize(int stage);

    void initializeHotSpots();

    /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setTargetPosition();

    virtual void setInitialPosition();

    void generateNextPosition(Coord& targetPosition, simtime_t& nextChange);

    HotSpot* getRandomHotSpot(HotSpot* excludedHotSpot);

    Coord getRandomPositionInsideHS(HotSpot* hotSpot);

    void checkHotSpotsBound();

    /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void move();

    virtual void finish();

    void collectStatistics(simtime_t appearenceTime, double x, double y);

    void saveStatistics();

  public:
    LevyMobility();
};

#endif
