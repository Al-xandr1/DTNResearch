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
#include "TrafficHistogram.cpp"

#ifndef WAYPOINTANALYZER_INCLUDED
#define WAYPOINTANALYZER_INCLUDED

using namespace std;

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
    Area(Bounds* bound)
    {
        this->n = 0;
        this->EX = 0;
        this->DX = 0;
        this->bound = new Bounds(bound);
        this->subAreas = NULL;
    }

    Area(double XMin, double YMin, double XMax, double YMax)
    {
        this->n = 0;
        this->EX = 0;
        this->DX = 0;
        this->bound = new Bounds(XMin, YMin, XMax, YMax);
        this->subAreas = NULL;
    }

    ~Area()
    {
        delete this->bound;
        if (subAreas != NULL) {
            for (int i=0; i<SUB_AREAS_COUNT; i++)
                if (this->subAreas[i] != NULL)
                    delete this->subAreas[i];

            delete this->subAreas;
        }
    }

    double getN() {return this->n;}
    double getEX() {return this->EX;}
    double getDX() {return this->DX;}
    Bounds* getBounds() {return this->bound;}

    bool isInArea(Point* point)
    {
        return (this->bound->getXMin() <= point->x && point->x <= this->bound->getXMax())
            && (this->bound->getYMin() <= point->y && point->y <= this->bound->getYMax());
    }

    bool putInArea(Point* point)
    {
        if (!isInArea(point)) {
            return false;
        }
        n++;

        if (subAreas != NULL) {
            for(int i=0; i<SUB_AREAS_COUNT; i++) {
                bool isPutted = subAreas[i]->putInArea(point);
                if (isPutted) {
                    return true;
                }
            }
            //unreachable statement
            exit(-111);
        }

        return true;
    }

    void computeLocalExDx()
    {
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

    static Area* createTreeStructure(Bounds* bounds)
    {
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

    static double** computeExDx(Area* rootArea)
    {
        double* ExPerLevel = new double[LEVELS];
        double* DxPerLevel = new double[LEVELS];

        queue<Area*> areasForProcess;
        areasForProcess.push(rootArea);
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

            DxPerLevel[l] /= areasCount;
            DxPerLevel[l] /= ExPerLevel[l] * ExPerLevel[l];
            DxPerLevel[l] -= 1;

            areasCount *= SUB_AREAS_COUNT;
        }

        return new double*[2]{ExPerLevel, DxPerLevel};
    }

    static void writeStatistics(ofstream* out, Area* rootArea)
    {
        double** ExDxPerLevel = Area::computeExDx(rootArea);
        double areasCount = SUB_AREAS_COUNT;
        cout << endl << endl;
        cout << "\t<EX-DX-STAT>" << endl;
        *out << "  <EX-DX-STAT>" << endl;
        for(int l=0; l<LEVELS; l++){
            cout << "\t" << "Level= " << l << "  areas= " << areasCount << "\tEX=" << ExDxPerLevel[0][l] << "\tDX=" << ExDxPerLevel[1][l] << endl;
            *out << areasCount << "\t" << ExDxPerLevel[0][l] << "\t" << ExDxPerLevel[1][l] << endl;
            areasCount *= SUB_AREAS_COUNT;
        }
        cout << "\t</EX-DX-STAT>" << endl;
        *out << "  </EX-DX-STAT>" << endl;
        delete ExDxPerLevel[0];
        delete ExDxPerLevel[1];
        delete ExDxPerLevel;
    }
};



class Statistics
{
private:
    Area* areaTree; //дерево площадей для аналиха дисперсии многих трасс
    vector<double>* lengthHist;
    vector<double>* velocityHist;
    vector<double>* pauseHist;
    WayPoint *previous;

public:
    Statistics(Bounds* bounds)
    {
        this->areaTree = Area::createTreeStructure(bounds);
        this->lengthHist = new vector<double>();
        this->velocityHist = new vector<double>();
        this->pauseHist = new vector<double>();
        this->previous = NULL;
    }

    ~Statistics()
    {
        delete this->areaTree;
        delete this->lengthHist;
        delete this->velocityHist;
        delete this->pauseHist;
        if (previous != NULL) delete previous;
    }

    bool addPoint(WayPoint* point)
    {
        bool success = areaTree->putInArea(point);

        if (previous != NULL)
        {
            double dist = previous->distance(point);
            double flyDur = previous->flyDuration(point);
            double pause = previous->pauseDuration();

            lengthHist->push_back(dist);
            velocityHist->push_back(dist/flyDur);
            pauseHist->push_back(pause);

            delete previous;
        }
        previous = new WayPoint(point);

        return success;
    }

    void printParams()
    {
        this->areaTree->getBounds()->print();
    }

    void writeVector(ofstream* out, char* tag, vector<double>* vec)
    {
        cout << "\t<" << tag << ">" << endl;
        *out << "  <" << tag << ">" << endl;
        for (int i=0; i<vec->size(); i++)
        {
            cout << (*vec)[i] << "  ";
            *out << (*vec)[i] << "  ";
        }
        cout << endl << "\t</" << tag << ">" << endl;
        *out << endl << "  </" << tag << ">" << endl;
    }

    void write(char* statFileName)
    {
        ofstream statFile(statFileName);
        if (statFile == NULL) {
            cout << "\t" << "Statistics write(): Output file " << statFileName << " opening failed." << endl;
            exit(333);
        }
        statFile << "<?xml version=\'1.0' ?>" << endl;
        statFile << "<STATISTICS>" << endl;
        Area::writeStatistics(&statFile, areaTree);
        writeVector(&statFile, "LENGTH-HIST",   this->lengthHist);
        writeVector(&statFile, "VELOCITY-HIST", this->velocityHist);
        writeVector(&statFile, "PAUSE-HIST",    this->pauseHist);
        statFile << "</STATISTICS>" << endl;
    }
};



class PointsAnalyzer {
    //todo доработать для испольщования для трасс

private:
    Bounds* bounds; //граница генерации путевых точек
    Statistics* commonStat;

public:
    PointsAnalyzer(char* boundsFileName)
    {
        this->bounds = new Bounds(boundsFileName);
        this->commonStat = new Statistics(this->bounds);
    }

    ~PointsAnalyzer()
    {
        delete this->bounds;
        delete this->commonStat;
    }

    void analyze(char* pointsFileName, char* statFileName)
    {
        cout << "\t" << "Points analyzing start...";
        Statistics* statPerFile = new Statistics(this->bounds);

        WayPointReader* reader = new WayPointReader(pointsFileName);
        WayPoint *point = NULL;
        int row = 1;
        while (reader->hasNext()) {
            point = reader->next();
            if (!statPerFile->addPoint(point))
            {
                cout << "\t" << row << "  "; point->print();
                statPerFile->printParams();
                exit(-222);
            }
            if (!commonStat->addPoint(point))
            {
                cout << "\t" << row << "  "; point->print();
                commonStat->printParams();
                exit(-223);
            }
            row++;
            delete point;
        }
        delete reader;

        statPerFile->write(statFileName);

        delete statPerFile;
        cout << endl << "\t" << "Points analyzed!" << endl << endl;
        cout << "------------------------------------------------------------------------------" << endl;
    }

    void writeStatistics(char* statFileName)
    {
        this->commonStat->write(statFileName);
    }
};

#endif //WAYPOINTANALYZER_INCLUDED
