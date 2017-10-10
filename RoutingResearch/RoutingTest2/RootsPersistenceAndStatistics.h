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
     * ���������������� ������������ ������� �� ���� ������� ��������� - ����������� ������� ��������� ������� �� ���� �������� �������.
     * �.�. ������ ��������� ����� ���������� ����, � ������� ������������ ���� �� ��� ������� � �������.
     */
    vector<int>* summarizedIndicatorVector;

    /**
     * ����������� ����������� ���������
     * (���, �����, ����������� ����� �������� � ����������� ������������ ���������)
     */
    vector<int>* rootsDimensionHistogram;

    /**
     * ����� �������� ���������������� ������ �� ���� ���������
     */
    vector<int>* summarizedRoot;

    void print();

public:
    RootsPersistenceAndStatistics() {
        this->persistenceFromMassCenter = -1;
        this->summarizedIndicatorVector = NULL;
        this->rootsDimensionHistogram = NULL;
        this->summarizedRoot = NULL;
    }

    virtual ~RootsPersistenceAndStatistics() {
        if (this->summarizedIndicatorVector) delete this->summarizedIndicatorVector;
        if (this->rootsDimensionHistogram) delete this->rootsDimensionHistogram;
        if (this->summarizedRoot) delete this->summarizedRoot;
    }

    virtual void initialize();
    virtual void handleMessage(cMessage *msg) {};

    double getPersistenceFromMassCenter() {return persistenceFromMassCenter;};
    vector<int>* getSummarizedIndicatorVector() {return summarizedIndicatorVector;};
    vector<int>* getRootsDimensionHistogram() {return rootsDimensionHistogram;};
    vector<int>* getSummarizedRoot() {return summarizedRoot;};
};

#endif
