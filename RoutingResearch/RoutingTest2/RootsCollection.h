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
 * ���������� � �������� ��� ����������� �������������
 * todo ����� ���������� � HotSpotRootInfo
 */
struct RootShortInfo {
    char RootName[256];     // ��� �������� - ������������� ����� *.rot
    int length;             // ����� ��������
    char** hotSpot;         // ������������������ ������� ��������

    RootShortInfo(string rootinfo)
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
 * todo ����� ���������� � RootCollection
 */
class RootsCollection
{
public:
    static bool isRootDataReady;
    static vector<RootShortInfo> RootData;

    void readRootsInfo(char* TracesDir, char* rootsfile);
    void print();
};


#endif /* ROOTSCOLLECTION_H_ */
