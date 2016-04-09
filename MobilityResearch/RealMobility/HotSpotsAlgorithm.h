/*
 * HotSpotsAlgorithm.h
 *
 *  Created on: 31 марта 2015 г.
 *      Author: atsarev
 */

#ifndef HOTSPOTSALGORITHM_H_
#define HOTSPOTSALGORITHM_H_

#include "INETDefs.h"
#include "Coord.h"
#include "math.h"
#include <string>
#include <vector>
#include "HotSpot.h"
#include "DevelopmentHelper.h"

class HotSpotsAlgorithm;
typedef HotSpotsAlgorithm HotSpotsAlgorithmDEF;
// из-за цикличеких определений приходиться делать так
#include "SimpleLevyMobility.h"

#define DEF_HS_DIR "./hotspotfiles"    //Директория по умолчанию для "горячих точек"
#define DEF_WP_DIR "./waypointfiles"    //Директория по умолчанию для "путевых точек"

// известные алгоритмы
// простой Леви, без использования кластеров.
#define SIMPLE_LEVY "SimpleLevy"

// Леви с использованием кластеров, которые выбираются случайным образом.
#define LEVY_HOTSPOTS_RANDOM "LevyHotSpotsRandom"

// Леви с использованием кластеров, которые выбираются по алгоритму LATP,
// и расстояние для алгоритма рассчитывается между центрами кластеров.
#define LEVY_HOTSPOTS_LATP_CENTER_LOGIC "LevyHotSpotsLATPCenterLogic"

// Леви с использованием кластеров, которые выбираются по алгоритму LATP,
// и расстояние для алгоритма рассчитывается между текущим положением и центром целевого кластера.
#define LEVY_HOTSPOTS_LATP "LevyHotSpotsLATP"

// Леви с использованием кластеров, которые выбираются по алгоритму LATP,
// и расстояние для алгоритма рассчитывается между текущим положением и центром целевого кластера
// а также учитывается количество возможных посещений для каждого кластера.
#define LEVY_HOTSPOTS_LATP_PATH_COUNTS "LevyHotSpotsLATPPathCounts"

// Леви с использованием кластеров, которые выбираются по алгоритму LATP,
// и расстояние для алгоритма рассчитывается между текущим положением и центром целевого кластера
// а также учитывается количество возможных посещений для каждого кластера.
// В кластере объект двигается по путевым точкам, загруженным заранее.
//#define LEVY_HOTSPOTS_WAYPOINTS_LATP_PATH_COUNTS "LevyHotSpotsWayPointsLATPPathCounts"

class HotSpotsAlgorithm {
private:
    RealMobilityDEF* levyMobility;

    //todo вместо этих переменных сделать грамотную иерархию наследников наследников
    bool useLATP;                  // true - если нужно успользовать LATP алгоритм, false - иначе
    bool useBetweenCentersLogic;   // true - если нужно считать дистанцию между центрами кластеров,
                                   // false - считается дистанция от текущего положения до целевого кластера
    bool useHotSpotAvailabilities; // true - если нужно использовать логику по ограничению
                                   // количества возможных посещений кластера на основе файла spotcount.cnt
                                   // false - если нет, т.е. каждый кластер можно посетить только один раз
    bool useWayPoints;             // true - если нужно использовать путевые точки внутри каждого кластера.
                                   // Перемещение по данным точкам будет осуществляться по алгоритму LATP.
                                   // false - если нет.

    vector<HotSpot*>* allHotSpots; // все кластеры
    vector<int>* availabilityPerHS;// массив счётчиков доступных посещений для кластеров

    double** distMatrix;           // матрица дистанций
    int currentIndexHS;            // индекс текущего кластера в структуре allHotSpots
    double powA;                   // показатель степени в диапазоне от 0 до бесконечности

public:
    HotSpotsAlgorithm(RealMobilityDEF* levyMobility, double powA,
            bool useLATP, bool useBetweenCentersLogic, bool useHotSpotAvailabilities,
            bool useWayPoints);
    virtual ~HotSpotsAlgorithm();
    Coord getInitialPosition();
    bool fixTargetPosition(Coord& targetPosition, Coord delta, double distance);

private:
    void initialize();
    void initializeHotSpotAvailabilities();
//    void initializeWayPoints();
    void initializeDistanceMatrix();
    void checkHotSpotsBound();
//    void checkWayPointsBound();

    bool setNextCurrentHotSpotIndex();
    Coord getRandomPositionInsideHS(uint hotSpotIndex);
    double getDistanceFromCurrentHS(uint targetHotSpotIndex);
    bool isAvailable(uint hotSpotIndex);
    void setVisited(uint hotSpotIndex);
};

#endif /* HOTSPOTSALGORITHM_H_ */
