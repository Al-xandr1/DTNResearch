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

#define DEF_TR_DIR "./Traces"                  //���������� �� ��������� ��� ��e� ���������� � �������
#define DEF_HS_DIR "./Traces/hotspotfiles"     //���������� �� ��������� ��� "�������"
#define DEF_WP_DIR "./Traces/waypointfiles"    //���������� �� ��������� ��� "������� �����"
#define DEF_RT_DIR "./Traces/rootfiles"


#define OUT_DIR        "outTrace"              // ���������� ��� ���������� �������� ����������
#define WPS_DIR        "waypointfiles"         // ���������� ��� ���������� ��������������� ������� �����
#define TRS_DIR        "tracefiles"            // ���������� ��� ���������� ��������������� �����
#define HS_DIR         "hotspotfiles"          // ���������� ��� ���������� �������� ���������� � ��������
#define LOC_FILE       "locations.loc"         // ��� ����� � ���������
#define SPC_FILE       "spotcount.cnt"         // ��� ����� � ����������� ����������
#define ALLROOTS_FILE  "allroots.roo"          // ��� �����


#define VAR_FILE  "variances.txt"   // ��� ����� � �����������

// ������ ������
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
