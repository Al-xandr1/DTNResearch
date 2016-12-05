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
 * Коллекция маршрутов (в виде последовательности локаций) для пользователей
 */
class RootsCollection {
public:
    static bool isRootsCollectionReady;
    static vector<RootDataShort> RootsDataShort;        // набор маршрутов пользователей. Структура соответствует файлу allroots.roo
    static vector<vector<HotSpotDataRoot>*> RootsData;  // набор маршрутов пользователей. Структура - набор последовательностей *.hts

    void readRootsData(char* TracesDir, char* allRootsFile, char* rootsDir, char* filePatter);

    void printRootsDataShort();
    void printRootsData();
};


#endif /* ROOTSCOLLECTION_H_ */
