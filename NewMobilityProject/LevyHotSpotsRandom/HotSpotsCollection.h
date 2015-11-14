#ifndef HOTSPOTSCOLLECTION_H_
#define HOTSPOTSCOLLECTION_H_

#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include "INETDefs.h"
#include "Coord.h"

#include "DevelopmentHelper.h"

using namespace std;

#define DEF_TR_DIR "./Traces"                  //���������� �� ��������� ��� ��e� ���������� � �������
#define DEF_HS_DIR "./Traces/hotspotfiles"     //���������� �� ��������� ��� "�������"
#define DEF_WP_DIR "./Traces/waypointfiles"    //���������� �� ��������� ��� "������� �����"

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

#endif /* HOTSPOTSCOLLECTION_H_ */