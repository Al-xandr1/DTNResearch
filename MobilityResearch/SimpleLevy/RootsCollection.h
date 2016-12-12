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
#include "Data.h"
#include "Coord.h"
#include "DevelopmentHelper.h"

using namespace std;

/*
 * ��������� ��������� (� ���� ������������������ �������) ��� �������������. Singleton ������
 */
class RootsCollection {
private:
    static RootsCollection* instance;             // ��������� �� singleton ������

    // ��� ��� ��������� ������ ������ � �������� ��������. ������������������ ������ � ��� �����������������
    //todo � ������ RootsDataShort �����???
    vector<RootDataShort>* RootsDataShort;        // ����� ��������� �������������. ��������� ������������� ����� allroots.roo
    vector<vector<HotSpotDataRoot>*>* RootsData;  // ����� ��������� �������������. ��������� - ���������� �� ������ *.rot

    RootsCollection() {
        this->RootsDataShort = NULL;
        this->RootsData = NULL;
        this->readRootsData(DEF_TR_DIR, ALLROOTS_FILE, DEF_RT_DIR, ROOT_PATTERT);
        this->printRootsDataShort();
        this->printRootsData();
    }

    ~RootsCollection() {
        if (RootsDataShort) {
            delete RootsDataShort;
            RootsDataShort = NULL;
        }
        if (RootsData) {
            for (unsigned int i=0; i<RootsData->size(); i++) {
                delete RootsData->at(i);
                delete RootsData;
            }
            RootsData = NULL;
        }
    }

    void readRootsData(char* TracesDir, char* allRootsFile, char* rootsDir, char* filePatter);

public:
    /**
    * ��������� �������� ���������������������� ������� ��� ������
    */
    static RootsCollection* getInstance();

    vector<RootDataShort>* getRootsDataShort()        {return RootsDataShort;}
    vector<vector<HotSpotDataRoot>*>* getRootsData()  {return RootsData;}

    RootDataShort*           getRootDataShortByNodeId(int nodeId) {return &(RootsDataShort->at(nodeId));}
    vector<HotSpotDataRoot>* getRootDataByNodeId(int nodeId)      {return RootsData->at(nodeId);}

    void printRootsDataShort();
    void printRootsData();
};


#endif /* ROOTSCOLLECTION_H_ */
