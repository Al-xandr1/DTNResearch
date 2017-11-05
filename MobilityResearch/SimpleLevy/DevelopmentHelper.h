#ifndef DEVELOPMENTHELPER_H_INCLUDED
#define DEVELOPMENTHELPER_H_INCLUDED

#include <string>
#include <vector>
#include <math.h>
#include <list>
#include <sstream>
#include <string>
#include "stlutils.h"
#include <omnetpp.h>

using namespace std;

// Имена директорий и файлов
#define DEF_TR_DIR     (char*)"./Traces"                //Директория по умолчанию для всeй информации о трассах
#define DEF_HS_DIR     (char*)"./Traces/hotspotfiles"   //Директория по умолчанию для "локаций"
#define DEF_TRS_DIR    (char*)"./Traces/tracefiles"     //Директория по умолчанию для исходных трасс
#define DEF_WP_DIR     (char*)"./Traces/waypointfiles"  //Директория по умолчанию для "путевых точек"
#define DEF_RT_DIR     (char*)"./Traces/rootfiles"      //Директория по умолчанию для "маршрутов"
#define DEF_PST_DIR    (char*)"./Traces/rootstatistics" //Директория по умолчанию для статистик маршрутов (файлов *.pst)

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
#define RT_PST_ST      (char*)"roots_persistence_statistics.pst"  // имя файла с данными о персистентности и статистике маршрутов

// Константы
#define PERSISTENCE    (char*)"persistence"           // имя параметра с коэффициентом персистентности в имени файла маршрута *.rot

// Формат файлов
#define TRACE_TYPE     (char*)".txt"
#define WAYPOINTS_TYPE (char*)".wpt"

// Шаблон файлов
#define TRACE_PATTERN  (char*)"*.txt"
#define ROOT_PATTERT   (char*)"*.rot"
#define PST_PATTERT    (char*)"*.pst"

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
 * Формирует параметр в строке с целочисленным значением (имени файла).
 */
const char* buildIntParameter(const char* name, int value);

/**
 * Формирует параметр в строке (имени файла).
 */
const char* buildParameter(const char* name, const char* value);

/**
 * Метод проверяет соответствие параметра id (в названии файла - ЕСЛИ он там есть) и nodeId
 */
bool checkFileIdAndNodeId(const char* filename, int nodeId);

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

/**
 * Получение суммы элементов вектора
 */
template<typename T>
inline T getSum(const std::vector<T>& vector) {
    T sum = 0;
    for(unsigned int i=0; i < vector.size(); i++) sum += vector.at(i);
    return sum;
}

/**
 * trim from start
 */
inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

/**
 *  trim from end
 */
inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

/**
 *  trim from both ends
 */
inline std::string &fullTrim(std::string &s) {
    return ltrim(rtrim(s));
}

#endif // DEVELOPMENTHELPER_H_INCLUDED
