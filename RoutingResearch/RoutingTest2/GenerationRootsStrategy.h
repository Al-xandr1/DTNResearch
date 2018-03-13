#ifndef GENERATIONROOTSSTRATEGY_H_
#define GENERATIONROOTSSTRATEGY_H_

#include <vector>
#include "Data.h"
#include "HotSpotsCollection.h"
#include "DevelopmentHelper.h"
#include "RootsPersistenceAndStatistics.h"


using namespace std;

/**
 * ����� ����� ��� ��������� ��������� ���������.
 */
class GenerationRootsStrategy {
protected:
    /**
     * ��������� �� ��������� ������� (������ ��� ������) (������������� �� �����������)
     */
    HotSpotsCollection* hsc;

    /**
     * ������������ ������������ �������� �������, ��� ������������ (� ����������� �� ���������)
     */
    bool useFixedHomeLocation;

public:
    GenerationRootsStrategy(HotSpotsCollection* hsc, bool useFixedHomeLocation) {;
        this->hsc = hsc;
        this->useFixedHomeLocation = useFixedHomeLocation;
    }

    virtual ~GenerationRootsStrategy() {
        this->hsc = NULL;
    };

    /**
     * �������� ���������� �������� � �������� ����� ��������� RegularRootLATP.
     */
    virtual bool generateNewRoot(
            vector<HotSpotData*>* firstRoot, vector<unsigned int>* firstRootSnumber, vector<int>* firstRootCounter, vector<int>* firstRootWptsPerVisit,
            vector<HotSpotData*>*& currentRoot, vector<unsigned int>*& currentRootSnumber, vector<int>*& currentRootCounter, vector<int>*& currentRootWptsPerVisit) {
        exit(-200);
        return false;
    };
};




/**
 * ��������� ��������� ��������� �� ������ ������������ ���������������.
 */
class GenerationRootsByPersistenceStrategy : public GenerationRootsStrategy {
private:
    /**
     * ������������� ����������� ��������������� (������������� �� �����������)
     */
    double rootPersistence;

public:
    GenerationRootsByPersistenceStrategy(HotSpotsCollection* hsc, double rootPersistence, bool useFixedHomeLocation)
            : GenerationRootsStrategy(hsc, useFixedHomeLocation) {
        this->rootPersistence = rootPersistence;
    };

    ~GenerationRootsByPersistenceStrategy() {
    };

    /**
     * �������� ���������� �������� � �������� ����� ��������� RegularRootLATP.
     */
    virtual bool generateNewRoot(
            vector<HotSpotData*>* firstRoot, vector<unsigned int>* firstRootSnumber, vector<int>* firstRootCounter, vector<int>* firstRootWptsPerVisit,
            vector<HotSpotData*>*& currentRoot, vector<unsigned int>*& currentRootSnumber, vector<int>*& currentRootCounter, vector<int>*& currentRootWptsPerVisit);
};




/**
 * ��������� ��������� ��������� �� ������ �������������� ������ �� ������� �������� �����.
 * ������� � ������� ������� ������������� � ������ RootsPersistenceAndStatistics.
 */
class GenerationRootsByStatisticsStrategy : public GenerationRootsStrategy {
private:
    /**
     * ��������� �� ������ � ����� ���������������� � ����������� ������ ��� ������) (������������� �� �����������)
     * �����: ������������������ ������� � RootsPersistenceAndStatistics ���������� � ������������������� � HotSpotsCollection
     * �����: ��� �������� �������� ������������ RootsPersistenceAndStatistics::findHotSpotIndexByName & HotSpotsCollection::findHotSpotbyName
     */
    RootsPersistenceAndStatistics* rootStatistics;

public:
    GenerationRootsByStatisticsStrategy(HotSpotsCollection* hsc, RootsPersistenceAndStatistics* rootStatistics, bool useFixedHomeLocation)
            : GenerationRootsStrategy(hsc, useFixedHomeLocation) {
        this->rootStatistics = rootStatistics;
    };

    ~GenerationRootsByStatisticsStrategy() {
        this->rootStatistics = NULL;
    };

    /**
     * �������� ���������� �������� � �������� ����� ��������� RegularRootLATP.
     */
    virtual bool generateNewRoot(
            vector<HotSpotData*>* firstRoot, vector<unsigned int>* firstRootSnumber, vector<int>* firstRootCounter, vector<int>* firstRootWptsPerVisit,
            vector<HotSpotData*>*& currentRoot, vector<unsigned int>*& currentRootSnumber, vector<int>*& currentRootCounter, vector<int>*& currentRootWptsPerVisit);

private:

    int generate(vector<double>* pdf);

    /**
     * ����� ���������� ��������� ����� ��������������� ������������� rootsDimensionHistogram.
     * ������ ����� ������������ ��� ���������� ���������� ������� � ������������ �������� (����������� ��������)
     */
    unsigned int generateRootDimension();

    /**
     * ����� ���������� ��������� ����� ��������������� ������������� summarizedIndicatorVector.
     * ������ ����� ������������ ��� ������ ��� �������, ������� ����� �������� � ������ ��������.
     *
     * �����: ������������ ��� ������ � ��������� ����� *.pst (������������������ � �� ���������� � ������������������� � HotSpotsCollection)
     */
    unsigned int generateHotSpotIndexPST();

    /**
     * �������� ���������� ��������� ��� ������ �������.
     * ����� ������ �� ����������������� ������� �� ���� ���������, ������� ������ �� ��������� ������������ ������.
     * ����� ������� � ������ ���������� ���������� ������� ���������� ��������� �� �� ���,
     * � ������� ���� ���� �� ���� ���������.
     *
     * �����: hotSpotIndexPST - ��� ������ � ��������� ����� *.pst (������������������ � �� ���������� � ������������������� � HotSpotsCollection)
     */
    unsigned int getHotSpotCount(unsigned int hotSpotIndexPST);

    /**
     * ����� ���������� ��������� ����� ��������������� ������������� homeHotspotHistogramPDF.
     * ������ ����� ������������ ��� ������ ��� �������, ������� ����� �������� � ������ �������� ��� ��������.
     *
     * �����: ������������ ��� ������ � ��������� ����� *.pst (������������������ � �� ���������� � ������������������� � HotSpotsCollection)
     */
    unsigned int generateHomeHotSpotIndexPST();
};

#endif
