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
#include "LeviStatic.h"
#include "HotSpotsCollection.h"
#include "RootsCollection.h"
#include "LevyHotSpotsLATP.h"
#include "MobileHost.h"
#include "Messages.h"

using namespace std;

class RegularRootLATP : public LevyHotSpotsLATP
{
  protected:

    RootsCollection* rc;

    double rootPersistence;

    vector<HotSpotData*>*      firstRoot;               // �������������� ������ (�������) �������� � �����������, ����������� �� ������ *.hts
    vector<unsigned int>*      firstRootSnumber;        // �������������� ������ (�������) � ��������� ������� � ��������� HotSpotsCollection
    vector<int>*               firstRootCounter;        // �������������� ������ (�������) �� ���������� ��������� �������
    vector<int>*               firstRootWptsPerVisit;   // �������������� ������ (�������) �������� ���������� ������� ����� �� ������� ��������

    HotSpotData*               homeHS;                  // ������ ������� ��������, ��� �� ���������

    vector<HotSpotData*>*      currentRoot;             // �������������� ������ (�������) �������� � �����������, ����������� �� ������ *.hts
    vector<unsigned int>*      currentRootSnumber;      // �������������� ������ (�������) � ��������� ������� � ��������� HotSpotsCollection
    vector<int>*               currentRootCounter;      // �������������� ������ (�������) �� ���������� ��������� �������
    vector<int>*               currentRootWptsPerVisit; // �������������� ������ (�������) �������� ���������� ������� ����� �� ������� ��������

    unsigned int curRootIndex;                          // ������ ������� ������� � ������� ��������
    unsigned int currentHSWaypointNum;                  // ���������� ���������� ������� ����� � ������� �������
    bool useWaypointCounter;                            // ����, ������������ ����� ������������ ������� ������� ����� �� ������� ��� ���

    bool      isLProbReady;
    double**  LocalProbMatrix;

    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/
    virtual void handleMessage(cMessage * message);
    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual bool findNextHotSpot();
    virtual void setCurrentHSbordersWith(HotSpotData* hsi);
    virtual bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound = false);

  public:
    RegularRootLATP();
    void loadFirstRoot();
    void printFirstRoot();
    void printCurrentRoot();
    void makeLocalProbMatrix(double powA);
    void deleteLocalProbMatrix();
    bool isRootFinished();
    void makeNewRoot();
};

#endif
