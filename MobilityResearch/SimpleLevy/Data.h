#ifndef DATA_H_
#define DATA_H_

#include <string.h>
#include <stdlib.h>
#include "DevelopmentHelper.h"

using namespace std;

/**
 * ������������ ����� ��������� ��������.
 */
struct TracePoint {
    double T, X, Y;     // ����� � ���������� ��������������� �����
    char* traceName;    // ��� ������, ������� ����������� ������ �����

    TracePoint(double t, double x, double y, char* traceName) {
        this->T = t;
        this->X = x;
        this->Y = y;
        this->traceName = traceName;
    }

    void print() {
        cout << "T=" << T << " X=" << X << " Y=" << Y << " traceName=" << traceName << endl;
    }
};


/**
 * ������������ ������� �����.
 */
struct Waypoint {
    double X, Y, Tb, Te; // ���������� � ����� ����� � ����� � ������ �� ��
    char* traceName;     // ��� ������, ������� ����������� ������ �����

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
 * ������� ���������� � �������� ��� ����������� �������������
 * ������������ ������� �� ����� allroots.roo ���� ����������� ���������������,
 * ������� ����� ���� ������ � ����� ����� (������������ � ��������������� ������� ���������)
 */
struct RootDataShort {
    char RootName[256];     // ��� �������� - ������������� ����� *.rot
    int length;             // ����� ��������
    char** hotSpot;         // ������������������ ������� ��������

    double* persistence;    /* ����������� ���������������, ��������������� ����������� ��������
                             * ������ �� ����� ����� ��������, ���� ����
                             * ������������ ���������, ��� �������� ��������,
                             * ����� ����. �� ������ �� �������� �������� ������������� �����
                             */

    RootDataShort(string rootinfo) {
        istringstream info(rootinfo);
        info>>RootName>>length;
        hotSpot = new char*[length];
        for(int i=0; i<length; i++ ){ hotSpot[i]=new char[16]; info>>hotSpot[i]; }
        persistence = extractDoubleParameter(RootName, PERSISTENCE);
        //���� ����������� ��������������� ����, �� �� ������ ���� ������ ����
        if (persistence != NULL) ASSERT(*(persistence) > 0);
    }

    void print() {
        cout << "RootName:" << RootName << "\t" << length << " Hot Spots:" << endl;
        for(int i=0; i<length; i++ ) cout << hotSpot[i] << endl;
        if (persistence != NULL) cout << "persistence = " << (*(persistence)) << endl;
    }
};


/**
 * ���������, �������������� ������ � ������� � ���������� � ���������.
 * ����� ������ ������, ������������ ��� �������������
 */
struct HotSpotData {
    char* hotSpotName;                                  // (READ-ONLY) ��� �������
    double Xmin, Xmax, Ymin, Ymax, Xcenter, Ycenter;    // (READ-ONLY) ������� � ����� �������
    double sumTime;                                     // (READ-ONLY) ����� ���������� � �������
    int waypointNum;                                    // (READ-ONLY) ���������� ������� ����� � �������
    int counter;                                        // (READ-ONLY) ���������� ��������� �������

    double generatedSumTime;           // ����� ���������� � ������� ��� �������������
    unsigned int generatedWaypointNum; // ���������� ������� ����� � ������� ��� �����������
    vector<Waypoint> waypoints;        // ��������������� ������� �����

    HotSpotData() :
        hotSpotName(NULL),
        Xmin(0), Xmax(0), Ymin(0), Ymax(0), Xcenter(0), Ycenter(0),
        sumTime(0),
        waypointNum(0),
        counter(0),
        generatedSumTime(0),
        generatedWaypointNum(0) {}

    HotSpotData(char* hotSpotName, double Xmin, double Xmax, double Ymin, double Ymax, double sumTime, int waypointNum) {
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

    HotSpotData(const HotSpotData& anotherData) :
        Xmin(anotherData.Xmin), Xmax(anotherData.Xmax),
        Ymin(anotherData.Ymin), Ymax(anotherData.Ymax),
        Xcenter(anotherData.Xcenter), Ycenter(anotherData.Ycenter),
        sumTime(anotherData.sumTime),
        waypointNum(anotherData.waypointNum),
        counter(anotherData.counter),
        generatedSumTime(anotherData.generatedSumTime),
        generatedWaypointNum(anotherData.generatedWaypointNum)
    {
        this->hotSpotName = new char[256];
        this->hotSpotName = strcpy(this->hotSpotName, anotherData.hotSpotName);
        this->waypoints = anotherData.waypoints;
    }

    ~HotSpotData() {
        if (hotSpotName) {
            delete[] hotSpotName;
            hotSpotName = NULL;
        }
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
 * ���������� ��������, �������������� �������, �� ��� ������������� � ��������� ����� (��� ��������)
 */
typedef HotSpotData HotSpotDataRoot;


#endif /* DATA_H_ */
