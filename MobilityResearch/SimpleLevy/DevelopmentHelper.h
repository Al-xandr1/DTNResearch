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

#define DEF_TR_DIR     (char*)"./Traces"              //���������� �� ��������� ��� ��e� ���������� � �������
#define DEF_HS_DIR     (char*)"./Traces/hotspotfiles" //���������� �� ��������� ��� "�������"
#define DEF_TRS_DIR    (char*)"./Traces/tracefiles"   //���������� �� ��������� ��� �������� �����
#define DEF_WP_DIR     (char*)"./Traces/waypointfiles"//���������� �� ��������� ��� "������� �����"
#define DEF_RT_DIR     (char*)"./Traces/rootfiles"    //���������� �� ��������� ��� "���������"

#define OUT_DIR        (char*)"outTrace"              // ���������� ��� ���������� �������� ����������
#define WPS_DIR        (char*)"waypointfiles"         // ���������� ��� ���������� ��������������� ������� �����
#define TRS_DIR        (char*)"tracefiles"            // ���������� ��� ���������� ��������������� �����
#define HS_DIR         (char*)"hotspotfiles"          // ���������� ��� ���������� �������� ���������� � ��������
#define LOC_FILE       (char*)"locations.loc"         // ��� ����� � ���������
#define SPC_FILE       (char*)"spotcount.cnt"         // ��� ����� � ����������� ����������
#define ALLROOTS_FILE  (char*)"allroots.roo"          // ��� �����
#define VAR_FILE       (char*)"variances.txt"         // ��� ����� � �����������
#define PACKETS_HIST   (char*)"packetsHistory.xml"    // ��� ����� ��� ���������� ������� �������
#define ICT_HIST       (char*)"ictHistory.xml"        // ��� ����� ��� ���������� ������� ICT
#define RT_HIST        (char*)"routeHistory.xml"      // ��� ����� ��� ���������� ������� � ���������� ��������� ������
#define STAT_FILE      (char*)"statistics.xml"        // ��� ����� ��� ���������� ����������

// ������ ������
#define TRACE_TYPE     (char*)".txt"
#define WAYPOINTS_TYPE (char*)".wpt"

// ������ ������
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
