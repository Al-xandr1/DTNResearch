#ifndef HOTSPOTSCOLLECTION_H_
#define HOTSPOTSCOLLECTION_H_

#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <windows.h>
#include "INETDefs.h"
#include "Data.h"
#include "Coord.h"
#include "DevelopmentHelper.h"

using namespace std;


/**
 * ��������� ���� ������� � �������������. ������������� ����� hotspotfiles. Singleton ������
 */
class HotSpotsCollection {
private:
    static HotSpotsCollection* instance;   // ��������� �� singleton ������

    vector<HotSpotData>* HSData;           // ����� ���� �������

    HotSpotsCollection() {
        this->HSData = NULL;
        this->readHotSpotsInfo(DEF_TR_DIR, SPC_FILE);
        this->print();
    }

    ~HotSpotsCollection() {
        if (HSData) delete HSData;
    }

    void readHotSpotsInfo(char* TracesDir, char* spotcountfile);

public:
    /**
     * ��������� �������� ���������������������� ������� ��� ������
     */
    static HotSpotsCollection* getInstance();
    static HotSpotData* randomRemove(vector<HotSpotData*>* hotSpots, int& HotSpotNum);

    vector<HotSpotData>* getHSData() {return HSData;}
    void getTotalSize(double& minX, double& maxX, double& minY, double& maxY);
    HotSpotData* findHotSpotbyName(char*, int&);
    void print();
};


/**
 * ����������� ������� ��������� � ������������.
 */
class HSDistanceMatrix
{
public:
    static bool isMatrixReady;
    static vector<double>*  DistanceMatrix;

    static bool isProbabilityReady;
    static double**  ProbabilityMatrix;

    void makeDistanceMatrix();
    void makeProbabilityMatrix(double powA);
    double getDistance(unsigned int i, unsigned int j);
};


#endif /* HOTSPOTSCOLLECTION_H_ */
