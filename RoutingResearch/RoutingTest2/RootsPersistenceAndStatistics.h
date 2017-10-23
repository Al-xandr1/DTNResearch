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
     * Просуммированный вектор от всех маршрутов, поделённый на суммарный индикаторный вектор.
     * Таким образом в каждой компоненте получается среднее количество посещений за те дни,
     * в которые было хотя бы одно посещение.
     */
    vector<double>* averageCounterVector;

    /**
     * Набор локаций, на основании которых собиралась статисктика от реальных маршрутов.
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
     * Получение индекса локации в файле *.pst по имени файла.
     */
    unsigned int findHotSpotIndexByName(const char* hotSpotName);
};

#endif
