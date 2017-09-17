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
#define TH_RT_DIR      (char*)"rootfiles_theory"      // директория для сохранения выходной информации о сгенерированных маршрутах (ТЕОРЕТИЧЕСКИХ, не пройденных)
#define AC_RT_DIR      (char*)"rootfiles_actual"      // директория для сохранения выходной информации о пройденных маршрутах (ФАКТИЧЕСКИХ, пройденных)
#define LOC_FILE       (char*)"locations.loc"         // имя файла с локациями
#define SPC_FILE       (char*)"spotcount.cnt"         // имя файла с количеством посещенией
#define ALLROOTS_FILE  (char*)"allroots.roo"          // имя файла
#define VAR_FILE       (char*)"variances.txt"         // имя файла с дисперсиями
#define PACKETS_HIST   (char*)"packetsHistory.xml"    // имя файла для сохранения истории пакетов
#define ICT_HIST       (char*)"ictHistory.xml"        // имя файла для сохранения истории ICT
#define RT_HIST        (char*)"routeHistory.xml"      // имя файла для сохранения истории о пройденных маршрутах узлами
#define STAT_FILE      (char*)"statistics.xml"        // имя файла для сохранения статистики
#define PERSISTENCE    (char*)"persistence"           // имя параметра с коэффициентом персистентности в имени файла маршрута *.rot

// формат файлов
#define TRACE_TYPE     (char*)".txt"
#define WAYPOINTS_TYPE (char*)".wpt"

// шаблон файлов
#define TRACE_PATTERN  (char*)"*.txt"
#define ROOT_PATTERT   (char*)"*.rot"


class NamesAndDirs {

public:
    static const char *getOutDir();

    static const char *getOutWpsDir();

    static const char *getOutTrsDir();

    static const char *getOutHsDir();

    static const char *getOutTheoryRtDir();

    static const char *getOutActualRtDir();

    static const char *getOutLocFile();

    static const char *getOutSptCntFile();
};


const char *buildFullName(const char *dir, const char *fileName);

char *createFileName(char *buffer, int numberOfExperiment,
                     const char *rawName, int index, const char *fileType);

int countMaxValue(list<int> queueSizePoints);

double getLength(double x1, double y1, double x2, double y2);

/**
 * Извлекает простое имя файла из полного пути к нему.
 * Например получает  KAIST_30sec_002.txt.wpt.rot  из  ./rootfiles/KAIST_30sec_002.txt.wpt.rot
 */
string extractSimpleName(const char* fullName);

/**
 * Извлекает значение параметра из строки (имени файла).
 * Извлекает параметр методом extractParameter и преобразует в число.
 *
 * @return  Возвращает указтель на число, так как если такого параметра нет,
 *          то нужно это обработать - в таком случае возвращается NULL:
 */
double* extractDoubleParameter(const char* fileName, const char* parameter);

/**
 * Извлекает значение параметра из строки (имени файла).
 * Имя параметра передаётся в качестве входного аргумента функции.
 * Наличие параметра определяется по вхождении имени параметра в имя файла.
 * Значение параметра находиться после указанного имени параметра между знаками = и _
 */
const char* extractParameter(const char* fileName, const char* parameter);

#endif // DEVELOPMENTHELPER_H_INCLUDED
