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
    vector<uint>* availabilityPerHS;   // ������ ��������� ��������� ��������� ��� ���������

    double** distMatrix;                // ������� ���������
    uint currentIndexHS;                // ������ �������� �������� � ��������� allHotSpots
    double powA;                        // ���������� ������� � ��������� �� 0 �� �������������

public:
    HotSpotsAlgorithm(LevyMobilityDEF* levyMobility, double powA);
    virtual ~HotSpotsAlgorithm();
    Coord getInitialPosition();
    Coord fixTargetPosition(Coord targetPosition, Coord delta, double distance);

private:
    void initialize();
    void checkHotSpotsBound();
    int getNextHotSpotIndex(int hotSpotIndex);
    Coord getRandomPositionInsideHS(HotSpot* hotSpot);
    double getDistance(int fromHotSpot, int toHotSpot);
    bool isVisited(uint hotSpotIndex);
    void setVisited(uint hotSpotIndex);
};

#endif /* HOTSPOTSALGORITHM_H_ */
