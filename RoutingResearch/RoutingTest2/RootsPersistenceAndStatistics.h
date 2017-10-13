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
    vector<int>* rootsDimensionHistogram;
    vector<double>* rootsDimensionHistogramPDF;

    /**
     * ���������������� ������������ ������� �� ���� ������� ��������� - ����������� ������� ��������� ������� �� ���� �������� �������.
     * �.�. ������ ��������� ����� ���������� ����, � ������� ������������ ���� �� ��� ������� � �������.
     */
    vector<int>* summarizedIndicatorVector;
    vector<double>* summarizedIndicatorVectorPDF;

    /**
     * ����� �������� ���������������� ������ �� ���� ���������
     */
    vector<int>* summarizedRoot;
    vector<double>* summarizedRootPDF;

    void loadData(cXMLElement *root, const char* histTag, const char* valsTag, vector<int>*& histogram, vector<double>*& histogramPDF);
    void print(const char* histogramName, vector<int>*& histogram);
    void print(const char* histogramName, vector<double>*& histogram);

public:
    RootsPersistenceAndStatistics() {
        this->persistenceFromMassCenter = -1;
        this->rootsDimensionHistogram = NULL;
        this->rootsDimensionHistogramPDF = NULL;
        this->summarizedIndicatorVector = NULL;
        this->summarizedIndicatorVectorPDF = NULL;
        this->summarizedRoot = NULL;
        this->summarizedRootPDF = NULL;
    }

    virtual ~RootsPersistenceAndStatistics() {
        if (this->rootsDimensionHistogram) delete this->rootsDimensionHistogram;
        if (this->rootsDimensionHistogramPDF) delete this->rootsDimensionHistogramPDF;
        if (this->summarizedIndicatorVector) delete this->summarizedIndicatorVector;
        if (this->summarizedIndicatorVectorPDF) delete this->summarizedIndicatorVectorPDF;
        if (this->summarizedRoot) delete this->summarizedRoot;
        if (this->summarizedRootPDF) delete this->summarizedRootPDF;
    }

    virtual void initialize();
    virtual void handleMessage(cMessage *msg) {};

    double getPersistenceFromMassCenter() {return persistenceFromMassCenter;};
    vector<int>* getRootsDimensionHistogram() {return rootsDimensionHistogram;};
    vector<int>* getSummarizedIndicatorVector() {return summarizedIndicatorVector;};
    vector<int>* getSummarizedRoot() {return summarizedRoot;};

    /**
     * ����� ���������� ��������� ����� ��������������� ������������� rootsDimensionHistogram.
     * ������ ����� ������������ ��� ���������� ���������� ������� � ������������ �������� (����������� ��������)
     */
    int generateRootDimension();

    /**
     * ����� ���������� ��������� ����� ��������������� ������������� summarizedIndicatorVector.
     * ������ ����� ������������ ��� ������ ��� �������, ������� ����� �������� � ������ ��������.
     */
    int generateHotSpotIndex();

    /**
     * todo ��� ���������� ������ �����???
     */
    int generateIndexFromSummarizedRoot();
};

#endif
