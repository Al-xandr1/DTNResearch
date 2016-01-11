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

#define DEF_TR_DIR "./Traces"                  //���������� �� ��������� ��� ��e� ���������� � �������
#define DEF_ROOT_DIR "./Traces/rootfiles"      //���������� �� ��������� ��� "���������"

struct RootShortInfo {
    char RootName[256];
    int length;
    char** hotSpot;

    RootShortInfo( string rootinfo)
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


class RootsCollection
{
public:
    static bool isRootDataReady;
    static vector<RootShortInfo> RootData;

    void readRootsInfo(char* TracesDir, char* rootsfile);
    void print();
};

void RootsCollection::readRootsInfo(char* TracesDir, char* rootsfile)
{
    ifstream rfile(buildFullName(TracesDir, rootsfile));
    while(!rfile.eof()) {
        string rootinfo;
        getline(rfile, rootinfo);
        RootData.push_back(RootShortInfo(rootinfo));
    }
    RootData.pop_back();
    isRootDataReady = true;
}

void RootsCollection::print()
{
    for (unsigned int i=0; i<RootData.size(); i++) RootData[i].print();
}

bool RootsCollection::isRootDataReady = false;
vector<RootShortInfo> RootsCollection::RootData;


#endif /* ROOTSCOLLECTION_H_ */
