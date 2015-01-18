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

    static double waypointXMin, waypointXMax, waypointYMin, waypointYMax;
};

double WaypointFinder::waypointXMin=10e10; double WaypointFinder::waypointXMax=-10e10;
double WaypointFinder::waypointYMin=10e10; double WaypointFinder::waypointYMax=-10e10;

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
            if(waypointX < waypointXMin) waypointXMin=waypointX;
            if(waypointX > waypointXMax) waypointXMax=waypointX;
            if(waypointY < waypointYMin) waypointYMin=waypointY;
            if(waypointY > waypointYMax) waypointYMax=waypointY;
            (*waypointFile) << waypointX << "\t" << waypointY  << "\t" << tMin << "\t" << tMxB << endl;
            while(xcoord.size()>1) removePoint();
        }
        else while( (removePoint()) && isOutofRange() );
    }
}

//-----------------------------------------------------------------------------------------------------------------------------

char* getWayPointFileName(char* traceFileName)
{
    return strcat(traceFileName, ".wpt");
}

char* buildFullName(char* buffer, char* dir, char* fileName)
{
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

    char traceFileNamePattern[256];
    buildFullName(traceFileNamePattern, traceFilesDir, "*.txt");
    cout << "   traceFileNamePattern: " << traceFileNamePattern << endl << endl;

    HANDLE h = FindFirstFile(traceFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE)
    {
        do
        {
            char inputFileName[256];
            buildFullName(inputFileName, traceFilesDir, f.cFileName);
            cout << "       inputFileName: " << inputFileName << endl;

            char outPutFileName[256];
            buildFullName(outPutFileName, wayPointFilesDir, getWayPointFileName(f.cFileName));
            cout << "       outPutFileName: " << outPutFileName << endl << endl;

            WaypointFinder test(inputFileName, outPutFileName);
            test.findWaypoints();
        }
        while(FindNextFile(h, &f));
    }
    else
    {
        fprintf(stderr, "Directory or files not found\n");
    }

    cout << "Xmin=" << WaypointFinder::waypointXMin << "\t Xmax=" << WaypointFinder::waypointXMax << endl;
    cout << "Ymin=" << WaypointFinder::waypointYMin << "\t Ymax=" << WaypointFinder::waypointYMax << endl;

    char boundsFileName[256];
    ofstream boundFile(buildFullName(boundsFileName, wayPointFilesDir, "bounds.bnd"));
    boundFile << WaypointFinder::waypointXMin << "\t" << WaypointFinder::waypointXMax << endl;
    boundFile << WaypointFinder::waypointYMin << "\t" << WaypointFinder::waypointYMax << endl;
    boundFile.close();

    cout << "End." << endl;
    return 0;
}

int mainForFile(int argc, char** argv)
{
    cout << "Hello world!" << endl;

    char* trFileName;
    char* wpFileName;
    switch(argc) {
        case 1 :  trFileName="test.txt"; wpFileName="test.wpt"; break;
        case 2 :  trFileName=argv[1];    wpFileName="test.wpt"; break;
        case 3 :
        default:  trFileName=argv[1];    wpFileName=argv[2]; break;
    }

    cout << "Start find!" << endl;

    WaypointFinder test(trFileName, wpFileName);
    test.findWaypoints();

    cout << "Xmin=" << WaypointFinder::waypointXMin << "\t Xmax=" << WaypointFinder::waypointXMax << endl;
    cout << "Ymin=" << WaypointFinder::waypointYMin << "\t Ymax=" << WaypointFinder::waypointYMax << endl;

    ofstream boundFile("bounds.bnd");
    boundFile << WaypointFinder::waypointXMin << "\t" << WaypointFinder::waypointXMax << endl;
    boundFile << WaypointFinder::waypointYMin << "\t" << WaypointFinder::waypointYMax << endl;
    boundFile.close();

    return 0;
}

int main(int argc, char** argv)
{
//    return mainForFile(argc, argv);
    return mainForDirectory(argc, argv);
}
