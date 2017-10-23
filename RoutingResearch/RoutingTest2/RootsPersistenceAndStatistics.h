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
#include <cxmlelement.h>

using namespace std;


/**
 * ����� �������� ����� ���� *.pst (roots_persistence_statistics.pst).
 * ������������� ������ ��������� ��������� ������� �� ������ ��������� �� �����.
 */
class RootsPersistenceAndStatistics : public cSimpleModule {
private:
    /**
     * ����������� ���������������
     */
    double persistenceFromMassCenter;

    /**
     * ����������� ����������� ���������
     * (���, �����, ����������� ����� �������� � ����������� ������������ ���������)
     */
    vector<double>* rootsDimensionHistogramPDF;

    /**
     * ���������������� ������������ ������� �� ���� ������� ��������� - ����������� ������� ��������� ������� �� ���� �������� �������.
     * �.�. ������ ��������� ����� ���������� ����, � ������� ������������ ���� �� ��� ������� � �������.
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

    void loadData(cXMLElement *root, const char* histTag, const char* valsTag, vector<int>*& histogram, vector<double>*& histogramPDF);

    void loadData(cXMLElement *root, const char* vecTag, const char* valsTag, vector<double>*& vec);

    void loadData(cXMLElement *root, const char* vecTag, const char* valsTag, vector<const char*>*& hotSpots);

    template<typename T>
    inline void print(const char* vecName, vector<T>*& vec) {
        if (vec) {
            cout << vecName << ": " << endl;
            for (unsigned int i = 0; i < vec->size(); i++) {
                cout << "'" << vec->at(i) << "'  ";
                if ((i+1) % 12 == 0) cout << endl;
            }
        } else {
            cout << vecName << ": null" << endl;
        }
        cout << endl << endl;
    }

public:
    RootsPersistenceAndStatistics() {
        this->persistenceFromMassCenter = -1;
        this->rootsDimensionHistogramPDF = NULL;
        this->summarizedIndicatorVectorPDF = NULL;
        this->averageCounterVector = NULL;
        this->hotSpots = NULL;
    }

    virtual ~RootsPersistenceAndStatistics() {
        if (this->rootsDimensionHistogramPDF) delete this->rootsDimensionHistogramPDF;
        if (this->summarizedIndicatorVectorPDF) delete this->summarizedIndicatorVectorPDF;
        if (this->averageCounterVector) delete this->averageCounterVector;
        if (this->hotSpots) {
            for (unsigned int i = 0; i < this->hotSpots->size(); i++) {
                delete [] this->hotSpots->at(i);
            }
            delete this->hotSpots;
        }
    }

    virtual void initialize();
    virtual void handleMessage(cMessage *msg) {};

    double getPersistenceFromMassCenter() {return persistenceFromMassCenter;};
    vector<double>* getRootsDimensionHistogramPDF() {return rootsDimensionHistogramPDF;};
    vector<double>* getSummarizedIndicatorVectorPDF() {return summarizedIndicatorVectorPDF;};
    vector<double>* getAverageCounterVector() {return averageCounterVector;};
    vector<const char*>* getHotSpots() {return hotSpots;};

    /**
     * ��������� ������� ������� � ����� *.pst �� ����� �����.
     */
    unsigned int findHotSpotIndexByName(const char* hotSpotName);
};

#endif
