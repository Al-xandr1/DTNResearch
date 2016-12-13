#ifndef DATA_H_
#define DATA_H_

#include <string.h>
#include <stdlib.h>

using namespace std;

/**
 * Представляет путевую точку.
 */
struct Waypoint{
    double X, Y, Tb, Te; // координаты и время входа в точку и выхода из неё
    char* traceName;     // имя трассы, которой принадлежит данная точка

    Waypoint(double x, double y, double Tb, double Te, char* traceName) {
        this->X = x;
        this->Y = y;
        this->Tb = Tb;
        this->Te = Te;
        this->traceName = traceName;
    }

    void print() {
        cout << "X=" << X << " Y=" << Y << " Tb=" << Tb << " Te=" << Te << " traceName=" << traceName << endl;
    }
};


/*
 * Краткая информация о маршруте для конкретного пользователся (строчка из файла allroots.roo)
 */
struct RootDataShort {
    char RootName[256];     // имя маршрута - соответствует файлу *.rot
    int length;             // длина маршрута
    char** hotSpot;         // последовательность локаций маршрута

    RootDataShort(string rootinfo) {
        istringstream info(rootinfo);
        info>>RootName>>length;
        hotSpot = new char*[length];
        for(int i=0; i<length; i++ ){ hotSpot[i]=new char[16]; info>>hotSpot[i]; }
    }

    void print() {
        cout << "RootName:" << RootName << "\t" << length << " Hot Spots:" << endl;
        for(int i=0; i<length; i++ ) cout<<hotSpot[i]<<endl;
    }
};


/**
 * Структура, представляющая данные о локации и количестве её посещения.
 * Также хранит данные, генерируемые при моделировании
 */
struct HotSpotData {
    char* hotSpotName;                                  // (READ-ONLY) имя локации
    double Xmin, Xmax, Ymin, Ymax, Xcenter, Ycenter;    // (READ-ONLY) границы и центр локации
    double sumTime;                                     // (READ-ONLY) время нахождения в локации
    unsigned int waypointNum;                           // (READ-ONLY) количество путевых точек в локации
    unsigned int counter;                               // (READ-ONLY) количество посещений локации

    double generatedSumTime;           // время нахождения в локации при моделировании
    unsigned int generatedWaypointNum; // количество путевых точек в локации при моделиронии
    vector<Waypoint> waypoints;        // сгенерированные путевые точки

    HotSpotData(char* hotSpotName, double Xmin, double Xmax, double Ymin, double Ymax, double sumTime, unsigned int waypointNum) {
        strcpy(this->hotSpotName = new char[256], hotSpotName);
        this->Xmin = Xmin;
        this->Xmax = Xmax;
        this->Ymin = Ymin;
        this->Ymax = Ymax;
        this->Xcenter = (Xmin + Xmax) / 2;
        this->Ycenter = (Ymin + Ymax) / 2;
        this->sumTime = sumTime;
        this->waypointNum = waypointNum;
        this->counter = 0;
        this->generatedSumTime = 0;
        this->generatedWaypointNum = 0;
     }

     void print() {
         cout << "\t Xmin = " << Xmin << ", Xmax = " << Xmax << endl;
         cout << "\t Ymin = " << Ymin << ", Ymax = " << Ymax << endl;
         cout << "\t Xcenter = " << Xcenter << ", Ycenter = " << Ycenter << endl;
         cout << "\t sumTime = " << sumTime << ", generatedSumTime = " << generatedSumTime << endl;
         cout << "\t waypointNum = " << waypointNum << ", generatedWaypointNum = " << generatedWaypointNum << endl;
         cout << "\t hotSpotName = " << hotSpotName << endl << endl;
     }

     bool isHotSpotEmpty() {
         return Xmin == Xmax || Ymin == Ymax;
     }
};


/**
 * Определяем стуктуру, представляющую локацию, но для использования в маршрутах узлов (для удобства)
 */
typedef HotSpotData HotSpotDataRoot;


#endif /* DATA_H_ */
