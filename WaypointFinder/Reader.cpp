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

using namespace std;

struct TracePoint {
    double t;
    double x;
    double y;

    TracePoint(double t, double x, double y) {
        this->t = t;
        this->x = x;
        this->y = y;
    }

    TracePoint(TracePoint* point) {
        this->t = point->t;
        this->x = point->x;
        this->y = point->y;
    }
};



struct WayPoint {
    double x;
    double y;
    double tMin;
    double tMxB;

    WayPoint(double x, double y, double tMin, double tMxB) {
        this->x = x;
        this->y = y;
        this->tMin = tMin;
        this->tMxB = tMxB;
    }

    WayPoint(WayPoint* point) {
        this->x = point->x;
        this->y = point->y;
        this->tMin = point->tMin;
        this->tMxB = point->tMxB;
    }
};



template <typename Point>
class Reader {

protected:
    ifstream* traceFile;
    Point* nextPoint;

protected:
    Reader(char* fileName) {
        traceFile = new ifstream(fileName);
        if (traceFile == NULL) {
            cout<<"Reader constructor: Input file "<< fileName <<" is not found." << endl;
            exit(1);
        }
        nextPoint = NULL;
    }

    virtual ~Reader() {
        traceFile->close();
        delete traceFile;
    }

public:
    bool hasNext() {
        nextPoint = NULL;
        if (!traceFile->eof()) {
            nextPoint = createPoint();
        }
        return nextPoint != NULL;
    }

    Point* next() {
        return nextPoint;
    }

protected:
    virtual Point* createPoint() {
        return NULL;
    }
};



class WayPointReader : public Reader<WayPoint> {
public:
    WayPointReader(char* fileName) : Reader(fileName){
    }

protected:
    virtual WayPoint* createPoint(){
        double x, y, tMin, tMxB;
        (*traceFile) >> x >> y >> tMin >> tMxB;
//todo считывает последнюю люшнюю строчку. НУЖНО УБИРАТЬ ПОСЛЕДНЮЮ ПУСТУЮ СТРОКУ
        return new WayPoint(x, y, tMin, tMxB);
    }
};



class TracePointReader : public Reader<TracePoint> {
public:
    TracePointReader(char* fileName) : Reader(fileName){
    }

protected:
    virtual TracePoint* createPoint(){
        double t, x, y;
        (*traceFile) >> t >> x >> y;
//todo считывает последнюю люшнюю строчку. НУЖНО УБИРАТЬ ПОСЛЕДНЮЮ ПУСТУЮ СТРОКУ
        return new TracePoint(t, x, y);
    }
};
