#ifndef ROOTSCOLLECTION_H_
#define ROOTSCOLLECTION_H_

#include <math.h>
#include <string.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include "INETDefs.h"
#include "Data.h"
#include "Coord.h"
#include "DevelopmentHelper.h"

using namespace std;

/*
 * Коллекция маршрутов (в виде последовательности локаций) для пользователей. Singleton объект
 */
class RootsCollection {
private:
    static RootsCollection* instance;             // указатель на singleton объект

    vector<RootDataShort>* RootsDataShort;        // набор маршрутов пользователей. Структура соответствует файлу allroots.roo
    vector<vector<HotSpotDataRoot>*>* RootsData;  // набор маршрутов пользователей. Структура - набор последовательностей *.hts

    RootsCollection() {
        this->RootsDataShort = NULL;
        this->RootsData = NULL;
        this->readRootsData(DEF_TR_DIR, ALLROOTS_FILE, DEF_RT_DIR, ROOT_PATTERT);
        this->printRootsDataShort();
        this->printRootsData();
    }

    ~RootsCollection() {
        if (RootsDataShort) {
            delete RootsDataShort;
            RootsDataShort = NULL;
        }
        if (RootsData) {
            for (unsigned int i=0; i<RootsData->size(); i++) {
                delete RootsData->at(i);
                delete RootsData;
            }
            RootsData = NULL;
        }
    }

    void readRootsData(char* TracesDir, char* allRootsFile, char* rootsDir, char* filePatter);

public:
    /**
    * Получение готового проинициализированного объекта для работы
    */
    static RootsCollection* getInstance();

    vector<RootDataShort>* getRootsDataShort()       {return RootsDataShort;}
    vector<vector<HotSpotDataRoot>*>* getRootsData() {return RootsData;}

    void printRootsDataShort();
    void printRootsData();
};


#endif /* ROOTSCOLLECTION_H_ */
