#ifndef TRACESCOLLECTION_H_
#define TRACESCOLLECTION_H_

#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <windows.h>
#include "INETDefs.h"
#include "Data.h"
#include "Coord.h"
#include "DevelopmentHelper.h"

using namespace std;

/**
 * Коллекция всех реальных маршрутов в моделировании. Соответствует папке tracefiles. Singleton объект
 */
class TracesCollection {
private:
    static TracesCollection* instance;    // указатель на singleton объект

    vector<vector<TracePoint>*>* traces;  // набор всех трасс

    TracesCollection() {
        this->traces = NULL;
        this->readTraces(DEF_TRS_DIR, TRACE_PATTERN);
        this->print();
    }

    ~TracesCollection() {
        if (traces) delete traces;
    }

    void readTraces(char* TracesDir, char* filePatter);

public:
    /**
     * Получение готового проинициализированного объекта для работы
     */
    static TracesCollection* getInstance();

    vector<vector<TracePoint>*>* getTraces() {return traces;}
    void print();
};

#endif
