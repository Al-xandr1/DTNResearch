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

#define DEF_TR_DIR "./Traces"                  //���������� �� ��������� ��� ��e� ���������� � �������
#define DEF_HS_DIR "./Traces/hotspotfiles"     //���������� �� ��������� ��� "�������"
#define DEF_WP_DIR "./Traces/waypointfiles"    //���������� �� ��������� ��� "������� �����"
#define DEF_RT_DIR "./Traces/rootfiles"

struct HotSpotShortInfo {
     double Xmin, Xmax, Ymin, Ymax, Xcenter, Ycenter;
     double sumTime;
     unsigned int waypointNum;
     unsigned int counter;
     char* hotSpotName;

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
     }

     void print()
     {
         cout << " Xmin=" << Xmin << " Xmax=" << Xmax << "\n" << " Ymin=" << Ymin << " Ymax=" << Ymax << endl << endl;
     }
};

class HotSpotsCollection
{
public:
    static bool isHSDataReady;
    static vector<HotSpotShortInfo> HSData;

    void readHotSpotsInfo(char* TracesDir, double& minX, double& maxX, double& minY, double& maxY);
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


class RootCollection {
public:
        static bool isRootDataReady;
        static vector<vector<HotSpotRootInfo>*> RootData;

        void readRootInfo(char* RootDir);
        void prtintRootInfo();
};


#endif /* HOTSPOTSCOLLECTION_H_ */