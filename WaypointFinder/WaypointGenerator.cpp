#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <math.h>
#include <queue>
#include "Reader.cpp"

using namespace std;

#define LEVELS 9
#define SUB_AREAS_COUNT 4


class Area {

private:
    int n;              //count of point in this area
    double EX;
    double DX;
    double minX, minY;  // bounds of this area
    double maxX, maxY;

    Area** subAreas;

public:
    Area(double minX, double minY, double maxX, double maxY){
        this->n = 0;
        this->EX = 0;
        this->DX = 0;
        this->minX = minX;
        this->minY = minY;
        this->maxX = maxX;
        this->maxY = maxY;
        this->subAreas = NULL;
    }

    double getN(){return this->n;}

    double getEX(){return this->EX;}
    double getDX(){return this->DX;}

    double getMinX(){return this->minX;}
    double getMinY(){return this->minY;}
    double getMaxX(){return this->maxX;}
    double getMaxY(){return this->maxY;}

    bool isInArea(double x, double y) {
        return (minX <= x && x <= maxX)
                && (minY <= y && y <= maxY);
    }

    bool putInArea(double x, double y) {
        if (!isInArea(x, y)) {
            return false;
        }
        n++;

        if (subAreas != NULL) {
            for(int i=0; i<SUB_AREAS_COUNT; i++) {
                bool isPutted = subAreas[i]->putInArea(x, y);
                if (isPutted) {
                    return true;
                }
            }
            //unreachable statement
            exit(-111);
        }

        return true;
    }

//    void computeLocalExDx() {
//        if (subAreas != NULL) {
//            this->EX = 0;
//            double ex2 = 0;
//            for(int i=0; i<SUB_AREAS_COUNT; i++) {
//                this->EX += subAreas[i]->n;
//                ex2 += (subAreas[i]->n * subAreas[i]->n);
//            }
//            this->EX /= SUB_AREAS_COUNT;
//            this->DX = (ex2 / SUB_AREAS_COUNT) - (this->EX * this->EX);
//        }
//    }

    static Area* createTreeStructure(double minX, double minY, double maxX, double maxY) {
        Area* initialArea = new Area(minX, minY, maxX, maxY);

        queue<Area*> areasForProcess;
        areasForProcess.push(initialArea);

        for (int l=0; l<LEVELS; l++) {
            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty()){
                Area* area = areasPerLevel.front();
                areasPerLevel.pop();

                area->subAreas = new Area*[SUB_AREAS_COUNT];

                double middleX = (area->minX + area->maxX) / 2;
                double middleY = (area->minY + area->maxY) / 2;

                area->subAreas[0] = new Area(area->minX, middleY, middleX, area->maxY);//S1
                area->subAreas[1] = new Area(middleX, middleY, area->maxX, area->maxY);//S2
                area->subAreas[2] = new Area(area->minX, area->minY, middleX, middleY);//S3
                area->subAreas[3] = new Area(middleX, area->minY, area->maxX, middleY);//S4

                if (l != LEVELS - 1) {
                    for(int i=0; i<SUB_AREAS_COUNT; i++) areasForProcess.push(area->subAreas[i]);
                }
            }
        }

        return initialArea;
    }

    static double** computeExDx(Area* area) {
        double* ExPerLevel = new double[LEVELS+1];
        double* DxPerLevel = new double[LEVELS+1];

        queue<Area*> areasForProcess;
        areasForProcess.push(area);
        double areasCount = SUB_AREAS_COUNT;

        for (int l=0; l<LEVELS; l++) {
            ExPerLevel[l] = DxPerLevel[l] = 0;

            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty()){
                Area* area = areasPerLevel.front();
                areasPerLevel.pop();

                if (area->subAreas != NULL) {
                    for(int i=0; i<SUB_AREAS_COUNT; i++) {
                        ExPerLevel[l] += area->subAreas[i]->n;
                        DxPerLevel[l] += (area->subAreas[i]->n * area->subAreas[i]->n);

                        areasForProcess.push(area->subAreas[i]);
                    }
                }
            }

            ExPerLevel[l] /= areasCount;
            DxPerLevel[l] /= areasCount; DxPerLevel[l] -= ExPerLevel[l] * ExPerLevel[l];

            areasCount *= SUB_AREAS_COUNT;
        }
        ExPerLevel[LEVELS] = DxPerLevel[LEVELS] = 0;

        return new double*[2]{ExPerLevel, DxPerLevel};
    }
};



class WaypointGenerator {

private:
    int n;
    double minX, minY;
    double maxX, maxY;

public:
    WaypointGenerator(int n, char* boundsFileName) {
        this->n = n;

        ifstream* boundsFile = new ifstream(boundsFileName);
        if (boundsFile == NULL) {
            cout<<"WaypointGenerator constructor: bounds file "<< boundsFileName <<" is not found." << endl;
            exit(1);
        }
        (*boundsFile) >> minX >> maxX
                      >> minY >> maxY;
    }

    ~WaypointGenerator() {
    }

    void generate() {
        cout << "WayPoints generated!" << endl;
    }

    void analyze(char* waypointFileName, char* statFileName) {
        cout << "WayPoints analyzing start..." << endl;

        Area* initialArea = Area::createTreeStructure(minX, minY, maxX, maxY);

        //Filling of the tree structure
        WayPointReader* reader = new WayPointReader(waypointFileName);
        int row = 1;
        while (reader->hasNext()) {
            WayPoint* point = reader->next();
            //cout << row++ << "  " << point->x << "  " << point->y << endl;
            if (!initialArea->putInArea(point->x, point->y)) {exit(-222);};
        }
        delete reader;


        ofstream* statFile = new ofstream(statFileName);
        if (statFile == NULL) {
            cout << "WaypointGenerator analyze(): Output file " << statFileName <<" opening failed." << endl;
            exit(333);
        }


        double** ExDxPerLevel = Area::computeExDx(initialArea);
        double areasCount = SUB_AREAS_COUNT;
        cout << endl << endl;
        for(int l=0; l<=LEVELS; l++){
            cout << "Level= " << l << "  areas= " << areasCount << "\tEX=" << ExDxPerLevel[0][l] << "\tDX=" << ExDxPerLevel[1][l] << endl;
            (*statFile) << areasCount << "\t" << ExDxPerLevel[0][l] << "\t" << ExDxPerLevel[1][l] << endl;
            areasCount *= SUB_AREAS_COUNT;
        }

        cout << endl << "WayPoints analyzed!" << endl;
    }
};

