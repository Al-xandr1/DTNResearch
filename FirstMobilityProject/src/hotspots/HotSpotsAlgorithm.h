/*
 * HotSpotsAlgorithm.h
 *
 *  Created on: 31 ����� 2015 �.
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
// ��-�� ���������� ����������� ����������� ������ ���
#include "LevyMobility.h"

#define DEF_HS_DIR "./hotspotfiles"    //���������� �� ��������� ��� "������� �����"

// ��������� ���������
#define SIMPLE_LEVY "SimpleLevy"
#define LEVY_HOTSPOTS_RANDOM "LevyHotSpotsRandom"
#define LEVY_HOTSPOTS_LATP_CENTER_LOGIC "LevyHotSpotsLATPCenterLogic"
#define LEVY_HOTSPOTS_LATP "LevyHotSpotsLATP"
#define LEVY_HOTSPOTS_LATP_PATH_COUNTS "LevyHotSpotsLATPPathCounts"


class HotSpotsAlgorithm {
private:
    LevyMobilityDEF* levyMobility;

    //todo ������ ���� ���������� ������� ��������� �������� ����������� �����������
    bool useLATP;                       // true - ���� ����� ������������ LATP ��������, false - �����
    bool useBetweenCentersLogic;        // true - ���� ����� ������� ��������� ����� �������� ���������,
                                        // false - ��������� ��������� �� �������� ��������� �� �������� ��������
    vector<HotSpot*>* allHotSpots;      // ��� ��������
    vector<HotSpot*>* visitedHotSpots;  // ���������� ��������
    double** distMatrix;                // ������� ���������
    HotSpot* currentHotSpot;            // ������� �������
    uint currentIndexHS;                // ������ �������� �������� � ��������� allHotSpots
    double powA;                        // ���������� ������� � ��������� �� 0 �� �������������

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
