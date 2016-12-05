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
 * Коллекция всех локаций в моделировании. Соответствует папке hotspotfiles
 */
class HotSpotsCollection
{
public:
    static bool isHotSpotsCollectionReady;
    static vector<HotSpotData> HSData;     // набор всех локаций

    void readHotSpotsInfo(char* TracesDir, double& minX, double& maxX, double& minY, double& maxY);
    HotSpotData* findHotSpotbyName(char*, int&);
    void print();
    static HotSpotData* randomRemove(vector<HotSpotData*>* hotSpots, int& HotSpotNum);
};


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
