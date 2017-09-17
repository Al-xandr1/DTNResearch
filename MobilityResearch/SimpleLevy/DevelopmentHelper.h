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
#define PERSISTENCE    (char*)"persistence"           // ��� ��������� � ������������� ��������������� � ����� ����� �������� *.rot

// ������ ������
#define TRACE_TYPE     (char*)".txt"
#define WAYPOINTS_TYPE (char*)".wpt"

// ������ ������
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
 * ��������� ������� ��� ����� �� ������� ���� � ����.
 * �������� ��������  KAIST_30sec_002.txt.wpt.rot  ��  ./rootfiles/KAIST_30sec_002.txt.wpt.rot
 */
string extractSimpleName(const char* fullName);

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

#endif // DEVELOPMENTHELPER_H_INCLUDED
