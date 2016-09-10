#ifndef LEVY_MOBILITY_H
#define LEVY_MOBILITY_H

#include <fstream>
#include <string>
#include <math.h>

#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"

#include "DevelopmentHelper.h"
#include "LeviStatic.h"

// определяет конкретную спецификацию мобильности
enum LeviMobilitySubtype { SimpleLevy = 0,              // простой Леви, без использования кластеров.
                       LevyHotSpotsRandom,              // Леви с использованием кластеров, которые выбираются случайным образом.
                       LevyHotSpotsLATP,                // Леви с использованием кластеров, которые выбираются по алгоритму LATP,
                                                        // и расстояние для алгоритма рассчитывается между текущим положением и центром целевого кластера.
                       LevyHotSpotsLATPCenterLogic,     // Леви с использованием кластеров, которые выбираются по алгоритму LATP,
                                                        // и расстояние для алгоритма рассчитывается между центрами кластеров.
                       LevyHotSpotsLATPPathCounts,      // Леви с использованием кластеров, которые выбираются по алгоритму LATP,
                                                        // и расстояние для алгоритма рассчитывается между текущим положением и центром целевого кластера
                                                        // а также учитывается количество возможных посещений для каждого кластера.
		       LevyHotSpotsWayPointsLATPPathCounts  // Леви с использованием кластеров, которые выбираются по алгоритму LATP,
                                                        // и расстояние для алгоритма рассчитывается между текущим положением и центром целевого кластера
                                                        // а также учитывается количество возможных посещений для каждого кластера.
                                                        // В кластере объект двигается по путевым точкам, загруженным заранее.
                     };

class SimpleLevyMobility : public LineSegmentsMobilityBase
{
  protected:
    int NodeID;

    bool isPause;
    long step;

    LeviJump  *jump;
    LeviPause *pause;
    double kForSpeed;
    double roForSpeed;

    double angle;
    double distance;
    double speed;
    Coord deltaVector;
    simtime_t travelTime;

    double powA;

    bool movementsFinished;    // показывает окончил ли пользователь движение или нет

    // текущая локация
    Coord currentHSMin, currentHSMax, currentHSCenter;
    int currentHSindex;

    //statistics collection
    char *wpFileName;
    char *trFileName;
    simtime_t waitTime;
    std::vector<simtime_t> inTimes;
    std::vector<simtime_t> outTimes;
    std::vector<double> xCoordinates;
    std::vector<double> yCoordinates;

  protected:
    virtual int numInitStages() const { return 3; }
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setInitialPosition();

    bool isHotSpotEmpty();
    bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange);
    virtual bool findNextHotSpot() { return true; }  // ищем новую локацию и устанавливаем её новые границы и центр

    virtual void finish();

    void collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y);
    void saveStatistics();

    bool isCorrectCoordinates(double x, double y);
    void log();

  public:
    SimpleLevyMobility();
    int getNodeID();

    Coord getLastPosition()      {return this->lastPosition;};
    Coord getConstraintAreaMin() {return this->constraintAreaMin;};
    Coord getConstraintAreaMax() {return this->constraintAreaMax;};
};

#endif
