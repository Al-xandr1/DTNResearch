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
    static RootsCollection *instance;              // ��������� �� singleton ������

    // ��� ��� ��������� ������ ������ � �������� ��������. ������������������ ������ � ��� �����������������
    //todo � ������ RootsDataShort �����???
    vector<RootDataShort> *RootsDataShort;         // ����� ��������� �������������. ��������� ������������� ����� allroots.roo
    vector<vector<HotSpotDataRoot> *> *RootsData;  // ����� ��������� �������������. ��������� - ���������� �� ������ *.rot

    /* ����� ��������������� ��������� �������������. ����� ������ - ���������� ��� ������ � ����� *.rot
     * ��������� ������:
     *      vector<HotSpotDataRoot>*  - ���������� ���������� ������� ����������� ������������ � ���������� ����;
     *      vector<vector<HotSpotDataRoot>*>*  - ����� ���������, ��� ������ - ��� ����� ���. �.�. ��� �������� ����������� ���� �� ��� ���;
     *      vector<vector<vector<HotSpotDataRoot>*>*>*  - ����� ��������� ���� ���� �� �����. ������ �������� ������� ����� ����.
     *
     * �.�. ������ �� ������� � ������� - ����� ����
     *      ������ �� �������� � ������� - ����� ���
     */
    vector<vector<vector<HotSpotDataRoot*> *> *> *generatedRootsData;

    RootsCollection() {
        this->RootsDataShort = NULL;
        this->RootsData = NULL;
        this->generatedRootsData = NULL;
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
            for (unsigned int i = 0; i < RootsData->size(); i++) {
                delete RootsData->at(i);
            }
            delete RootsData;
            RootsData = NULL;
        }
        if (generatedRootsData) {
            for (unsigned int i=0; i<generatedRootsData->size(); i++) {
                vector<vector<HotSpotDataRoot*>*>* rootsPerNode = generatedRootsData->at(i);
                for (unsigned int j=0; j<rootsPerNode->size(); j++) {
                    vector<HotSpotDataRoot*>* dailyRoot = rootsPerNode->at(j);
                    for (unsigned int k=0; k<dailyRoot->size(); k++) {
                        delete dailyRoot->at(k);
                    }
                    delete dailyRoot;
                }
                delete rootsPerNode;
            }
            delete generatedRootsData;
            generatedRootsData = NULL;
        }
    }

    void readRootsData(char *TracesDir, char *allRootsFile, char *rootsDir, char *filePatter);

public:
    /**
    * ��������� �������� ���������������������� ������� ��� ������
    */
    static RootsCollection *getInstance();

    vector<RootDataShort> *getRootsDataShort() { return RootsDataShort; }

    vector<vector<HotSpotDataRoot> *> *getRootsData() { return RootsData; }

    RootDataShort *getRootDataShortByNodeId(int nodeId) { return &(RootsDataShort->at(nodeId)); }

    vector<HotSpotDataRoot> *getRootDataByNodeId(int nodeId) { return RootsData->at(nodeId); }

    /**
     * ��������� ��������� ������ ��� ���������� ������������ � ���������� ��������� ����.
     * �� �������� �� RegularRootLATP.h:
     *     vector<HotSpotData*>*      currentRoot;         - �������������� ������ (�������) �������� � �����������, ����������� �� ������ *.hts
     *     vector<unsigned int>*      currentRootSnumber;  - �������������� ������ (�������) � ��������� ������� � ��������� HotSpotsCollection
     *     vector<int>*               currentRootCounter;  - �������������� ������ (�������) �� ���������� ��������� �������
     *     unsigned int               nodeId               - ID ����, ��� �������� ����������� �������
     *     unsigned int               day                  - ����� ���, ��� �������� ����������� �������.
     *                                                       ��� ���������� � 1, �� ��������� generatedRootsData ��� �������� ������� � 0.
     */
    void collectRoot(vector<HotSpotData*>* root, vector<unsigned int>* rootSnumber, vector<int>* rootCounter, unsigned int nodeId, unsigned int day);

    void printRootsDataShort();

    void printRootsData();

    void saveGeneratedRootsData();
};

#endif /* ROOTSCOLLECTION_H_ */
