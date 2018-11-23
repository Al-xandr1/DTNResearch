#ifndef AREA_H_INCLUDED
#define AREA_H_INCLUDED

/**
* Use this library http://www.boost.org
* The description of usage is here http://www.boost.org/doc/libs/1_55_0/more/getting_started/windows.html
* With IDE CLion it works this hint with https://stackoverflow.com/questions/3897839/how-to-link-c-program-with-boost-using-cmake
*/
#include "boost/multiprecision/float128.hpp"

using namespace std;
using namespace boost::multiprecision;

#define LEVELS 9
#define SUB_AREAS_COUNT 4
#define PI 3.1415926


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
        if (this->bound) delete this->bound;
        if (this->subAreas)
        {
            for (int i = 0; i < getSubAreasCount(); i++)
                if (this->subAreas[i])
                    delete this->subAreas[i];

            delete this->subAreas;
        }
    }

    void setN(int n) {this->n = n;}
    int getN() {return this->n;}
    double getEX() {return this->EX;}
    double getDX() {return this->DX;}
    Area** getSubAreas() {return this->subAreas;}
    Bounds* getBounds() {return this->bound;}
    static int getLevels() {return LEVELS;}
    static int getSubAreasCount() {return SUB_AREAS_COUNT;}

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

        if (subAreas) {
            for(int i = 0; i < getSubAreasCount(); i++) {
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

    bool validDistributionOfN()
    {
        if (this->getN() < 0) return false;
        if (this->subAreas)
        {
            int checkSum = 0;
            for (int i = 0; i < getSubAreasCount(); i++)
            {
                if (this->subAreas[i]->getN() < 0) return false;
                checkSum += this->subAreas[i]->getN();
            }
            if (checkSum != this->getN()) return false;
        }
        return true;
    }

    static Area* createTreeStructure(Bounds* bounds)
    {
        Area* initialArea = new Area(bounds);

        queue<Area*> areasForProcess;
        areasForProcess.push(initialArea);

        for (int l = 0; l < getLevels(); l++)
        {
            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty())
            {
                Area* area = areasPerLevel.front();
                areasPerLevel.pop();

                area->subAreas = new Area*[getSubAreasCount()];

                double middleX = (area->bound->getXMin() + area->bound->getXMax()) / 2;
                double middleY = (area->bound->getYMin() + area->bound->getYMax()) / 2;

                area->subAreas[0] = new Area(area->bound->getXMin(), middleY, middleX, area->bound->getYMax());//S1
                area->subAreas[1] = new Area(middleX, middleY, area->bound->getXMax(), area->bound->getYMax());//S2
                area->subAreas[2] = new Area(area->bound->getXMin(), area->bound->getYMin(), middleX, middleY);//S3
                area->subAreas[3] = new Area(middleX, area->bound->getYMin(), area->bound->getXMax(), middleY);//S4

                if (l != getLevels() - 1) {
                    for(int i = 0; i < getSubAreasCount(); i++) areasForProcess.push(area->subAreas[i]);
                }
            }
        }

        return initialArea;
    }

    static double** computeExDx(Area* rootArea)
    {
        double* ExPerLevel = new double[getLevels()];
        double* DxPerLevel = new double[getLevels()];
        float128 MX2, MX;

        queue<Area*> areasForProcess;
        areasForProcess.push(rootArea);
        double areasCount = getSubAreasCount();

        for (int l = 0; l < getLevels(); l++)
        {
            MX = 0; MX2 = 0;

            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty())
            {
                Area* area = areasPerLevel.front();
                areasPerLevel.pop();

                if (!area->validDistributionOfN()) {cout << "It is not valid distribution of points on level " << l << endl; exit(238);}

                if (area->subAreas != NULL) {
                    for(int i = 0; i < getSubAreasCount(); i++) {
                        MX += float128(area->subAreas[i]->n);
                        MX2 += float128(area->subAreas[i]->n) * float128(area->subAreas[i]->n);
                        
                        areasForProcess.push(area->subAreas[i]);
                    }
                }
            }

            ExPerLevel[l] = double(MX / float128(areasCount));
            //todo есть погрешность какая-то начиная с сотых долей...!!!
            DxPerLevel[l] = double(MX2 * float128(areasCount) / (MX * MX)) - 1;

            areasCount *= getSubAreasCount();
        }

        //todo добавить вычисление параметра херста!

        return new double*[2]{ExPerLevel, DxPerLevel};
    }

    //Запись полученных точек в файл
    static void writePoints(Area* rootArea, ofstream* waypointFile)
    {
        bool firstRow = true;
        queue<Area*> areasForProcess;
        areasForProcess.push(rootArea);

        while (!areasForProcess.empty())
        {
            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty())
            {
                Area* area = areasPerLevel.front();
                areasPerLevel.pop();

                if (area->subAreas)
                {
                    for(int i = 0; i < getSubAreasCount(); i++) areasForProcess.push(area->subAreas[i]);
                }
                else
                {
                    for (int i = 0; i < area->getN(); i++)
                    {
                        if (firstRow) firstRow = false;
                        else (*waypointFile) << endl;
                        Point* randPoint = area->bound->generateRandomPoint();
                        (*waypointFile) << randPoint->x << "\t" << randPoint->y << "\t" << -1.0 << "\t" << -1.0; // todo отсутствие времени обработать
                        delete randPoint;
                    }
                }
            }
        }
    }
};

#endif // AREA_H_INCLUDED
