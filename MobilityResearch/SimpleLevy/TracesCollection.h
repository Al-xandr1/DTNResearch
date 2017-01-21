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
 * ��������� ���� �������� ��������� � �������������. ������������� ����� tracefiles. Singleton ������
 */
class TracesCollection {
private:
    static TracesCollection* instance;    // ��������� �� singleton ������

    vector<vector<TracePoint>*>* traces;  // ����� ���� �����

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
     * ��������� �������� ���������������������� ������� ��� ������
     */
    static TracesCollection* getInstance();

    vector<vector<TracePoint>*>* getTraces() {return traces;}
    void print();
};

#endif
