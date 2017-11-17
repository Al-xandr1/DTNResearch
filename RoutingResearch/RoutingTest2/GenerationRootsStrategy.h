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
public:
    GenerationRootsStrategy() {;
    }
    virtual ~GenerationRootsStrategy() {
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
    double rootPersistence;     // ������������� ����������� ��������������� (������������� �� �����������)
    HotSpotsCollection* hsc;    // ��������� �� ��������� ������� (������ ��� ������) (������������� �� �����������)

public:
    GenerationRootsByPersistenceStrategy(double rootPersistence, HotSpotsCollection* hsc) : GenerationRootsStrategy() {
        this->rootPersistence = rootPersistence;
        this->hsc = hsc;
    };

    ~GenerationRootsByPersistenceStrategy() {
        hsc = NULL;
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

    /** ��������� �� ������ � ����� ���������������� � ����������� ������ ��� ������) (������������� �� �����������)
     */
    RootsPersistenceAndStatistics* rootStatistics;
    /**
     * ��������� �� ��������� ������� (������ ��� ������) (������������� �� �����������)
     */
    HotSpotsCollection* hsc;
    // �����: ������������������ ������� � RootsPersistenceAndStatistics ���������� � ������������������� � HotSpotsCollection
    // �����: ��� �������� �������� ������������ RootsPersistenceAndStatistics::findHotSpotIndexByName & HotSpotsCollection::findHotSpotbyName

public:
    GenerationRootsByStatisticsStrategy(RootsPersistenceAndStatistics* rootStatistics, HotSpotsCollection* hsc) : GenerationRootsStrategy() {
        this->rootStatistics = rootStatistics;
        this->hsc = hsc;
    };

    ~GenerationRootsByStatisticsStrategy() {
        this->rootStatistics = NULL;
        this->hsc = NULL;
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
};

#endif
