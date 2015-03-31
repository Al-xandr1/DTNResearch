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
#include "LevyMobility.h"

#define DEF_HS_DIR "./hotspotfiles"    //Директория по умолчанию для "горячих точек"

// известные алгоритмы
#define SIMPLE_LEVY "SimpleLevy"
#define LEVY_HOTSPOTS_RANDOM "LevyHotSpotsRandom"
#define LEVY_HOTSPOTS_LATP_CENTER_LOGIC "LevyHotSpotsLATPCenterLogic"
#define LEVY_HOTSPOTS_LATP "LevyHotSpotsLATP"
#define LEVY_HOTSPOTS_LATP_PATH_COUNTS "LevyHotSpotsLATPPathCounts"


class HotSpotsAlgorithm {
private:
    LevyMobilityDEF* levyMobility;

    //todo вместо этих переменных сделать грамотную иерархию наследников наследников
    bool useLATP;                       // true - если нужно успользовать LATP алгоритм, false - иначе
    bool useBetweenCentersLogic;        // true - если нужно считать дистанцию между центрами кластеров,
                                        // false - считается дистанция от текущего положения до целевого кластера
    vector<HotSpot*>* allHotSpots;      // все кластеры
    vector<HotSpot*>* visitedHotSpots;  // посещённые кластеры
    double** distMatrix;                // матрица дистанций
    HotSpot* currentHotSpot;            // текущий кластер
    uint currentIndexHS;                // индекс текущего кластера в структуре allHotSpots
    double powA;                        // показатель степени в диапазоне от 0 до бесконечности

public:
    HotSpotsAlgorithm(LevyMobilityDEF* mobility);
    virtual ~HotSpotsAlgorithm();
    Coord getInitialPosition();
    Coord fixTargetPosition(Coord targetPosition, Coord delta);

private:
    void initialize();
    void checkHotSpotsBound();
    HotSpot* getRandomHotSpot(HotSpot* currentHotSpot);
    Coord getRandomPositionInsideHS(HotSpot* hotSpot);
    double getDistance(int fromHotSpot, int toHotSpot);
    bool isVisited(int i);
    void setVisited(HotSpot* hotSpot);
};

#endif /* HOTSPOTSALGORITHM_H_ */
