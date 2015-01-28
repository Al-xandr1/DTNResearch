#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <cmath>
#include <queue>
#include "Reader.h"
#include "Bounds.h"

#ifndef WAYPOINTGENERATOR_INCLUDED
#define WAYPOINTGENERATOR_INCLUDED

using namespace std;


class WaypointGenerator {

private:
    int n;
    Bounds* bounds; //граница генерации путевых точек

//    Area* commonAreaTree; //дерево площадей для аналиха дисперсии многих трасс

public:
    WaypointGenerator(int n, char* boundsFileName) {
        this->n = n;
        this->bounds = new Bounds(boundsFileName);

//        this->commonAreaTree = Area::createTreeStructure(this->bounds);
    }

    ~WaypointGenerator() {
        delete this->bounds;
//        delete this->commonAreaTree;
    }

    void generate() {
        cout << "\t" << "WayPoints generated!" << endl;
    }
};

#endif //WAYPOINTGENERATOR_INCLUDED
