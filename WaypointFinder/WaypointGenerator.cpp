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


#define ERR 0.5   //проверяем границы с точностью до полуметра
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

class Bounds {

private:
    double XMin, XMax, YMin, YMax;

public:
    Bounds() {
        XMin = YMin = 10e10;
        XMax = YMax = -10e10;
    }

    Bounds(double XMin, double YMin, double XMax, double YMax) {
        this->XMin = XMin;
        this->YMin = YMin;
        this->XMax = XMax;
        this->YMax = YMax;
    }

    Bounds(Bounds* bounds) {
        this->XMin = bounds->XMin;
        this->YMin = bounds->YMin;
        this->XMax = bounds->XMax;
        this->YMax = bounds->YMax;
    }

    Bounds(char* fileName) {
        read(fileName);
    }

    void changeBounds(double x, double y) {
        XMin = MIN(x, XMin);
        XMax = MAX(x, XMax);
        YMin = MIN(y, YMin);
        YMax = MAX(y, YMax);
    }

    double getXMin() {return XMin;}
    double getXMax() {return XMax;}
    double getYMin() {return YMin;}
    double getYMax() {return YMax;}

    void write(char* fileName) {
        ofstream boundsFile(fileName);
        if (boundsFile == NULL) {
            cout << "\t" << "Bounds write: bounds file "<< fileName <<" is not found." << endl;
            exit(-112);
        }
        //сделать окугление в большую по модулю сторону
        boundsFile << (XMin-ERR) << "\t" << (XMax+ERR) << endl;
        boundsFile << (YMin-ERR) << "\t" << (YMax+ERR);
        boundsFile.close();
    }

    void read(char* fileName) {
        ifstream boundsFile(fileName);
        if (boundsFile == NULL) {
            cout << "\t" << "Bounds read: bounds file "<< fileName <<" is not found." << endl;
            exit(-113);
        }
        boundsFile >> XMin >> XMax
                   >> YMin >> YMax;
        boundsFile.close();
    }

    void print() {
        cout << "\t" << "Xmin=" << XMin << "\t Xmax=" << XMax << endl;
        cout << "\t" << "Ymin=" << YMin << "\t Ymax=" << YMax << endl;
    }
};



#define LEVELS 9
#define SUB_AREAS_COUNT 4

class Area {

private:
    int n;              //count of point in this area
    double EX;
    double DX;
    Bounds* bound;

    Area** subAreas;

public:
    Area(Bounds* bound){
        this->n = 0;
        this->EX = 0;
        this->DX = 0;
        this->bound = new Bounds(bound);
        this->subAreas = NULL;
    }

    Area(double XMin, double YMin, double XMax, double YMax) {
        this->n = 0;
        this->EX = 0;
        this->DX = 0;
        this->bound = new Bounds(XMin, YMin, XMax, YMax);
        this->subAreas = NULL;
    }

    ~Area() {
        delete this->bound;
        if (subAreas != NULL) {
            for (int i=0; i<SUB_AREAS_COUNT; i++)
                if (this->subAreas[i] != NULL)
                    delete this->subAreas[i];

            delete this->subAreas;
        }
    }

    double getN(){return this->n;}

    double getEX(){return this->EX;}
    double getDX(){return this->DX;}

    Bounds* getBounds() {return this->bound;}

    bool isInArea(double x, double y) {
        return (this->bound->getXMin() <= x && x <= this->bound->getXMax())
                && (this->bound->getYMin() <= y && y <= this->bound->getYMax());
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

    void computeLocalExDx() {
        if (subAreas != NULL) {
            this->EX = 0;
            double ex2 = 0;
            for(int i=0; i<SUB_AREAS_COUNT; i++) {
                this->EX += subAreas[i]->n;
                ex2 += (subAreas[i]->n * subAreas[i]->n);
            }
            this->EX /= SUB_AREAS_COUNT;
            this->DX = (ex2 / SUB_AREAS_COUNT) - (this->EX * this->EX);
        }
    }

    static Area* createTreeStructure(Bounds* bounds) {
        Area* initialArea = new Area(bounds);

        queue<Area*> areasForProcess;
        areasForProcess.push(initialArea);

        for (int l=0; l<LEVELS; l++) {
            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty()){
                Area* area = areasPerLevel.front();
                areasPerLevel.pop();

                area->subAreas = new Area*[SUB_AREAS_COUNT];

                double middleX = (area->bound->getXMin() + area->bound->getXMax()) / 2;
                double middleY = (area->bound->getYMin() + area->bound->getYMax()) / 2;

                area->subAreas[0] = new Area(area->bound->getXMin(), middleY, middleX, area->bound->getYMax());//S1
                area->subAreas[1] = new Area(middleX, middleY, area->bound->getXMax(), area->bound->getYMax());//S2
                area->subAreas[2] = new Area(area->bound->getXMin(), area->bound->getYMin(), middleX, middleY);//S3
                area->subAreas[3] = new Area(middleX, area->bound->getYMin(), area->bound->getXMax(), middleY);//S4

                if (l != LEVELS - 1) {
                    for(int i=0; i<SUB_AREAS_COUNT; i++) areasForProcess.push(area->subAreas[i]);
                }
            }
        }

        return initialArea;
    }

    static double** computeExDx(Area* rootArea) {
        double* ExPerLevel = new double[LEVELS+1];
        double* DxPerLevel = new double[LEVELS+1];

        queue<Area*> areasForProcess;
        areasForProcess.push(rootArea);
        double areasCount = SUB_AREAS_COUNT;

        for (int l=0; l<LEVELS; l++) {
            ExPerLevel[l] = DxPerLevel[l] = 0;

            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty()){
                Area* rootArea = areasPerLevel.front();
                areasPerLevel.pop();

                if (rootArea->subAreas != NULL) {
                    for(int i=0; i<SUB_AREAS_COUNT; i++) {
                        ExPerLevel[l] += rootArea->subAreas[i]->n;
                        DxPerLevel[l] += (rootArea->subAreas[i]->n * rootArea->subAreas[i]->n);

                        areasForProcess.push(rootArea->subAreas[i]);
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

    static void writeStatistics(Area* rootArea, char* statFileName) {
        ofstream statFile(statFileName);
        if (statFile == NULL) {
            cout << "\t" << "Area write(): Output file " << statFileName <<" opening failed." << endl;
            exit(333);
        }

        double** ExDxPerLevel = Area::computeExDx(rootArea);
        double areasCount = SUB_AREAS_COUNT;
        cout << endl << endl;
        for(int l=0; l<LEVELS; l++){
            cout << "\t" << "Level= " << l << "  areas= " << areasCount << "\tEX=" << ExDxPerLevel[0][l] << "\tDX=" << ExDxPerLevel[1][l] << endl;
            statFile << areasCount << "\t" << ExDxPerLevel[0][l] << "\t" << ExDxPerLevel[1][l] << endl;
            areasCount *= SUB_AREAS_COUNT;
        }
        delete ExDxPerLevel[0];
        delete ExDxPerLevel[1];
        delete ExDxPerLevel;

        statFile.close();
    }
};



class WaypointGenerator {

private:
    int n;
    Bounds* bounds; //граница генерации путевых точек

    Area* commonAreaTree; //дерево площадей для аналиха дисперсии многих трасс

public:
    WaypointGenerator(int n, char* boundsFileName) {
        this->n = n;
        this->bounds = new Bounds(boundsFileName);

        this->commonAreaTree = Area::createTreeStructure(this->bounds);
    }

    ~WaypointGenerator() {
        delete this->bounds;
        delete this->commonAreaTree;
    }

    void generate() {
        cout << "\t" << "WayPoints generated!" << endl;
    }

    void analyze(char* waypointFileName, char* statFileName) {
        cout << "\t" << "WayPoints analyzing start..." << endl;

        Area* initialArea = Area::createTreeStructure(this->bounds);

        //Filling of the tree structure
//        WayPointReader* reader = new WayPointReader(waypointFileName);
        TracePointReader* reader = new TracePointReader(waypointFileName);
        int row = 1;
        while (reader->hasNext()) {
//            WayPoint* point = reader->next();
            TracePoint* point = reader->next();
            if (!initialArea->putInArea(point->x, point->y)) {
                cout << "\t" << row << "  " << point->x << "  " << point->y << endl;
                exit(-222);
            }
            if (!commonAreaTree->putInArea(point->x, point->y)) {
                cout << "\t" << row << "  " << point->x << "  " << point->y << endl;
                exit(-223);
            }
            delete point;
            row++;
        }
        delete reader;

        Area::writeStatistics(initialArea, statFileName);
        delete initialArea;

        cout << endl << "\t" << "WayPoints analyzed!" << endl << endl;
    }

    void writeStatistics(char* statFileName) {
        Area::writeStatistics(commonAreaTree, statFileName);
    }
};

