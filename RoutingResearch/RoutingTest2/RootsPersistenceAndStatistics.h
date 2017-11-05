#ifndef ROOTSPERSISTENCEANDSTATISTICS_H_
#define ROOTSPERSISTENCEANDSTATISTICS_H_

#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <windows.h>
#include "INETDefs.h"
#include "Data.h"
#include <omnetpp.h>
#include "DevelopmentHelper.h"
#include "lib/pugixml-1.8/src/pugixml.hpp"

using namespace std;
using namespace pugi;

//---------------------------------- RootsPersistenceAndStatistics -----------------------------------------------

using namespace std;


/**
 * ����� �������� ����� ���� *.pst.
 */
class RootsPersistenceAndStatistics {
private:

    /**
     * ��� xml ����� �� ����������� ���������
     */
    string* xmlFileName;

    /**
     * ����������� ���������������
     */
    double persistenceFromMassCenter;

    /**
     * ����������� ����������� ���������
     * (���, �����, ����������� ����� �������� � ����������� ������������ ���������)
     */
    vector<int>* rootsDimensionHistogram;

    /**
     * ����� ������������� �� ������ rootsDimensionHistogram
     */
    vector<double>* rootsDimensionHistogramPDF;

    /**
     * ���������������� ������������ ������� �� ���� ������� ��������� - ����������� ������� ��������� ������� �� ���� �������� �������.
     * �.�. ������ ��������� ����� ���������� ����, � ������� ������������ ���� �� ��� ������� � �������.
     */
    vector<int>* summarizedIndicatorVector;

    /**
     * ����� ������������� �� ������ summarizedIndicatorVector
     */
    vector<double>* summarizedIndicatorVectorPDF;

    /**
     * ���������������� ������ �� ���� ���������, ��������� �� ��������� ������������ ������.
     * ����� ������� � ������ ���������� ���������� ������� ���������� ��������� �� �� ���,
     * � ������� ���� ���� �� ���� ���������.
     */
    vector<double>* averageCounterVector;

    /**
     * ����� �������, �� ��������� ������� ���������� ����������� �� �������� ���������.
     */
    vector<const char*>* hotSpots;

    void initialize();

    void parseData(const char* histogramStr, vector<int>*& histogram, vector<double>*& histogramPDF);

    void parseData(const char* vecStr, vector<double>*& vec);

    void parseData(const char* hotSpotsStr, vector<const char*>*& hotSpots);

    template<typename T>
    inline void print(const char* vecName, vector<T>*& vec) {
        if (vec) {
            cout << vecName << ": " << endl;
            for (unsigned int i = 0; i < vec->size(); i++) {
                cout << "'" << vec->at(i) << "'  ";
                if ((i+1) % 11 == 0) cout << endl;
            }
        } else {
            cout << vecName << ": null" << endl;
        }
        cout << endl;
    }

public:
    RootsPersistenceAndStatistics(const char* xmlFileName) {
        this->xmlFileName = new string(xmlFileName);
        this->persistenceFromMassCenter = -1;
        this->rootsDimensionHistogram = NULL;
        this->rootsDimensionHistogramPDF = NULL;
        this->summarizedIndicatorVector = NULL;
        this->summarizedIndicatorVectorPDF = NULL;
        this->averageCounterVector = NULL;
        this->hotSpots = NULL;
        initialize();
    }

    virtual ~RootsPersistenceAndStatistics() {
        if (this->xmlFileName) delete this->xmlFileName;
        if (this->rootsDimensionHistogram) delete this->rootsDimensionHistogram;
        if (this->rootsDimensionHistogramPDF) delete this->rootsDimensionHistogramPDF;
        if (this->summarizedIndicatorVector) delete this->summarizedIndicatorVector;
        if (this->summarizedIndicatorVectorPDF) delete this->summarizedIndicatorVectorPDF;
        if (this->averageCounterVector) delete this->averageCounterVector;
        if (this->hotSpots) {
            for (unsigned int i = 0; i < this->hotSpots->size(); i++) {
                delete [] this->hotSpots->at(i);
            }
            delete this->hotSpots;
        }
    }

    string* getXmlFileName() {return xmlFileName;};
    double getPersistenceFromMassCenter() {return persistenceFromMassCenter;};
    vector<double>* getRootsDimensionHistogramPDF() {return rootsDimensionHistogramPDF;};
    vector<double>* getSummarizedIndicatorVectorPDF() {return summarizedIndicatorVectorPDF;};
    vector<double>* getAverageCounterVector() {return averageCounterVector;};
    vector<const char*>* getHotSpots() {return hotSpots;};

    /**
     * ��������� ������� ������� � ����� *.pst �� ����� �����.
     */
    unsigned int findHotSpotIndexByName(const char* hotSpotName);

    void print();
};


//----------------------------- RootsPersistenceAndStatisticsCollection --------------------------------------


/**
 * ��������� ��������� �� ������� �������� ��������� ��� ���� �������������
 */
class RootsPersistenceAndStatisticsCollection {
private:
    static RootsPersistenceAndStatisticsCollection* instance;   // ��������� �� singleton ������

    /**
     * ����� �����������
     */
    RootsPersistenceAndStatistics* commonRootStatistics;

    /**
     * ����� ��������, �������������� ����� *.pst ��� ������� ���������� ����.
     */
    vector<RootsPersistenceAndStatistics*>* personalRootStatisticsList;

    RootsPersistenceAndStatisticsCollection() {
        this->commonRootStatistics = NULL;
        this->personalRootStatisticsList = NULL;
        this->readRootStatistics(RT_PST_ST, DEF_PST_DIR);
        this->print();
    }

    ~RootsPersistenceAndStatisticsCollection() {
        if (commonRootStatistics) delete commonRootStatistics;
        if (personalRootStatisticsList) {
            for (unsigned int i = 0; i < personalRootStatisticsList->size(); i++)
                delete personalRootStatisticsList->at(i);
            delete personalRootStatisticsList;
        }
    }

    void readRootStatistics(const char* commonRootStatisticsFileName, const char* personalRootStatisticsDir);

public:
    /**
     * ��������� �������� ���������������������� ������� ��� ������
     */
    static RootsPersistenceAndStatisticsCollection* getInstance();

    RootsPersistenceAndStatistics* getCommonRootStatistics() {return commonRootStatistics;}

    vector<RootsPersistenceAndStatistics*>* getPersonalRootStatisticsList() {return personalRootStatisticsList;}

    RootsPersistenceAndStatistics* findPersonalRootStatistics(unsigned int nodeId) {
        if (nodeId < personalRootStatisticsList->size()) {
            RootsPersistenceAndStatistics* stat = personalRootStatisticsList->at(nodeId);
            // ��������� ������������ ��������� id (� �������� ����� - ���� �� ��� ����) � nodeId
            ASSERT(checkFileIdAndNodeId(stat->getXmlFileName()->c_str(), nodeId));
            return stat;
        }

        return NULL;
    }

    void print();
};

#endif
