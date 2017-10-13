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
    vector<int>* rootsDimensionHistogram;
    vector<double>* rootsDimensionHistogramPDF;

    /**
     * Просуммированные индикаторные вектора от всех дневных маршрутов - гистограмму частоты появления локации за весь горизонт анализа.
     * Т.е. каждый компонент равен количеству дней, в которые пользователь хотя бы раз заходил в локацию.
     */
    vector<int>* summarizedIndicatorVector;
    vector<double>* summarizedIndicatorVectorPDF;

    /**
     * Метод получает просуммированный вектор от всех маршрутов
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
     * Метод генерирует случайное число пропорчионально распределению rootsDimensionHistogram.
     * Данное число используется для количества уникальных локаций в генерируемом маршруте (размерность маршрута)
     */
    int generateRootDimension();

    /**
     * Метод генерирует случайное число пропорчионально распределению summarizedIndicatorVector.
     * Данное число используется как индекс той локации, которую нужно включить в состав маршрута.
     */
    int generateHotSpotIndex();

    /**
     * todo что генерирует данный метод???
     */
    int generateIndexFromSummarizedRoot();
};

#endif
