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
 * Класс загрузки файла типа *.pst (roots_persistence_statistics.pst).
 * Предоставляет методы генерации случайных величин на основе статистик из файла.
 */
class RootsPersistenceAndStatistics : public cSimpleModule {
private:
    /**
     * Коэффициент персистентности
     */
    double persistenceFromMassCenter;

    /**
     * Гистограмма размерности маршрутов
     * (или, иначе, гистограмма весов векторов в многомерном пространстве маршрутов)
     */
    vector<double>* rootsDimensionHistogramPDF;

    /**
     * Просуммированные индикаторные вектора от всех дневных маршрутов - гистограмму частоты появления локации за весь горизонт анализа.
     * Т.е. каждый компонент равен количеству дней, в которые пользователь хотя бы раз заходил в локацию.
     */
    vector<double>* summarizedIndicatorVectorPDF;

    /**
     * Метод получает просуммированный вектор от всех маршрутов
     */
    vector<double>* summarizedRootPDF;

    void loadData(cXMLElement *root, const char* histTag, const char* valsTag, vector<int>*& histogram, vector<double>*& histogramPDF);
    void print(const char* histogramName, vector<int>*& histogram);
    void print(const char* histogramName, vector<double>*& histogram);

public:
    RootsPersistenceAndStatistics() {
        this->persistenceFromMassCenter = -1;
        this->rootsDimensionHistogramPDF = NULL;
        this->summarizedIndicatorVectorPDF = NULL;
        this->summarizedRootPDF = NULL;
    }

    virtual ~RootsPersistenceAndStatistics() {
        if (this->rootsDimensionHistogramPDF) delete this->rootsDimensionHistogramPDF;
        if (this->summarizedIndicatorVectorPDF) delete this->summarizedIndicatorVectorPDF;
        if (this->summarizedRootPDF) delete this->summarizedRootPDF;
    }

    virtual void initialize();
    virtual void handleMessage(cMessage *msg) {};

    double getPersistenceFromMassCenter() {return persistenceFromMassCenter;};
    vector<double>* getRootsDimensionHistogramPDF() {return rootsDimensionHistogramPDF;};
    vector<double>* getSummarizedIndicatorVectorPDF() {return summarizedIndicatorVectorPDF;};
    vector<double>* getSummarizedRootPDF() {return summarizedRootPDF;};
};

#endif
