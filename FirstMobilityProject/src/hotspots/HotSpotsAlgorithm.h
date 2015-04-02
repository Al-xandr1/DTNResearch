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
// ������� ����, ��� ������������� ���������.
#define SIMPLE_LEVY "SimpleLevy"

// ���� � �������������� ���������, ������� ���������� ��������� �������.
#define LEVY_HOTSPOTS_RANDOM "LevyHotSpotsRandom"

// ���� � �������������� ���������, ������� ���������� �� ��������� LATP,
// � ���������� ��� ��������� �������������� ����� �������� ���������.
#define LEVY_HOTSPOTS_LATP_CENTER_LOGIC "LevyHotSpotsLATPCenterLogic"

// ���� � �������������� ���������, ������� ���������� �� ��������� LATP,
// � ���������� ��� ��������� �������������� ����� ������� ���������� � ������� �������� ��������.
#define LEVY_HOTSPOTS_LATP "LevyHotSpotsLATP"

// ���� � �������������� ���������, ������� ���������� �� ��������� LATP,
// � ���������� ��� ��������� �������������� ����� ������� ���������� � ������� �������� ��������
// � ����� ����������� ���������� ��������� ��������� ��� ������� ��������.
#define LEVY_HOTSPOTS_LATP_PATH_COUNTS "LevyHotSpotsLATPPathCounts"


class HotSpotsAlgorithm {
private:
    LevyMobilityDEF* levyMobility;

    //todo ������ ���� ���������� ������� ��������� �������� ����������� �����������
    bool useLATP;                  // true - ���� ����� ������������ LATP ��������, false - �����
    bool useBetweenCentersLogic;   // true - ���� ����� ������� ��������� ����� �������� ���������,
                                   // false - ��������� ��������� �� �������� ��������� �� �������� ��������
    bool useHotSpotAvailabilities; // true - ���� ����� ������������ ������ �� �����������
                                   // ���������� ��������� ��������� �������� �� ������ ����� spotcount.cnt
                                   // false - ���� ���, �.�. ������ ������� ����� �������� ������ ���� ���

    vector<HotSpot*>* allHotSpots; // ��� ��������
    vector<int>* availabilityPerHS;// ������ ��������� ��������� ��������� ��� ���������

    double** distMatrix;           // ������� ���������
    int currentIndexHS;            // ������ �������� �������� � ��������� allHotSpots
    double powA;                   // ���������� ������� � ��������� �� 0 �� �������������

public:
    HotSpotsAlgorithm(LevyMobilityDEF* levyMobility, double powA,
            bool useLATP, bool useBetweenCentersLogic, bool useHotSpotAvailabilities);
    virtual ~HotSpotsAlgorithm();
    Coord getInitialPosition();
    bool fixTargetPosition(Coord& targetPosition, Coord delta, double distance);

private:
    void initialize();
    void initializeHotSpotAvailabilities();
    void initializeDistanceMatrix();
    void checkHotSpotsBound();
    bool setNextCurrentHotSpotIndex();
    Coord getRandomPositionInsideHS(uint hotSpotIndex);
    double getDistanceFromCurrentHS(uint targetHotSpotIndex);
    bool isAvailable(uint hotSpotIndex);
    void setVisited(uint hotSpotIndex);
};

#endif /* HOTSPOTSALGORITHM_H_ */
