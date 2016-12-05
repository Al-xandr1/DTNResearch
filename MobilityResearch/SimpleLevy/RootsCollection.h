#ifndef ROOTSCOLLECTION_H_
#define ROOTSCOLLECTION_H_

#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "INETDefs.h"
#include "Coord.h"
#include "DevelopmentHelper.h"

using namespace std;

/*
 * Краткая информация о маршруте для конкретного пользователся
 */
struct RootDataShort {
    char RootName[256];     // имя маршрута - соответствует файлу *.rot
    int length;             // длина маршрута
    char** hotSpot;         // последовательность локаций маршрута

    RootDataShort(string rootinfo)
    {
        istringstream info(rootinfo);
        info>>RootName>>length;
        hotSpot = new char*[length];
        for(int i=0; i<length; i++ ){ hotSpot[i]=new char[16]; info>>hotSpot[i]; }
    }

    void print()
    {
        cout << "RootName:" << RootName << "\t" << length << " Hot Spots:" << endl;
        for(int i=0; i<length; i++ ) cout<<hotSpot[i]<<endl;
    }
};


/*
 * todo стоит объединить с HotSpotShortInfo
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
 * Коллекция маршрутов (в виде последовательности локаций) для пользователей
 */
class RootsCollection {
public:
    static bool isRootsCollectionReady;
    static vector<RootDataShort> RootsDataShort;        // набор маршрутов пользователей. Структура соответствует файлу allroots.roo
    static vector<vector<HotSpotRootInfo>*> RootsData;  // набор маршрутов пользователей. Структура - набор последовательностей *.hts

    void readRootsData(char* TracesDir, char* allRootsFile, char* rootsDir, char* filePatter);

    void printRootsDataShort();
    void printRootsData();
};


#endif /* ROOTSCOLLECTION_H_ */
