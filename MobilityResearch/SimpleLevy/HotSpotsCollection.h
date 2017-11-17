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
    HotSpotData* findHotSpotbyName(const char*, int&);
    void print();
};


/**
 * ����������� ������� ��������� � ������������.
 */
class HSDistanceMatrix {
private:
    static HSDistanceMatrix* instance;     // ��������� �� singleton ������

    vector<double>*  DistanceMatrix;
    double**         ProbabilityMatrix;

    HSDistanceMatrix(double powA) {
        this->DistanceMatrix = NULL;
        this->ProbabilityMatrix = NULL;
        this->makeDistanceMatrix();
        this->makeProbabilityMatrix(powA);
    }

    ~HSDistanceMatrix() {
        if (DistanceMatrix) {
            delete DistanceMatrix;
            DistanceMatrix = NULL;
        }
        if (ProbabilityMatrix) {
            HotSpotsCollection* hsc = HotSpotsCollection::getInstance();
            for(int i=0; i<hsc->getHSData()->size();i++) {
                delete[] ProbabilityMatrix[i];
            }
            delete[]  ProbabilityMatrix;
            ProbabilityMatrix = NULL;
        }
    }

    void makeDistanceMatrix();
    void makeProbabilityMatrix(double powA);

public:
    /**
     * ��������� �������� ���������������������� ������� ��� ������
     */
    static HSDistanceMatrix* getInstance(double powA);

    vector<double>*  getDistanceMatrix()    {return DistanceMatrix;}
    double**         getProbabilityMatrix() {return ProbabilityMatrix;}
    double getDistance(unsigned int i, unsigned int j);
};


#endif /* HOTSPOTSCOLLECTION_H_ */
