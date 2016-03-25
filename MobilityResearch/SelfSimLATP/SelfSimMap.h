#ifndef SELF_SIM_MAP
#define SELF_SIM_MAP

#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <vector>

#include "INETDefs.h"

class SelfSimMapGenerator {
protected:
    double Xmin, Xmax, Ymin, Ymax;
    int levels;
    long int arraySize;
    unsigned int* pointsInArea;
    double* newvar;

public:
    static double* variance;
    static bool isVarfileLoaded;

    std::vector<double> mapx;
    std::vector<double> mapy;

    SelfSimMapGenerator(double xmin, double xmax, double ymin, double ymax, int lvl);
    ~SelfSimMapGenerator();
    void MakeSelfSimSet(char* varfile, int waypoints);
    void PutPointsInArea(int lvl, long int index, double xmin, double xmax, double ymin, double ymax);
    void PutSetOnMap();
};

#endif
