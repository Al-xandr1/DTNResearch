#ifndef REGROOT_LATP_H
#define REGROOT_LATP_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <vector>
#include <stdlib.h>
#include <windows.h>
#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"
#include "cmessage.h"
#include "DevelopmentHelper.h"
#include "LeviStaticOLD.h"
#include "HotSpotsCollection.h"
#include "RootsCollection.h"
#include "LevyHotSpotsLATP.h"
#include "MobileHost.h"
#include "Messages.h"
#include "RootsPersistenceAndStatistics.h"
#include "GenerationRootsStrategy.h"

using namespace std;

class RegularRootLATP : public LevyHotSpotsLATP
{
  protected:

    RootsCollection* rc;
    RootsPersistenceAndStatistics* rootStatistics;      // ������ �� ������ � ����� ���������������� � �����������
    double rootPersistence;                             // ����������� ���������������, ��� ����������� ����������� ����
    GenerationRootsStrategy* rootGenerator;             // ��������� ����� ��������� (������ ���� �� ���������������, ���� �� �����������

    vector<HotSpotData*>*      firstRoot;               // �������������� ������ (�������) �������� � �����������, ����������� �� ������ *.hts
    vector<unsigned int>*      firstRootSnumber;        // �������������� ������ (�������) � ��������� ������� � ��������� HotSpotsCollection
    vector<int>*               firstRootCounter;        // �������������� ������ (�������) �� ���������� ��������� �������
    vector<int>*               firstRootWptsPerVisit;   // �������������� ������ (�������) �������� ���������� ������� ����� �� ������� ��������

    HotSpotData*               homeHS;                  // ������ ������� ��������, ��� �� ���������

    vector<unsigned int>*      currentRootActualTrack;  // ����������� ������������������ ������� ��� ����������� ��������
    vector<HotSpotData*>*      currentRoot;             // �������������� ������ (�������) �������� � �����������, ����������� �� ������ *.hts
    vector<unsigned int>*      currentRootSnumber;      // �������������� ������ (�������) � ��������� ������� � ��������� HotSpotsCollection
    vector<int>*               currentRootCounter;      // �������������� ������ (�������) �� ���������� ��������� �������
    vector<int>*               currentRootWptsPerVisit; // �������������� ������ (�������) �������� ���������� ������� ����� �� ������� ��������
    vector<int>*               currentRootCounterSAVED; // ����� ������� �������� �������� ���� ����������� currentRootCounter ��� ����������� ���������� ���������� �����������

    unsigned int curRootIndex;                          // ������ ������� ������� � ������� ��������
    unsigned int currentHSWaypointNum;                  // ���������� ���������� ������� ����� � ������� �������
    bool useWaypointCounter;                            // ����, ������������ ����� ������������ ������� ������� ����� �� ������� ��� ���

    bool      isLProbReady;
    double**  LocalProbMatrix;

    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/
    virtual void handleMessage(cMessage * message);
    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual bool findNextHotSpot();
    void setCurRootIndex(unsigned int curRootIndex, bool writeIndexToTrack);
    virtual bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound = false);

  public:
    RegularRootLATP();
    void loadFirstRoot();
    void printFirstRoot();
    void printCurrentRoot();
    void printRoot(
            const char* lable,
            vector<HotSpotData*>* root,
            vector<unsigned int>* rootSnumber,
            vector<int>* rootCounter,
            vector<int>* rootWptsPerVisit);
    void makeLocalProbMatrix(double powA);
    void deleteLocalProbMatrix();
    bool isRootFinished();
    void makeNewRoot();
};

#endif
