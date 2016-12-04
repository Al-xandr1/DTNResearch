#ifndef HOTSPOTSCOLLECTION_H_
#define HOTSPOTSCOLLECTION_H_

#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <windows.h>

#include "INETDefs.h"
#include "Coord.h"

#include "DevelopmentHelper.h"

using namespace std;


struct Waypoint{
    double X, Y, Tb, Te;
    char* traceName;

    Waypoint(double x, double y, double Tb, double Te, char* traceName) {
        this->X = x;
        this->Y = y;
        this->Tb = Tb;
        this->Te = Te;
        this->traceName = traceName;
    }
};

struct HotSpotShortInfo {
     double Xmin, Xmax, Ymin, Ymax, Xcenter, Ycenter;
     double sumTime;            // (READ-ONLY) время нахождения в локации
     unsigned int waypointNum;  // (READ-ONLY) количество путевых точек в локации
     unsigned int counter;      // (READ-ONLY) количество посещений локации
     char* hotSpotName;         // (READ-ONLY) имя локации

     double generatedSumTime;           // время нахождения в локации при моделировании
     unsigned int generatedWaypointNum; // количество путевых точек в локации при моделиронии
     vector<Waypoint> waypoints;        // сгенерированные путевые точки

     HotSpotShortInfo(char* hotSpotName, double Xmin, double Xmax, double Ymin, double Ymax, double sumTime, unsigned int waypointNum, unsigned int counter=0 )
     {
         strcpy(this->hotSpotName = new char[256], hotSpotName);
         this->Xmin = Xmin;
         this->Xmax = Xmax;
         this->Ymin = Ymin;
         this->Ymax = Ymax;
         this->Xcenter = (Xmin + Xmax) / 2;
         this->Ycenter = (Ymin + Ymax) / 2;
         this->sumTime = sumTime;
         this->waypointNum = waypointNum;
         this->generatedSumTime = 0;
         this->generatedWaypointNum = 0;
     }

     void print()
     {
         cout << "\t Xmin = " << Xmin << ", Xmax = " << Xmax << endl;
         cout << "\t Ymin = " << Ymin << ", Ymax = " << Ymax << endl;
         cout << "\t Xcenter = " << Xcenter << ", Ycenter = " << Ycenter << endl;
         cout << "\t sumTime = " << sumTime << ", generatedSumTime = " << generatedSumTime << endl;
         cout << "\t waypointNum = " << waypointNum << ", generatedWaypointNum = " << generatedWaypointNum << endl;
         cout << "\t hotSpotName = " << hotSpotName << endl << endl;
     }
};

class HotSpotsCollection
{
public:
    static bool isHSDataReady;
    static vector<HotSpotShortInfo> HSData;

    void readHotSpotsInfo(char* TracesDir, double& minX, double& maxX, double& minY, double& maxY);
    HotSpotShortInfo* findHotSpotbyName(char*, int&);
    void print();
    static HotSpotShortInfo* randomRemove(vector<HotSpotShortInfo*>* hotSpots, int& HotSpotNum);
};


class HSDistanceMatrix
{
public:
    static bool isMatrixReady;
    static vector<double>*  DistanceMatrix;

    static bool isProbabilityReady;
    static double**  ProbabilityMatrix;

    void makeDistanceMatrix();
    void makeProbabilityMatrix(double powA);
    double getDistance(unsigned int i, unsigned int j);
};


// ----------------------------------- for SLAW ----------------------------------------------

/*
 * todo стоит объединить с RootShortInfo
 */
struct HotSpotRootInfo {
     char* hotSpotName;
     double Xmin, Xmax, Ymin, Ymax;
     double sumTime;
     unsigned int waypointNum;

     HotSpotRootInfo(char* hotSpotName=NULL, double Xmin=0, double Xmax=0, double Ymin=0, double Ymax=0, double sumTime=0, unsigned int waypointNum=0)
     {
         if(hotSpotName!=NULL) strcpy(this->hotSpotName = new char[256], hotSpotName);
         else this->hotSpotName=NULL;
         this->Xmin=Xmin;
         this->Xmax=Xmax;
         this->Ymin=Ymin;
         this->Ymax=Ymax;
         this->sumTime = sumTime;
         this->waypointNum = waypointNum;
     }

     void printHotSpotRootInfo()
     {
         cout << this->hotSpotName <<" "<<this->Xmin<<" "<<this->Xmax<<" "
              << this->Ymin<<" "<<this->Ymax<<" "<<this->sumTime<<" "<<waypointNum<<endl;
     }
};

/*
 * todo стоит объединить с RootsCollection
 */
class RootCollection {
public:
        static bool isRootDataReady;
        static vector<vector<HotSpotRootInfo>*> RootData;

        void readRootInfo(char* RootDir);
        void prtintRootInfo();
};


#endif /* HOTSPOTSCOLLECTION_H_ */
