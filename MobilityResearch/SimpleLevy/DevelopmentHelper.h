#ifndef DEVELOPMENTHELPER_H_INCLUDED
#define DEVELOPMENTHELPER_H_INCLUDED

#include <string>
#include <vector>
#include <math.h>
#include <list>
#include <sstream>
#include <string>
#include <omnetpp.h>

using namespace std;

#define DEF_TR_DIR "./Traces"                  //Директория по умолчанию для всeй информации о трассах
#define DEF_HS_DIR "./Traces/hotspotfiles"     //Директория по умолчанию для "локаций"
#define DEF_WP_DIR "./Traces/waypointfiles"    //Директория по умолчанию для "путевых точек"
#define DEF_RT_DIR "./Traces/rootfiles"


#define OUT_DIR        "outTrace"              // директория для сохранения выходной информации
#define WPS_DIR        "waypointfiles"         // директория для сохранения сгенерированных путевых точек
#define TRS_DIR        "tracefiles"            // директория для сохранения сгенерированных трасс
#define HS_DIR         "hotspotfiles"          // директория для сохранения выходной информации о локациях
#define LOC_FILE       "locations.loc"         // имя файла с локациями
#define SPC_FILE       "spotcount.cnt"         // имя файла с количеством посещенией
#define ALLROOTS_FILE  "allroots.roo"          // имя файла


#define VAR_FILE  "variances.txt"   // имя файла с дисперсиями

// формат файлов
#define TRACE_TYPE ".txt"
#define WAYPOINTS_TYPE ".wpt"


class NamesAndDirs {

public:
    static char* getOutDir();
    static char* getWpsDir();
    static char* getTrsDir();
    static char* getHsDir();
    static char* getLocFile();
    static char* getSptCntFile();
};


char* buildFullName(char* dir, char* fileName);
char* createFileName(char* buffer, int numberOfExperiment,
                     const char* rawName, int index, const char* fileType);
int countMaxValue(list<int> queueSizePoints);
double getLength(double x1, double y1, double x2, double y2);

#endif // DEVELOPMENTHELPER_H_INCLUDED
