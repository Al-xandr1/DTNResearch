#ifndef DEVELOPMENTHELPER_H_INCLUDED
#define DEVELOPMENTHELPER_H_INCLUDED

#include <string>
#include <vector>
#include <math.h>
#include <list>
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include "stlutils.h"
#include <omnetpp.h>

using namespace std;

#define myDelete(obj)      if(obj){delete   obj; obj = NULL;}
#define myDeleteArray(obj) if(obj){delete[] obj; obj = NULL;}

// ����� ���������� � ������
#define DEF_TR_DIR     (char*)"./Traces"                //���������� �� ��������� ��� ��e� ���������� � �������
#define DEF_HS_DIR     (char*)"./Traces/hotspotfiles"   //���������� �� ��������� ��� "�������"
#define DEF_TRS_DIR    (char*)"./Traces/tracefiles"     //���������� �� ��������� ��� �������� �����
#define DEF_WP_DIR     (char*)"./Traces/waypointfiles"  //���������� �� ��������� ��� "������� �����"
#define DEF_RT_DIR     (char*)"./Traces/rootfiles"      //���������� �� ��������� ��� "���������"
#define DEF_PST_DIR    (char*)"./Traces/rootstatistics" //���������� �� ��������� ��� ��������� ��������� (������ *.pst)

#define OUT_DIR        (char*)"outTrace"              // ���������� ��� ���������� �������� ����������
#define WPS_DIR        (char*)"waypointfiles"         // ���������� ��� ���������� ��������������� ������� �����
#define TRS_DIR        (char*)"tracefiles"            // ���������� ��� ���������� ��������������� �����
#define HS_DIR         (char*)"hotspotfiles"          // ���������� ��� ���������� �������� ���������� � ��������
#define TH_RT_DIR      (char*)"rootfiles_theory"      // ���������� ��� ���������� �������� ���������� � ��������������� ��������� (�������������, �� ����������)
#define AC_RT_DIR      (char*)"rootfiles_actual"      // ���������� ��� ���������� �������� ���������� � ���������� ��������� (�����������, ����������)
#define LOC_FILE       (char*)"locations.loc"         // ��� ����� � ���������
#define SPC_FILE       (char*)"spotcount.cnt"         // ��� ����� � ����������� ����������
#define ALLROOTS_FILE  (char*)"allroots.roo"          // ��� �����
#define VAR_FILE       (char*)"variances.txt"         // ��� ����� � �����������
#define PACKETS_HIST   (char*)"packetsHistory.xml"    // ��� ����� ��� ���������� ������� �������
#define ICT_HIST       (char*)"ictHistory.xml"        // ��� ����� ��� ���������� ������� ICT
#define RT_HIST        (char*)"routeHistory.xml"      // ��� ����� ��� ���������� ������� � ���������� ��������� ������
#define STAT_FILE      (char*)"statistics.xml"        // ��� ����� ��� ���������� ����������
#define STAT_STAT_FILE (char*)"statistics.stat"        // ��� ����� ��� ���������� ���������� � ������������ (��� ������, �.�. �������� ������ � \DTNResearch\Utils\WaypointFinder)
#define RT_PST_ST      (char*)"roots_persistence_statistics.pst"  // ��� ����� � ������� � ��������������� � ���������� ���������

// ���������
#define PERSISTENCE    (char*)"persistence"           // ��� ��������� � ������������� ��������������� � ����� ����� �������� *.rot

// ������ ������
#define TRACE_TYPE     (char*)".txt"
#define WAYPOINTS_TYPE (char*)".wpt"

// ������ ������
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

ofstream* createXmlFile(const char* filename, const char* openRootTag) ;

void closeXmlFile(ofstream* xmlFile, const char* closeRootTag);

int countMaxValue(list<int> queueSizePoints);

double getLength(double x1, double y1, double x2, double y2);

/**
 * ��������� ������� ��� ����� �� ������� ���� � ����.
 * �������� ��������  KAIST_30sec_002.txt.wpt.rot  ��  ./rootfiles/KAIST_30sec_002.txt.wpt.rot
 */
string extractSimpleName(const char* fullName);

/**
 * ��������� �������� � ������ � ������������� ��������� (����� �����).
 */
const char* buildIntParameter(const char* name, int value, int leftPadByZero);

/**
 * ��������� �������� � ������ (����� �����).
 */
const char* buildParameter(const char* name, const char* value);

/**
 * ����� ��������� ������������ ��������� id (� �������� ����� - ���� �� ��� ����) � nodeId
 */
bool checkFileIdAndNodeId(const char* filename, int nodeId);

/**
 * ��������� �������� ��������� �� ������ (����� �����).
 * ��������� �������� ������� extractParameter � ����������� � �����.
 *
 * @return  ���������� �������� �� �����, ��� ��� ���� ������ ��������� ���,
 *          �� ����� ��� ���������� - � ����� ������ ������������ NULL:
 */
double* extractDoubleParameter(const char* fileName, const char* parameter);

/**
 * ��������� �������� ��������� �� ������ (����� �����).
 * ��� ��������� ��������� � �������� �������� ��������� �������.
 * ������� ��������� ������������ �� ��������� ����� ��������� � ��� �����.
 * �������� ��������� ���������� ����� ���������� ����� ��������� ����� ������� = � _
 */
const char* extractParameter(const char* fileName, const char* parameter);

/**
 * ��������� ����� ��������� �������
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

template <class T>
void deleteVector(vector<T*>*& deleteme, bool deleteVector) {
    while(!deleteme->empty()) {
        myDelete(deleteme->back());
        deleteme->pop_back();
    }
    if (deleteVector) myDelete(deleteme);
}

inline vector<string>* getSubDirectories(const char* currentDirectory) {
    vector<string>* subDirectories = new vector<string>();

    WIN32_FIND_DATA fi;
    HANDLE h = FindFirstFileEx(
            (string(currentDirectory) + string("/*")).c_str(),
            FindExInfoStandard,
            &fi,
            FindExSearchLimitToDirectories,
            NULL,
            0);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                if (strcmp(fi.cFileName, (char*)".") != 0 && strcmp(fi.cFileName, (char*)"..") != 0) {
                    subDirectories->push_back(string(fi.cFileName));
                }
            }
        } while (FindNextFile(h, &fi));
        FindClose(h);
    }

    return subDirectories;
}

double checkValue(const double value, const double maxPermittedValue, const char* where = "unknown");

#endif // DEVELOPMENTHELPER_H_INCLUDED
