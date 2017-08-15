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
    static RootsCollection *instance;              // указатель на singleton объект

    // эти две коллекции хранят данные о локациях маршрута. Последовательность данных в них синхронизированна
    //todo а теперь RootsDataShort нужен???
    vector<RootDataShort> *RootsDataShort;         // набор маршрутов пользователей. Структура соответствует файлу allroots.roo
    vector<vector<HotSpotDataRoot> *> *RootsData;  // набор маршрутов пользователей. Структура - информация из файлов *.rot

    /* Набор сгенерированных маршрутов пользователей. Набор данных - информация для записи в файлы *.rot
     * Структура данных:
     *      vector<HotSpotDataRoot>*  - конкретный сохранённый маршрут конкретного пользователя в конкретный день;
     *      vector<vector<HotSpotDataRoot>*>*  - набор маршрутов, где ИНДЕКС - это НОМЕР ДНЯ. Т.е. это маршруты КОНКРЕТНОГО узла за ВСЕ дни;
     *      vector<vector<vector<HotSpotDataRoot>*>*>*  - набор маршрутов всех дней по УЗЛАМ. Индекс внешнего вектора номер УЗЛА.
     *
     * Т.е. индекс по строкам в матрице - номер узла
     *      индекс по столбцам в матрице - номер дня
     */
    vector<vector<vector<HotSpotDataRoot*> *> *> *generatedRootsData;

    RootsCollection() {
        this->RootsDataShort = NULL;
        this->RootsData = NULL;
        this->generatedRootsData = NULL;
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
            for (unsigned int i = 0; i < RootsData->size(); i++) {
                delete RootsData->at(i);
            }
            delete RootsData;
            RootsData = NULL;
        }
        if (generatedRootsData) {
            for (unsigned int i=0; i<generatedRootsData->size(); i++) {
                vector<vector<HotSpotDataRoot*>*>* rootsPerNode = generatedRootsData->at(i);
                for (unsigned int j=0; j<rootsPerNode->size(); j++) {
                    vector<HotSpotDataRoot*>* dailyRoot = rootsPerNode->at(j);
                    for (unsigned int k=0; k<dailyRoot->size(); k++) {
                        delete dailyRoot->at(k);
                    }
                    delete dailyRoot;
                }
                delete rootsPerNode;
            }
            delete generatedRootsData;
            generatedRootsData = NULL;
        }
    }

    void readRootsData(char *TracesDir, char *allRootsFile, char *rootsDir, char *filePatter);

public:
    /**
    * Получение готового проинициализированного объекта для работы
    */
    static RootsCollection *getInstance();

    vector<RootDataShort> *getRootsDataShort() { return RootsDataShort; }

    vector<vector<HotSpotDataRoot> *> *getRootsData() { return RootsData; }

    RootDataShort *getRootDataShortByNodeId(int nodeId) { return &(RootsDataShort->at(nodeId)); }

    vector<HotSpotDataRoot> *getRootDataByNodeId(int nodeId) { return RootsData->at(nodeId); }

    /**
     * Сохраняет указанный машрут для указанного пользователя в конкретный указанный день.
     * По описанию из RegularRootLATP.h:
     *     vector<HotSpotData*>*      currentRoot;         - сформированный вектор (текущий) маршрута с информацией, загруженной из файлов *.hts
     *     vector<unsigned int>*      currentRootSnumber;  - сформированный вектор (текущий) с индексами локаций в структуре HotSpotsCollection
     *     vector<int>*               currentRootCounter;  - сформированный вектор (текущий) со счётчиками посещений локаций
     *     unsigned int               nodeId               - ID узла, для которого сохраняется маршрут
     *     unsigned int               day                  - номер дня, для которого сохраняется маршрут.
     *                                                       Дни нумеруются с 1, но структуре generatedRootsData они хранятся начиная с 0.
     */
    void collectRoot(vector<HotSpotData*>* root, vector<unsigned int>* rootSnumber, vector<int>* rootCounter, unsigned int nodeId, unsigned int day);

    void printRootsDataShort();

    void printRootsData();

    void saveGeneratedRootsData();
};

#endif /* ROOTSCOLLECTION_H_ */
