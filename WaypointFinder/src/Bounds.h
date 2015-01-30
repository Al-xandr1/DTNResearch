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

#ifndef BOUNDS_H_INCLUDED
#define BOUNDS_H_INCLUDED


#define ERR 0.5   //нужно из-за ошибки округления при записи в файл
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

using namespace std;


class Bounds
{

private:
    //todo XMin, XMax, YMin, YMax;  -> Point minPoint, maxPoint;
    double XMin, XMax, YMin, YMax;

public:
    Bounds()
    {
        XMin = YMin = 10e10;
        XMax = YMax = -10e10;
    }

    Bounds(double XMin, double YMin, double XMax, double YMax)
    {
        this->XMin = XMin;
        this->YMin = YMin;
        this->XMax = XMax;
        this->YMax = YMax;
    }

    Bounds(Bounds* bounds)
    {
        this->XMin = bounds->XMin;
        this->YMin = bounds->YMin;
        this->XMax = bounds->XMax;
        this->YMax = bounds->YMax;
    }

    Bounds(char* fileName)
    {
        read(fileName);
    }

    double getXMin() {return XMin;}
    double getXMax() {return XMax;}
    double getYMin() {return YMin;}
    double getYMax() {return YMax;}

    double getSquare() {return abs(XMax - XMin) * abs(YMax - YMin);}
    double getDiagLength()
    {
        return sqrt((XMax - XMin)*(XMax - XMin)
                  + (YMax - YMin)*(YMax - YMin));
    }

    void changeBounds(double x, double y)
    {
        XMin = MIN(x, XMin);
        XMax = MAX(x, XMax);
        YMin = MIN(y, YMin);
        YMax = MAX(y, YMax);
    }

    void write(char* fileName)
    {
        ofstream boundsFile(fileName);
        if (boundsFile == NULL) {
            cout << "\t" << "Bounds write: bounds file "<< fileName <<" is not found." << endl;
            exit(-112);
        }
        boundsFile << (XMin-ERR) << "\t" << (XMax+ERR) << endl;
        boundsFile << (YMin-ERR) << "\t" << (YMax+ERR);
        boundsFile.close();
    }

    void read(char* fileName)
    {
        ifstream boundsFile(fileName);
        if (boundsFile == NULL) {
            cout << "\t" << "Bounds read: bounds file "<< fileName <<" is not found." << endl;
            exit(-113);
        }
        boundsFile >> XMin >> XMax
                   >> YMin >> YMax;
        boundsFile.close();
    }

    void print()
    {
        cout << "\t" << "Xmin=" << XMin << "\t Xmax=" << XMax << endl;
        cout << "\t" << "Ymin=" << YMin << "\t Ymax=" << YMax << endl;
    }
};

#endif // BOUNDS_H_INCLUDED
