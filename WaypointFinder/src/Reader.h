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

#ifndef READER_H_INCLUDED
#define READER_H_INCLUDED

using namespace std;


class Point
{
public:
    double x, y;

    Point(double x, double y)
    {
        this->x = x;
        this->y = y;
    }

    Point(Point* point)
    {
        this->x = point->x;
        this->y = point->y;
    }

    double distance(Point* follower)
    {
        return sqrt((follower->x - this->x)*(follower->x - this->x)
                  + (follower->y - this->y)*(follower->y - this->y));
    }

    void print()
    {
        cout << "x= " << this->x << "  y= " << this->y << endl;
    }
};



class TracePoint : public Point
{
public:
    double t;

    TracePoint(double t, double x, double y) : Point(x, y)
    {
        this->t = t;
    }

    TracePoint(TracePoint* point) : Point(point->x, point->y)
    {
        this->t = point->t;
    }

    double flyDuration(TracePoint* follower)
    {
        return follower->t - this->t;
    }
};



class WayPoint : public TracePoint
{
public:
    double tMxB;

    WayPoint(double x, double y, double t, double tMxB) : TracePoint(t, x, y)
    {
        this->tMxB = tMxB;
    }

    WayPoint(WayPoint* point) : TracePoint(point->t, point->x, point->y)
    {
        this->tMxB = point->tMxB;
    }

    double flyDuration(WayPoint* follower)
    {
        return follower->t - this->tMxB;
    }

    double pauseDuration()
    {
        return this->tMxB - this->t;
    }
};



template <typename Point>
class Reader
{
protected:
    ifstream* traceFile;
    Point* nextPoint;

protected:
    Reader(char* fileName)
    {
        traceFile = new ifstream(fileName);
        if (traceFile == NULL) {
            cout<<"Reader constructor: Input file "<< fileName <<" is not found." << endl;
            exit(1);
        }
        nextPoint = NULL;
    }

    virtual ~Reader()
    {
        traceFile->close();
        delete traceFile;
    }

public:
    bool hasNext()
    {
        nextPoint = NULL;
        if (!traceFile->eof()) {
            nextPoint = readPoint();
        }
        return nextPoint != NULL;
    }

    Point* next()
    {
        return nextPoint;
    }

protected:
    virtual Point* readPoint()
    {
        return NULL;
    }
};



class TracePointReader : public Reader<TracePoint>
{
public:
    TracePointReader(char* fileName) : Reader(fileName){}

protected:
    virtual TracePoint* readPoint()
    {
        double t, x, y;
        t = x = y = -10e10;
        (*traceFile) >> t >> x >> y;
        //т.к. последн€€ строка (ѕ”—“јя) считываетс€ криво
        if (t != -10e10)
            return new TracePoint(t, x, y);
        else
            return NULL;
    }
};



class WayPointReader : public Reader<WayPoint>
{
public:
    WayPointReader(char* fileName) : Reader(fileName){}

protected:
    virtual WayPoint* readPoint()
    {
        double x, y, tMin, tMxB;
        x = y = tMin = tMxB = -10e10;
        (*traceFile) >> x >> y >> tMin >> tMxB;
        //т.к. последн€€ строка (ѕ”—“јя) считываетс€ криво
        if (tMin != -10e10)
            return new WayPoint(x, y, tMin, tMxB);
        else
            return NULL;
    }
};

#endif // READER_H_INCLUDED
