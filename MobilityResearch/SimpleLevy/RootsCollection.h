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
    static RootsCollection *instance;                  // указатель на singleton объект

    // эти две коллекции хранят данные о локациях маршрута. Последовательность данных в них синхронизированна
    vector<vector<HotSpotDataRoot> *> *RootsData;      // набор маршрутов пользователей. Структура - информация из файлов *.rot
    vector<RootDataShort>             *RootsDataShort; // набор маршрутов пользователей. Структура соответствует файлу allroots.roo
    vector<RootsCollection *>         *DailyRoot;      // набор коллекций маршрутов для каждого дня (из подпапок _day=***_ в папке rootfiles)

    /* Набор сгенерированных маршрутов пользователей. Набор данных - информация для записи в файлы *.rot
     * Структура данных:
     *      vector<HotSpotDataRoot>*  - конкретный сохранённый маршрут конкретного пользователя в конкретный день;
     *      vector<vector<HotSpotDataRoot>*>*  - набор маршрутов, где ИНДЕКС - это НОМЕР ДНЯ. Т.е. это маршруты КОНКРЕТНОГО узла за ВСЕ дни;
     *      vector<vector<vector<HotSpotDataRoot>*>*>*  - набор маршрутов всех дней по УЗЛАМ. Индекс внешнего вектора номер УЗЛА.
     *
     * Т.е. индекс по строкам в матрице - номер узла
     *      индекс по столбцам в матрице - номер дня
     */
    vector<vector<vector<HotSpotDataRoot*> *> *> *generatedTheoryRootsData; // для хранение СГЕНЕРИРОВАННЫХ, НО НЕ ПРОЙДЕННЫХ маршрутов
    vector<vector<vector<HotSpotDataRoot*> *> *> *generatedActualRootsData; // для хранение ФАКТИЧЕСКИ ПРОЙДЕННЫХ маршрутов

    RootsCollection(const char* TracesDir, const char* allRootsFile, const char* rootsDir, const char* filePattern) {
        this->RootsDataShort = NULL;
        this->RootsData = NULL;
        this->DailyRoot = NULL;
        this->generatedTheoryRootsData = NULL;
        this->generatedActualRootsData = NULL;
        this->readRootsData(TracesDir, allRootsFile, rootsDir, filePattern);
        this->readDailyRoots(TracesDir, allRootsFile, rootsDir, filePattern);
        this->printRootsData();
        this->printRootsDataShort();
        if (DailyRoot) {
            cout << "RootsCollection: dailyRoots loaded, count days = " << DailyRoot->size() << endl;
        }
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
        if (generatedTheoryRootsData) {
            for (unsigned int i=0; i<generatedTheoryRootsData->size(); i++) {
                vector<vector<HotSpotDataRoot*>*>* rootsPerNode = generatedTheoryRootsData->at(i);
                for (unsigned int j=0; j<rootsPerNode->size(); j++) {
                    vector<HotSpotDataRoot*>* dailyRoot = rootsPerNode->at(j);
                    for (unsigned int k=0; k<dailyRoot->size(); k++) {
                        delete dailyRoot->at(k);
                    }
                    delete dailyRoot;
                }
                delete rootsPerNode;
            }
            delete generatedTheoryRootsData;
            generatedTheoryRootsData = NULL;
        }
        if (generatedActualRootsData) {
            for (unsigned int i=0; i<generatedActualRootsData->size(); i++) {
                vector<vector<HotSpotDataRoot*>*>* rootsPerNode = generatedActualRootsData->at(i);
                for (unsigned int j=0; j<rootsPerNode->size(); j++) {
                    vector<HotSpotDataRoot*>* dailyRoot = rootsPerNode->at(j);
                    for (unsigned int k=0; k<dailyRoot->size(); k++) {
                        delete dailyRoot->at(k);
                    }
                    delete dailyRoot;
                }
                delete rootsPerNode;
            }
            delete generatedActualRootsData;
            generatedActualRootsData = NULL;
        }
    }

    void readRootsData(const char* TracesDir, const char* allRootsFile, const char* rootsDir, const char* filePattern);

    void readDailyRoots(const char* fakeTracesDir, const char* fakeAllRootsFile, const char* rootsDir, const char* filePattern);

    void innerSaveRoots(const char *logPrefix, const char *rtDir, vector<vector<vector<HotSpotDataRoot*> *> *> *generatedRootsData);

public:
    /**
    * Получение готового проинициализированного объекта для работы
    */
    static RootsCollection *getInstance();

    vector<vector<HotSpotDataRoot> *> *getRootsData() { return RootsData; }

    vector<RootDataShort> *getRootsDataShort() { return RootsDataShort; }

    vector<RootsCollection *> *getDailyRoot() { return DailyRoot; };

    RootDataShort *getRootDataShortByNodeId(int nodeId) {
        ASSERT(nodeId >= 0 && nodeId < RootsDataShort->size());
        RootDataShort *rootDataShort = &(RootsDataShort->at(nodeId));
        // проверяем соответствие параметра id (в названии файла - ЕСЛИ он там есть) и nodeId
        ASSERT(checkFileIdAndNodeId(rootDataShort->RootName, nodeId));
        return rootDataShort;
    }

    vector<HotSpotDataRoot> *getRootDataByNodeId(int nodeId) {
        ASSERT(nodeId >= 0 && nodeId < RootsData->size());
        // вызываем для контроля соответствия параметра id из названия файла и nodeId
        getRootDataShortByNodeId(nodeId);
        return RootsData->at(nodeId);
    }

    vector<vector<vector<HotSpotDataRoot*>*>*> *getGeneratedTheoryRootsData() {return generatedTheoryRootsData;}

    vector<vector<vector<HotSpotDataRoot*>*>*> *getGeneratedActualRootsData() {return generatedActualRootsData;}

    /**
     * Сохраняет указанный машрут в качестве СФОРМИРОВАННОГО, НО НЕ ПРОЙДЕННОГО для указанного пользователя в конкретный указанный день.
     */
    void collectTheoryRoot(vector<HotSpotData*>* root,
                           vector<unsigned int>* rootSnumber,
                           vector<int>* rootCounter,
                           unsigned int nodeId,
                           unsigned int day);

    /**
     * Сохраняет указанный машрут в качестве ФАКТИЧЕСКИ ПРОЙДЕННОГО для указанного пользователя в конкретный указанный день.
     */
    void collectActualRoot(vector<HotSpotData*>* root,
                           vector<unsigned int>* rootSnumber,
                           vector<int>* rootCounter,
                           vector<unsigned int>* rootTrack,
                           vector<double>* rootTrackSumTime,
                           vector<int>* rootTrackWaypointNum,
                           unsigned int nodeId,
                           unsigned int day);

    void printRootsDataShort();

    void printRootsData();

    void saveRoots(const char *thRtDir, const char *acRtDir);

private:
    /**
     * Сохраняет указанный машрут для указанного пользователя в конкретный указанный день.
     * generatedRootsData - это структура, в которую предполагается проводить сохранение.
     * На текущий момент возможно две структуры: для фактически пройденных маршрутов и для сформированных, но не пройденных.
     *
     * По описанию из RegularRootLATP.h:
     *     vector<HotSpotData*>*      currentRoot;         - сформированный вектор (текущий) маршрута с информацией, загруженной из файлов *.hts
     *     vector<unsigned int>*      currentRootSnumber;  - сформированный вектор (текущий) с индексами локаций в структуре HotSpotsCollection
     *     vector<int>*               currentRootCounter;  - сформированный вектор (текущий) со счётчиками посещений локаций
     *     vector<unsigned int>*      rootTrack            - сформированный вектор фактической последовательности появления локаций в маршруте
     *     vector<double>*            rootTrackSumTime     - сформированный вектор фактических времён, проведённых в появляющихся локациях маршрута
     *     vector<int>*               rootTrackWaypointNum - сформированный вектор фактического количества путевых точек, пройденных в появляющихся локациях маршрута
     *     unsigned int               nodeId               - ID узла, для которого сохраняется маршрут
     *     unsigned int               day                  - номер дня, для которого сохраняется маршрут.
     *                                                       Дни нумеруются с 1, но структуре generatedRootsData они хранятся начиная с 0.
     */
    void static collectRoot(vector<vector<vector<HotSpotDataRoot*> *> *> *generatedRootsData,
                            vector<HotSpotData*>* root,
                            vector<unsigned int>* rootSnumber,
                            vector<int>* rootCounter,
                            vector<unsigned int>* rootTrack,
                            vector<double>* rootTrackSumTime,
                            vector<int>* rootTrackWaypointNum,
                            unsigned int nodeId,
                            unsigned int day);
};

#endif /* ROOTSCOLLECTION_H_ */
