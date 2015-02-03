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
#include "Area.h"

#ifndef WAYPOINTGENERATOR_INCLUDED
#define WAYPOINTGENERATOR_INCLUDED

using namespace std;


class WaypointGenerator {

private:
    int n;
    double R;
    Bounds* bounds; //граница генерации путевых точек
    Area* areaTree; //дерево площадей для аналиха дисперсии многих трасс

public:
    WaypointGenerator(int n, double R, char* boundsFileName)
    {
        this->n = n;
        this->R = R;
        this->bounds = new Bounds(boundsFileName);
        this->areaTree = Area::createTreeStructure(this->bounds);
    }

    ~WaypointGenerator()
    {
        if (this->bounds) delete this->bounds;
        if (this->areaTree) delete this->areaTree;
    }

    void generate(char* wayPointsFileName) {
        cout << "\t" << "WayPoints generating start..." << endl;

        cout << "!!!! R are not used !!!!" << endl;                     //todo

        ofstream* wayPointsFile = new ofstream(wayPointsFileName);
        if (wayPointsFile == NULL)
        {
            cout << endl << "\t" << "WaypointGenerator generate(): Output file " << wayPointsFileName << " opening failed." << endl;
            exit(344);
        }
        Area::generateNperAreaAndSave(this->areaTree, this->n, wayPointsFile);
        wayPointsFile->close();
        delete wayPointsFile;
        cout << "\t" << "WayPoints generated!" << endl;
    }
};

#endif //WAYPOINTGENERATOR_INCLUDED
