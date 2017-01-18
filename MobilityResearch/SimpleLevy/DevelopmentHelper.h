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

#define DEF_TR_DIR     (char*)"./Traces"              //Директория по умолчанию для всeй информации о трассах
#define DEF_HS_DIR     (char*)"./Traces/hotspotfiles" //Директория по умолчанию для "локаций"
#define DEF_TRS_DIR    (char*)"./Traces/tracefiles"   //Директория по умолчанию для исходных трасс
#define DEF_WP_DIR     (char*)"./Traces/waypointfiles"//Директория по умолчанию для "путевых точек"
#define DEF_RT_DIR     (char*)"./Traces/rootfiles"    //Директория по умолчанию для "маршрутов"

#define OUT_DIR        (char*)"outTrace"              // директория для сохранения выходной информации
#define WPS_DIR        (char*)"waypointfiles"         // директория для сохранения сгенерированных путевых точек
#define TRS_DIR        (char*)"tracefiles"            // директория для сохранения сгенерированных трасс
#define HS_DIR         (char*)"hotspotfiles"          // директория для сохранения выходной информации о локациях
#define LOC_FILE       (char*)"locations.loc"         // имя файла с локациями
#define SPC_FILE       (char*)"spotcount.cnt"         // имя файла с количеством посещенией
#define ALLROOTS_FILE  (char*)"allroots.roo"          // имя файла
#define VAR_FILE       (char*)"variances.txt"         // имя файла с дисперсиями
#define PACKETS_HIST   (char*)"packetsHistory.xml"    // имя файла для сохранения истории пакетов
#define ICT_HIST       (char*)"ictHistory.xml"        // имя файла для сохранения истории ICT
#define RT_HIST        (char*)"routeHistory.xml"      // имя файла для сохранения истории о пройденных маршрутах узлами
#define STAT_FILE      (char*)"statistics.xml"        // имя файла для сохранения статистики

// формат файлов
#define TRACE_TYPE     (char*)".txt"
#define WAYPOINTS_TYPE (char*)".wpt"

// шаблон файлов
#define TRACE_PATTERN  (char*)"*.txt"
#define ROOT_PATTERT   (char*)"*.rot"


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
