#ifndef __ROUTINGTEST2_STATISTICSCALCULATOR_H_
#define __ROUTINGTEST2_STATISTICSCALCULATOR_H_

#include <iostream>
#include <fstream>
#include <omnetpp.h>
#include <Messages.h>
#include "DevelopmentHelper.h"

using namespace std;

class StatisticsCalculator : public cSimpleModule
{
private:
    cXMLElement *routeHistoryOne;  // первый "вектор" длительности маршрутов
    cXMLElement *routeHistoryTwo;  // второй "вектор" длительности маршрутов

    /*
     * В общем случай один файл истории хранит длительности для N узлов для D дней.
     * Эти данный загружаются в виде матрицы N на D:
     *                  N - строк (номер строки = номер узла)
     *                  D - столбцов (номер столбца = номер дня)
     */
    vector<vector<double>*>* durationMatrixOne;
    vector<vector<double>*>* durationMatrixTwo;

protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg) {};

    /*
     * Загружает матрицы длительностей всех файлов.
     */
    vector<vector<double>*>* loadMatrix(cXMLElement *routeHistory);
    double norm(vector<vector<double>*>* matrixOne, vector<vector<double>*>* matrixTwo);
    void print(vector<vector<double>*>* matrix, const char* name);

public:
    StatisticsCalculator() {
        routeHistoryOne = NULL;
        routeHistoryTwo = NULL;
        durationMatrixOne = NULL;
        durationMatrixTwo = NULL;
    }
};

#endif
