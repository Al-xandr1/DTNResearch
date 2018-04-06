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
    static RootsCollection *instance;                  // ��������� �� singleton ������

    // ��� ��� ��������� ������ ������ � �������� ��������. ������������������ ������ � ��� �����������������
    vector<vector<HotSpotDataRoot> *> *RootsData;      // ����� ��������� �������������. ��������� - ���������� �� ������ *.rot
    vector<RootDataShort>             *RootsDataShort; // ����� ��������� �������������. ��������� ������������� ����� allroots.roo
    vector<RootsCollection *>         *DailyRoot;      // ����� ��������� ��������� ��� ������� ��� (�� �������� _day=***_ � ����� rootfiles)

    /* ����� ��������������� ��������� �������������. ����� ������ - ���������� ��� ������ � ����� *.rot
     * ��������� ������:
     *      vector<HotSpotDataRoot>*  - ���������� ���������� ������� ����������� ������������ � ���������� ����;
     *      vector<vector<HotSpotDataRoot>*>*  - ����� ���������, ��� ������ - ��� ����� ���. �.�. ��� �������� ����������� ���� �� ��� ���;
     *      vector<vector<vector<HotSpotDataRoot>*>*>*  - ����� ��������� ���� ���� �� �����. ������ �������� ������� ����� ����.
     *
     * �.�. ������ �� ������� � ������� - ����� ����
     *      ������ �� �������� � ������� - ����� ���
     */
    vector<vector<vector<HotSpotDataRoot*> *> *> *generatedTheoryRootsData; // ��� �������� ���������������, �� �� ���������� ���������
    vector<vector<vector<HotSpotDataRoot*> *> *> *generatedActualRootsData; // ��� �������� ���������� ���������� ���������

    RootsCollection(const char* TracesDir, const char* allRootsFile, const char* rootsDir, const char* filePattern) {
        this->RootsDataShort = NULL;
        this->RootsData = NULL;
        this->DailyRoot = NULL;
        this->generatedTheoryRootsData = NULL;
        this->generatedActualRootsData = NULL;
        this->readRootsData(TracesDir, allRootsFile, rootsDir, filePattern);
        this->readDailyRoots(TracesDir, allRootsFile, rootsDir, filePattern);
        this->printRootsData();
        this->printRootsDataShort();
        if (DailyRoot) {
            cout << "RootsCollection: dailyRoots loaded, count days = " << DailyRoot->size() << endl;
        }
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
        if (generatedTheoryRootsData) {
            for (unsigned int i=0; i<generatedTheoryRootsData->size(); i++) {
                vector<vector<HotSpotDataRoot*>*>* rootsPerNode = generatedTheoryRootsData->at(i);
                for (unsigned int j=0; j<rootsPerNode->size(); j++) {
                    vector<HotSpotDataRoot*>* dailyRoot = rootsPerNode->at(j);
                    for (unsigned int k=0; k<dailyRoot->size(); k++) {
                        delete dailyRoot->at(k);
                    }
                    delete dailyRoot;
                }
                delete rootsPerNode;
            }
            delete generatedTheoryRootsData;
            generatedTheoryRootsData = NULL;
        }
        if (generatedActualRootsData) {
            for (unsigned int i=0; i<generatedActualRootsData->size(); i++) {
                vector<vector<HotSpotDataRoot*>*>* rootsPerNode = generatedActualRootsData->at(i);
                for (unsigned int j=0; j<rootsPerNode->size(); j++) {
                    vector<HotSpotDataRoot*>* dailyRoot = rootsPerNode->at(j);
                    for (unsigned int k=0; k<dailyRoot->size(); k++) {
                        delete dailyRoot->at(k);
                    }
                    delete dailyRoot;
                }
                delete rootsPerNode;
            }
            delete generatedActualRootsData;
            generatedActualRootsData = NULL;
        }
    }

    void readRootsData(const char* TracesDir, const char* allRootsFile, const char* rootsDir, const char* filePattern);

    void readDailyRoots(const char* fakeTracesDir, const char* fakeAllRootsFile, const char* rootsDir, const char* filePattern);

    void innerSaveRoots(const char *logPrefix, const char *rtDir, vector<vector<vector<HotSpotDataRoot*> *> *> *generatedRootsData);

public:
    /**
    * ��������� �������� ���������������������� ������� ��� ������
    */
    static RootsCollection *getInstance();

    vector<vector<HotSpotDataRoot> *> *getRootsData() { return RootsData; }

    vector<RootDataShort> *getRootsDataShort() { return RootsDataShort; }

    vector<RootsCollection *> *getDailyRoot() { return DailyRoot; };

    RootDataShort *getRootDataShortByNodeId(int nodeId) {
        ASSERT(nodeId >= 0 && nodeId < RootsDataShort->size());
        RootDataShort *rootDataShort = &(RootsDataShort->at(nodeId));
        // ��������� ������������ ��������� id (� �������� ����� - ���� �� ��� ����) � nodeId
        ASSERT(checkFileIdAndNodeId(rootDataShort->RootName, nodeId));
        return rootDataShort;
    }

    vector<HotSpotDataRoot> *getRootDataByNodeId(int nodeId) {
        ASSERT(nodeId >= 0 && nodeId < RootsData->size());
        // �������� ��� �������� ������������ ��������� id �� �������� ����� � nodeId
        getRootDataShortByNodeId(nodeId);
        return RootsData->at(nodeId);
    }

    vector<vector<vector<HotSpotDataRoot*>*>*> *getGeneratedTheoryRootsData() {return generatedTheoryRootsData;}

    vector<vector<vector<HotSpotDataRoot*>*>*> *getGeneratedActualRootsData() {return generatedActualRootsData;}

    /**
     * ��������� ��������� ������ � �������� ���������������, �� �� ����������� ��� ���������� ������������ � ���������� ��������� ����.
     */
    void collectTheoryRoot(vector<HotSpotData*>* root,
                           vector<unsigned int>* rootSnumber,
                           vector<int>* rootCounter,
                           unsigned int nodeId,
                           unsigned int day);

    /**
     * ��������� ��������� ������ � �������� ���������� ����������� ��� ���������� ������������ � ���������� ��������� ����.
     */
    void collectActualRoot(vector<HotSpotData*>* root,
                           vector<unsigned int>* rootSnumber,
                           vector<int>* rootCounter,
                           vector<unsigned int>* rootTrack,
                           vector<double>* rootTrackSumTime,
                           vector<int>* rootTrackWaypointNum,
                           unsigned int nodeId,
                           unsigned int day);

    void printRootsDataShort();

    void printRootsData();

    void saveRoots(const char *thRtDir, const char *acRtDir);

private:
    /**
     * ��������� ��������� ������ ��� ���������� ������������ � ���������� ��������� ����.
     * generatedRootsData - ��� ���������, � ������� �������������� ��������� ����������.
     * �� ������� ������ �������� ��� ���������: ��� ���������� ���������� ��������� � ��� ��������������, �� �� ����������.
     *
     * �� �������� �� RegularRootLATP.h:
     *     vector<HotSpotData*>*      currentRoot;         - �������������� ������ (�������) �������� � �����������, ����������� �� ������ *.hts
     *     vector<unsigned int>*      currentRootSnumber;  - �������������� ������ (�������) � ��������� ������� � ��������� HotSpotsCollection
     *     vector<int>*               currentRootCounter;  - �������������� ������ (�������) �� ���������� ��������� �������
     *     vector<unsigned int>*      rootTrack            - �������������� ������ ����������� ������������������ ��������� ������� � ��������
     *     vector<double>*            rootTrackSumTime     - �������������� ������ ����������� �����, ���������� � ������������ �������� ��������
     *     vector<int>*               rootTrackWaypointNum - �������������� ������ ������������ ���������� ������� �����, ���������� � ������������ �������� ��������
     *     unsigned int               nodeId               - ID ����, ��� �������� ����������� �������
     *     unsigned int               day                  - ����� ���, ��� �������� ����������� �������.
     *                                                       ��� ���������� � 1, �� ��������� generatedRootsData ��� �������� ������� � 0.
     */
    void static collectRoot(vector<vector<vector<HotSpotDataRoot*> *> *> *generatedRootsData,
                            vector<HotSpotData*>* root,
                            vector<unsigned int>* rootSnumber,
                            vector<int>* rootCounter,
                            vector<unsigned int>* rootTrack,
                            vector<double>* rootTrackSumTime,
                            vector<int>* rootTrackWaypointNum,
                            unsigned int nodeId,
                            unsigned int day);
};

#endif /* ROOTSCOLLECTION_H_ */
