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

#define INPUT_WPT_FORMAT FALSE


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
    double t,x,y, fake;
    if( traceFile->eof() ) return false;
    else {
        fake=t=x=y=-10e10;
        if (INPUT_WPT_FORMAT) {
            (*traceFile)>>x>>y>>t>>fake;
            //cout<< x << "\t" << y << "\t" << t << "\t" << fake <<endl;
        }
        else {
            (*traceFile)>>t>>x>>y;
            //cout<< t << "\t" << x << "\t" << y <<endl;
        }
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
