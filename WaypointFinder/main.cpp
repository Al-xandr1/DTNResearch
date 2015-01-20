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
#include "WaypointGenerator.cpp"

using namespace std;

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

    void save(char* fileName) {
        ofstream boundFile(fileName);
        boundFile << XMin << "\t" << XMax << endl;
        boundFile << YMin << "\t" << YMax << endl;
        boundFile.close();
    }

    void print() {
        cout << "Xmin=" << XMin << "\t Xmax=" << XMax << endl;
        cout << "Ymin=" << YMin << "\t Ymax=" << YMax << endl;
    }
};


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

    void changeTraceBounds(double x, double y);
    void changeWayPointBounds(double x, double y);

    Bounds traceBounds;  //границы для одной трассы
    static Bounds totalTraceBounds; //границы для всех трасс

    Bounds wayPointBounds;   //границы для одного файла путевых точек
    static Bounds totalWayPointBounds;  //границы для всех файлоа путевых точек
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
}

bool WaypointFinder::addPoint()
{
    double t,x,y;
    if( traceFile->eof() ) return false;
    else {
        (*traceFile)>>t>>x>>y;
        xcoord.push(x);  sumX+=x;
        ycoord.push(y);  sumY+=y;

        traceBounds.changeBounds(x, y);
        totalTraceBounds.changeBounds(x, y);

        time.push(t);
        if(xcoord.size()==1) tMin=tMxB=tMax=t;
        else  tMxB=tMax; tMax=t;
        return true;
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
    double waypointX, waypointY;
    while( !traceFile->eof() ) {
        while( (addPoint()) && !isOutofRange() );
        if (isStopTimeReached()) {
            waypointX=(sumX-xcoord.back())/(xcoord.size()-1);
            waypointY=(sumY-ycoord.back())/(ycoord.size()-1);

            wayPointBounds.changeBounds(waypointX, waypointY);
            totalWayPointBounds.changeBounds(waypointX, waypointY);

            (*waypointFile) << waypointX << "\t" << waypointY  << "\t" << tMin << "\t" << tMxB << endl;
            while(xcoord.size()>1) removePoint();
        }
        else while( (removePoint()) && isOutofRange() );
    }
}



//-----------------------------------------------------------------------------------------------------------------------------

char* buildWayPointFileName(char* name) {
    char* buffer = new char[256];
    strcpy(buffer, name);
    return strcat(buffer, ".wpt");
}

char* buildBoundsFileName(char* name) {
    char* buffer = new char[256];
    strcpy(buffer, name);
    return strcat(name, ".bnd");
}

char* buildStatisticFileName(char* name) {
    char* buffer = new char[256];
    strcpy(buffer, name);
    return strcat(name, ".stat");
}

char* buildFullName(char* dir, char* fileName) {
    char* buffer = new char[256];
    strcpy(buffer, dir);
    strcat(buffer, "/");
    strcat(buffer, fileName);
}

int mainForDirectory(int argc, char** argv)
{
    cout << "Hello world!" << endl;

    char* traceFilesDir;    //full path name of directory
    char* wayPointFilesDir; //full path name of directory
    switch(argc)
    {
    case 1 :
        traceFilesDir="./tracefiles";
        wayPointFilesDir="./waypointfiles";
        break;
    case 2 :
        traceFilesDir=argv[1];
        wayPointFilesDir="./waypointfiles";
        break;
    case 3 :
    default:
        traceFilesDir=argv[1];
        wayPointFilesDir=argv[2];
        break;
    }

    cout << "Start find..." << endl;

    WIN32_FIND_DATA f;
    if (FindFirstFile(wayPointFilesDir, &f) == INVALID_HANDLE_VALUE)
    {
        if (CreateDirectory(wayPointFilesDir, NULL))
            cout << "create output directory " << endl;
        else
            cout << "error create output directory" << endl;
    }

    char* traceFileNamePattern = buildFullName(traceFilesDir, "*.txt");
    cout << "   traceFileNamePattern: " << traceFileNamePattern << endl << endl;

    HANDLE h = FindFirstFile(traceFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE)
    {
        do
        {
            char* inputFileName = buildFullName(traceFilesDir, f.cFileName);
            cout << "       inputFileName: " << inputFileName << endl;

            char* wpFileName = buildWayPointFileName(f.cFileName);
            char* outPutFileName = buildFullName(wayPointFilesDir, wpFileName);
            cout << "       outPutFileName: " << outPutFileName << endl << endl;

            WaypointFinder test(inputFileName, outPutFileName);
            test.findWaypoints();

            //запись в отдельный файл границы ТРАССЫ
            test.traceBounds.save(buildFullName(traceFilesDir, buildBoundsFileName(f.cFileName)));

            //запись в отдельный файл границы ПУТЕВЫХ ТОЧЕК
            test.wayPointBounds.save(buildFullName(wayPointFilesDir, buildBoundsFileName(wpFileName)));
        }
        while(FindNextFile(h, &f));
    }
    else
    {
        fprintf(stderr, "Directory or files not found\n");
    }

    WaypointFinder::totalWayPointBounds.print();
    WaypointFinder::totalTraceBounds.save(buildFullName(traceFilesDir, "bounds.bnd"));
    WaypointFinder::totalWayPointBounds.save(buildFullName(wayPointFilesDir, "bounds.bnd"));

    cout << "End." << endl;
    return 0;
}

int mainForGenerator(int argc, char** argv){
    //todo доработать удобнее
    WaypointGenerator generator(1000, "NewYork_30sec_038.txt.wpt.bnd");
    generator.analyze("NewYork_30sec_038.txt.wpt", "NewYork_30sec_038.txt.wpt.stat");

    return 0;
}


int main(int argc, char** argv)
{
    return mainForDirectory(argc, argv);
//    return mainForGenerator(argc, argv);
}
