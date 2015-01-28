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
#include <windows.h>
#include "Bounds.h"

#ifndef WAYPOINTFINDER_INCLUDED
#define WAYPOINTFINDER_INCLUDED

using namespace std;


class WaypointFinder {
protected:
    double R2, T;      // main waypoint parameters (by default R2=5m*5m , T=30sec)

    ifstream* traceFile;
    ofstream* waypointFile;

    queue<double> xcoord;
    queue<double> ycoord;
    queue<double> time;
    double sumX, sumY, tMin, tMxB, tMax;

public:
    WaypointFinder(char* trFileName, char* wpFileName, double range, double stopTime );
    ~WaypointFinder();
    bool addPoint();
    bool removePoint();
    bool isOutofRange();
    bool isStopTimeReached();
    void findWaypoints();

    Bounds traceBounds;  //границы дл€ одной трассы
    static Bounds totalTraceBounds; //границы дл€ всех трасс

    Bounds wayPointBounds;   //границы дл€ одного файла путевых точек
    static Bounds totalWayPointBounds;  //границы дл€ всех файлоа путевых точек
};

Bounds WaypointFinder::totalTraceBounds;
Bounds WaypointFinder::totalWayPointBounds;

WaypointFinder::WaypointFinder(char* trFileName, char* wpFileName, double range=5, double stopTime=30 )
{
    R2=range*range;  T=stopTime;
    sumX=sumY=tMin=tMxB=tMax=0;

    traceFile = new ifstream(trFileName);
    if(traceFile == NULL){
        cout<<"WaypointFinder constructor: Input trace file "<<trFileName<<" is not found."<<endl;
        exit(1);
    }

    waypointFile = new ofstream(wpFileName);
    if(waypointFile == NULL){
        cout<<"WaypointFinder constructor: Output file "<<wpFileName<<" opening failed."<<endl;
        exit(2);
    }
}

WaypointFinder::~WaypointFinder()
{
    traceFile->close();
    waypointFile->close();
    delete traceFile;
    delete waypointFile;
}

bool WaypointFinder::addPoint()
{
    double t,x,y;
    if( traceFile->eof() ) return false;
    else {
        t=x=y=-10e10;
        (*traceFile)>>t>>x>>y;
        //т.к. последн€€ строка (ѕ”—“јя) считываетс€ криво
        if (t != -10e10) {
            xcoord.push(x);  sumX+=x;
            ycoord.push(y);  sumY+=y;

            traceBounds.changeBounds(x, y);
            totalTraceBounds.changeBounds(x, y);

            time.push(t);
            if(xcoord.size()==1) tMin=tMxB=tMax=t;
            else  tMxB=tMax; tMax=t;
            return true;
        } else
            return false;
    }
}

bool WaypointFinder::removePoint()
{
    if(xcoord.size()>1) {
        sumX-=xcoord.front();  xcoord.pop();
        sumY-=ycoord.front();  ycoord.pop();
        time.pop();   tMin=time.front();
        if(xcoord.size()==1) tMxB=tMax;
        return true;
    }
    else return false;
}

bool WaypointFinder::isOutofRange()
{
    double xwp=0, ywp=0, dx, dy;
    queue<double> xc(xcoord);
    queue<double> yc(ycoord);
    bool outofRange = false;

    if(xcoord.size()>0) {
        xwp=sumX/xcoord.size();
        ywp=sumY/ycoord.size();
        dx=xc.back()-xwp;  dy=yc.back()-ywp;
        if( dx*dx+dy*dy > R2 ) return true;
    }
    while( xc.size()>1 ) {
        dx=xc.front()-xwp; dy=yc.front()-ywp;
        if( dx*dx+dy*dy <= R2 ) { xc.pop(); yc.pop(); }
        else { outofRange = true; break; }
    }
    return outofRange;
}

bool WaypointFinder::isStopTimeReached()
{
    return tMxB-tMin > T;
}

void WaypointFinder::findWaypoints()
{
    bool firstRow = true;
    double waypointX, waypointY;
    while( !traceFile->eof() ) {
        while( (addPoint()) && !isOutofRange() );
        if (isStopTimeReached()) {
            waypointX=(sumX-xcoord.back())/(xcoord.size()-1);
            waypointY=(sumY-ycoord.back())/(ycoord.size()-1);

            wayPointBounds.changeBounds(waypointX, waypointY);
            totalWayPointBounds.changeBounds(waypointX, waypointY);

            if (firstRow) firstRow = false;
            else (*waypointFile) << endl;

            (*waypointFile) << waypointX << "\t" << waypointY  << "\t" << tMin << "\t" << tMxB;
            while(xcoord.size()>1) removePoint();
        }
        else while( (removePoint()) && isOutofRange() );
    }
}

#endif // WAYPOINTFINDER_INCLUDED


//-------------------------------------- Old non-complete version --------------------------------------------------------------------

//
//class AnilizeQueue {
//private:
//    std::queue<TracePoint>* queue;
//    TracePoint* summOfPoints; //store sum of coordinates for average value AND time range between the latest point and the earliest
//    TracePoint* averagePoint;
//
//    void updateTimeRange() {
//        if (queue->size() > 0) {
//            summOfPoints->t = queue->back().t - queue->front().t;
//        } else {
//            summOfPoints->t = 0;
//        }
//    }
//
//public:
//    AnilizeQueue() {
//        queue = new std::queue<TracePoint>();
//        summOfPoints = new TracePoint(0, 0, 0);
//        averagePoint = NULL;
//    }
//
//    ~AnilizeQueue() {
//        delete queue;
//        delete summOfPoints;
//    }
//
//    //Adds an element to the back of the queue.
//    void push(TracePoint* point) {
//        queue->push(point);
//
//        summOfPoints->x += point->x;
//        summOfPoints->y += point->y;
//        updateTimeRange();
//    }
//
//    //Retrieves and Removes an element from the front of the queue.
//    TracePoint* pop() {
//        TracePoint& point = queue->front();
//        queue->pop();
//
//        summOfPoints->x -= point.x;
//        summOfPoints->y -= point.y;
//        updateTimeRange();
//        return &point;
//    }
//
//    TracePoint getAveragePoint() {
//        double averageX = summOfPoints->x / queue->size();
//        double averageY = summOfPoints->y / queue->size();
//        if (averagePoint)
//            delete averagePoint;
//
//        return (averagePoint = new TracePoint(summOfPoints->t, averageX, averageY));
//    }
//
//    int size() {
//        return queue->size();
//    }
//};
//
//// check TracePoint for enclosing in circle with specified center and radius
//bool isPointInCircle(double centerX, double centerY, double radius, TracePoint* point) {
//    return (pow(point->x - centerX, 2) + pow(point->y - centerY, 2)) <= radius;
//}
//
//#define RADIUS 5        //meters
//#define TIMEOUT 30      //seconds
//
//int test() {
//    cout << "Start!" << endl;
//
//    AnilizeQueue* queue = new AnilizeQueue();
//
//    TracePointReader* reader = new TracePointReader("NewYork_30sec_001.txt");
//    int row = 1;
//    while (reader->hasNext()) {
//        TracePoint* point = reader->next();
//        queue->push(point);
//    }
//    delete reader;
//    cout << endl;
//    //todo в конце считываетс€ 0 0 0
//
//    cout << "   Average: x=" << queue->getAveragePoint().x << ", y="
//            << queue->getAveragePoint().y << ", timeRange="
//            << queue->getAveragePoint().t << endl;
//
//    int size = queue->size();
//    for (int i = 0; i < size; i++) {
//        TracePoint* point = queue->pop();
//        cout << row++ << "  " << point->t << "  " << point->x << "  "
//                << point->y << endl;
//    }
//
//    cout << "   Average: x=" << queue->getAveragePoint().x << ", y="
//            << queue->getAveragePoint().y << ", timeRange="
//            << queue->getAveragePoint().t << endl;
//
//    cout << "End.";
//    return 0;
//}
