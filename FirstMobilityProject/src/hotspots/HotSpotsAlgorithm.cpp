/*
 * HotSpotsAlgorithm.cpp
 *
 *  Created on: 31 ����� 2015 �.
 *      Author: atsarev
 */

#include <HotSpotsAlgorithm.h>

HotSpotsAlgorithm::HotSpotsAlgorithm(LevyMobilityDEF* mobility) {
    levyMobility = mobility;
    useLATP = true;
    useBetweenCentersLogic = false;

    allHotSpots = NULL;
    visitedHotSpots = NULL;
    distMatrix = NULL;
    currentHotSpot = NULL;
    currentIndexHS = -1;
    powA = -1;

    initialize();
}

HotSpotsAlgorithm::~HotSpotsAlgorithm() {
}

void HotSpotsAlgorithm::initialize() {
    HotSpotReader hsReader;
    allHotSpots = hsReader.readAllHotSpots(DEF_HS_DIR);
    visitedHotSpots = new vector<HotSpot*>();
    checkHotSpotsBound();

    if (useBetweenCentersLogic) {
        // ��������� ������� ���������� ����� �������� �������
        distMatrix = new double*[allHotSpots->size()];
        for (uint i = 0; i < allHotSpots->size(); i++) {
            distMatrix[i] = new double[allHotSpots->size()];
            for (uint j = 0; j < allHotSpots->size(); j++) {
                if (i == j) {
                    distMatrix[i][i] = -1;
                } else {
                    distMatrix[i][j] = (*allHotSpots)[i]->getDistance((*allHotSpots)[j]);
                }
            }
        }
    }
}

// ���������: ��� ��� ������� ����� �� ������� �� ����� �������
void HotSpotsAlgorithm::checkHotSpotsBound() {
    if (allHotSpots) {
        for (uint i = 0; i < allHotSpots->size(); i++) {
            HotSpot* hotSpot = (*allHotSpots)[i];
            if (hotSpot->Xmin < levyMobility->getConstraintAreaMin().x || hotSpot->Xmax > levyMobility->getConstraintAreaMax().x ||
                hotSpot->Ymin < levyMobility->getConstraintAreaMin().y || hotSpot->Ymax > levyMobility->getConstraintAreaMax().y) {
                cout << "HotSpots has wrong bounds!"; EV << "HotSpots has wrong bounds!";
                exit(123);
            }
        }
    }
}

// �������� ��������� ������� ����� �� ������ hotSpots, �������� �� ��������� excludedHotSpot
HotSpot* HotSpotsAlgorithm::getRandomHotSpot(HotSpot* currentHotSpot) {
    uint index = 0;
    HotSpot* newHotSpot = NULL;

    if (useLATP && currentHotSpot != NULL) {
        // ����� �� ��������� "Least action trip planning"
        // � � ������, ����� ��� ���������� ������� �������, ������ ��� ����� ���������� currentIndexHS

        double checkSum = 0;
        // ����������� ��������� � ������� ��������������� ������� �� ����� ����������
        double* hotSpotProbability = new double[allHotSpots->size()];
        for (uint i = 0; i < allHotSpots->size(); i++) {
            if (i != currentIndexHS && !isVisited(i)) {
                // ���� ������� �� ������� � �� ����������, �� ������� ����������� ��� ���������
                double denominator = 0;
                for (uint k = 0; k < allHotSpots->size(); k++)
                    if (k != currentIndexHS && !isVisited(k)) denominator += 1 / pow(getDistance(currentIndexHS, k), powA);
                if (denominator == 0) exit(-111);

                checkSum += (hotSpotProbability[i] = 1 / pow(getDistance(currentIndexHS, i), powA) / denominator);
            } else {
                // ������� ����� ������� ����������� ���������
                checkSum += (hotSpotProbability[i] = 0);
            }
        }
        if (checkSum != 1) cout << "\t\t checkSum = " << checkSum << endl;

        double rnd = ((double) rand()) / RAND_MAX,
               probSumm = 0;
        for (uint i = 0; i < allHotSpots->size(); i++)
            if ( (probSumm += hotSpotProbability[i]) >= rnd ) {index = i; break;}
        newHotSpot = (*allHotSpots)[index];

    } else {
        // ������� ��������� ����� ��������: ���� ��� LATP ���� ��� ������ ����������� �������� ��������
        newHotSpot = currentHotSpot;
        do {
            index = rint(uniform(0, allHotSpots->size() - 1));
            newHotSpot = (*allHotSpots)[index];
        } while (newHotSpot == currentHotSpot);
    }

    // ���������� ������� ������
    if (index == currentIndexHS) exit(-432);
    currentIndexHS = index;
    if (useLATP) setVisited(newHotSpot);

    cout << "getRandomHotSpot: index = " << currentIndexHS << ", size = " << allHotSpots->size() << endl;
    return newHotSpot;
}

// �������� ��������� ��������� ������ �������� ������� �����
Coord HotSpotsAlgorithm::getRandomPositionInsideHS(HotSpot* hotSpot) {
    Coord newPoint(uniform(hotSpot->Xmin, hotSpot->Xmax),
                   uniform(hotSpot->Ymin, hotSpot->Ymax),
                   0);
    if (!hotSpot->isPointBelong(newPoint)){
        exit(-343);
    }
    return newPoint;
}

Coord HotSpotsAlgorithm::getInitialPosition() {
    currentHotSpot = getRandomHotSpot(NULL);
    return getRandomPositionInsideHS(currentHotSpot);
}

Coord HotSpotsAlgorithm::fixTargetPosition(Coord targetPosition, Coord delta, double distance) {
    // ����������� �� ����� ����� ������� ������� �����
    if ( !currentHotSpot->isPointBelong(targetPosition) ) {
        // ��� ��������� ��������� ��������������� ����������
        double x, y, Xdir, Ydir, dir;
        bool flag = ( (y=levyMobility->getLastPosition().y) < currentHotSpot->Ycenter);

        // �������� ����� ������� ������� ��������������
        if ( (x=levyMobility->getLastPosition().x) < currentHotSpot->Xcenter ) {
            if (flag) { Xdir=currentHotSpot->Xmax-x; Ydir=currentHotSpot->Ymax-y; }
            else      { Xdir=currentHotSpot->Xmax-x; Ydir=currentHotSpot->Ymin-y; }
        } else {
            if (flag) { Xdir=currentHotSpot->Xmin-x; Ydir=currentHotSpot->Ymax-y; }
            else      { Xdir=currentHotSpot->Xmin-x; Ydir=currentHotSpot->Ymin-y; }
        }

        double distance = levyMobility->getLastPosition().distance(targetPosition);
        // ���������, ����� �� �������� � ��������������
        if ( distance > (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
            // TODO � ���� ������ ��� ����� ���� ����� ������� ��� �� ����������� ����� (������) ��� ������� � ����, ������ ��������� ���
            //����� ���� �������� ��������� �������
            currentHotSpot = getRandomHotSpot(currentHotSpot);
            return getRandomPositionInsideHS(currentHotSpot);
        } else {
            delta.x = Xdir * distance/dir;
            delta.y = Ydir * distance/dir;
            return levyMobility->getLastPosition() + delta;
        }
    }

    return targetPosition;
}

// ��������� ��������� ����� ��������� �� �� ������� � �������� ������� allHotSpots.
// � ����������� �� ��������� useBetweenCentersLogic ������� ���������� ���� ����� �������� �������,
// ���� ��� ����� ������� ���������� � ������� ������� ��������
double HotSpotsAlgorithm::getDistance(int fromHotSpot, int toHotSpot) {
    if (fromHotSpot == toHotSpot) {
        exit(-765);
    }

    if (useBetweenCentersLogic) {
        return distMatrix[fromHotSpot][toHotSpot];

    } else {
        HotSpot* targetHotSpot = (*allHotSpots)[toHotSpot];
        double deltaX = (targetHotSpot->Xcenter - levyMobility->getLastPosition().x),
               deltaY = (targetHotSpot->Ycenter - levyMobility->getLastPosition().y);
        return sqrt(deltaX * deltaX + deltaY * deltaY);
    }
}

// ��������� ���������� ������� ��� ���
bool HotSpotsAlgorithm::isVisited(int i) {
    HotSpot* currentHS = (*allHotSpots)[i];
    for (uint j=0; j < visitedHotSpots->size(); j++) {
        if ( (*visitedHotSpots)[j] == currentHS ) return true;
    }
    return false;
}

// ��������� ��������� ���������� ���������
void HotSpotsAlgorithm::setVisited(HotSpot* hotSpot) {
    visitedHotSpots->push_back(hotSpot);
    // ���� ���-�� ���������� ��������� ������ ���������� ������, �� ������� ����� ������,
    // ��� ����� ������� ����������� ��������� ���� �����
    if (visitedHotSpots->size() >=  rint(allHotSpots->size() * 0.8)) {
        visitedHotSpots->erase(visitedHotSpots->begin());
    }
}
