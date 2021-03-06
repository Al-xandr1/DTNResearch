#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <math.h>
#include <queue>
#include <windows.h>
#include "WaypointFinder.h"
#include "PointsAnalyzer.h"
#include "WaypointGenerator.h"
#include "TraceMerger.h"
#include "HotSpot.h"
#include "../libs/pugixml-1.5/src/pugixml.hpp"

using namespace std;


#define DEF_TRACE_DIR "./tracefiles"        //���������� �� ��������� ��� �����
#define DEF_WP_DIR "./waypointfiles"        //���������� �� ��������� ��� ������� �����
#define DEF_HS_DIR "./hotspotfiles"         //���������� �� ��������� ��� "������� �����"
#define DEF_BND_FILE_NAME "bounds.bnd"      //��� ����� �� ��������� � ���������
#define DEF_ALL_TRACES_FILE_NAME "allTraces.movements"//��� ����� �� ��������� �� �����������
#define DEF_STAT_FILE_NAME "statistics.stat"//��� ����� �� ��������� �� �����������

#define MFMAKE "-mfmake" //������� ��� ������������ ����� � �� ����� �������� (movement file make)
#define WPFIND "-wpfind" //������� ��� ���������� ������� �����
#define WPGEN "-wpgen" //������� ��� ���������� ������� �����
#define STAT "-stat" //������� ��� ����� ����������
#define WPT "--wpt"  //�������� ��� ����� ���������� ��� ������� �����
#define TXT "--txt"  //�������� ��� ����� ���������� ��� ������


int mainForWPFinder(int argc, char** argv)
{
    cout << "Finder start!" << endl << endl;

    char* traceFilesDir;    //full path name of directory
    char* wayPointFilesDir; //full path name of directory
    switch(argc)
    {
    case 2 :
        traceFilesDir = DEF_TRACE_DIR;
        wayPointFilesDir = DEF_WP_DIR;
        break;
    case 3 :
        traceFilesDir = argv[2];
        wayPointFilesDir = DEF_WP_DIR;
        break;
    case 4 :
    default:
        traceFilesDir = argv[2];
        wayPointFilesDir = argv[3];
    }

    WIN32_FIND_DATA f;
    if (FindFirstFile(wayPointFilesDir, &f) == INVALID_HANDLE_VALUE)
    {
        if (CreateDirectory(wayPointFilesDir, NULL))
            cout << "create output directory " << endl;
        else
            cout << "error create output directory" << endl;
    }

    char* traceFileNamePattern = buildFullName(traceFilesDir, "*.txt");
    cout << "   traceFileNamePattern: " << traceFileNamePattern << endl << endl;

    HANDLE h = FindFirstFile(traceFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE)
    {
        do
        {
            char* inputFileName = buildFullName(traceFilesDir, f.cFileName);
            cout << "       inputFileName: " << inputFileName << endl;

            char* wpFileName = buildWayPointFileName(f.cFileName);
            char* outPutFileName = buildFullName(wayPointFilesDir, wpFileName);
            cout << "       outPutFileName: " << outPutFileName << endl << endl;

            WaypointFinder test(inputFileName, outPutFileName);
            test.findWaypoints();

            //������ � ��������� ���� ������� ������
            test.traceBounds.write(buildFullName(traceFilesDir, buildBoundsFileName(f.cFileName)));
            //������ � ��������� ���� ������� ������� �����
            test.wayPointBounds.write(buildFullName(wayPointFilesDir, buildBoundsFileName(wpFileName)));

            delete inputFileName;
            delete wpFileName;
            delete outPutFileName;
        }
        while(FindNextFile(h, &f));
    }
    else
    {
        fprintf(stderr, "Directory or files not found\n");
    }

    cout << endl << "\t totalTraceBounds:" << endl;
    WaypointFinder::totalTraceBounds.print();
    cout << endl;
    cout << "\t totalWayPointBounds:" << endl;
    WaypointFinder::totalWayPointBounds.print();

    WaypointFinder::totalTraceBounds.write(buildFullName(traceFilesDir, DEF_BND_FILE_NAME));
    WaypointFinder::totalWayPointBounds.write(buildFullName(wayPointFilesDir, DEF_BND_FILE_NAME));

    cout << "Finder end." << endl << endl;
    return 0;
}



int mainForAnalyzer(int argc, char** argv) {
    cout << "Analyzing start!" << endl << endl;

    char* fileDir;    //full path name of directory
    switch(argc)
    {
    case 2 :
        fprintf(stderr, "You must specify parameter of the command -stat:\n\t %s \n\t %s\n", WPT, TXT);
        exit(-446);

    case 3 :
        fileDir = DEF_WP_DIR;
        break;
    case 4:
    default:
        fileDir = argv[3];
    }

    char* fileType;
    if (strcmp(argv[2], WPT) == 0) {
        fileType = "*.wpt";
    }
    else if (strcmp(argv[2], TXT) == 0) {
        fileType = "*.txt";
    }
    else {
        fprintf(stderr, "Unknown parameter of command -stat: %s. Permitted parameters:\n\t %s \n\t %s\n", argv[3], WPT, TXT);
        exit(-523);
    }

    WIN32_FIND_DATA f;
    if (FindFirstFile(fileDir, &f) == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "File directory for file analyzing not found.\n");
        exit(-777);
    }

    char* fileNamePattern = buildFullName(fileDir, fileType);
    cout << "   fileNamePattern: " << fileNamePattern << endl << endl;



    ifstream boundsFile(buildFullName(fileDir, DEF_BND_FILE_NAME));
    if (boundsFile == NULL) {
        // create file with bounds
        Bounds* totalBounds = new Bounds();
        HANDLE h = FindFirstFile(fileNamePattern, &f);
        if(h != INVALID_HANDLE_VALUE)
        {
            do
            {
                char* pointsFileName = buildFullName(fileDir, f.cFileName);
                cout << "       pointsFileName: " << pointsFileName << endl;
                WayPointReader* reader = new WayPointReader(pointsFileName);
                while (reader->hasNext()) {
                    WayPoint *point = reader->next();
                    totalBounds->changeBounds(point->x, point->y);
                    delete point;
                }
                delete pointsFileName;
                delete reader;
            }
            while(FindNextFile(h, &f));
        }
        else fprintf(stderr, "Directory or files not found\n");
        totalBounds->write(buildFullName(fileDir, DEF_BND_FILE_NAME));
    }



    PointsAnalyzer analyzer(buildFullName(fileDir, DEF_BND_FILE_NAME));

    HANDLE h = FindFirstFile(fileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE)
    {
        do
        {
            char* inputFileName = buildFullName(fileDir, f.cFileName);
            cout << "       inputFileName: " << inputFileName << endl;

            char* statFileName = buildStatisticFileName(f.cFileName);
            char* outPutFileName = buildFullName(fileDir, statFileName);
            cout << "       outPutFileName: " << outPutFileName << endl << endl;

            analyzer.analyze(inputFileName, outPutFileName);

            delete inputFileName;
            delete outPutFileName;
        }
        while(FindNextFile(h, &f));
    }
    else
    {
        fprintf(stderr, "Directory or files not found\n");
    }

    analyzer.writeStatistics(buildFullName(fileDir, DEF_STAT_FILE_NAME));

    cout << "Analyzing end." << endl << endl;
    return 0;
}



int mainForGenerator(int argc, char** argv) {
    cout << "Generating start!" << endl << endl;

    char* fileDir;    //full path name of directory
    switch(argc)
    {
    case 2 :
        fileDir = DEF_WP_DIR;
        break;
    case 3:
    default:
        fileDir = argv[2];
    }

    char* boundFileName = buildFullName(fileDir, DEF_BND_FILE_NAME);
    char* statFileName = buildFullName(fileDir, DEF_STAT_FILE_NAME);
    char* generatedWPFileName = buildFullName(fileDir, "generatedWayPoints.wpt");  //todo ������� ������ ������������ �����

    WaypointGenerator generator(boundFileName, statFileName);
    generator.generate(generatedWPFileName);

    cout << "Generating end." << endl << endl;
    return 0;
}



int mainForMovementsFile(int argc, char** argv)
{
    cout << "Making movements file start!" << endl << endl;

    char* traceFilesDir;    //full path name of directory
    switch(argc)
    {
    case 2 :
        traceFilesDir = DEF_TRACE_DIR;
        break;
    case 3 :
    default:
        traceFilesDir = argv[2];
    }

    WIN32_FIND_DATA f;
    if (FindFirstFile(traceFilesDir, &f) == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "File directory for making movements file not found.\n");
        exit(-776);
    }

    char* traceFileNamePattern = buildFullName(traceFilesDir, "*.txt");
    cout << "   traceFileNamePattern: " << traceFileNamePattern << endl << endl;

    TraceMerger* traceMerger;

    HANDLE h = FindFirstFile(traceFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE)
    {
        traceMerger = new TraceMerger(buildFullName(traceFilesDir, DEF_ALL_TRACES_FILE_NAME));

        do
        {
            char* inputFileName = buildFullName(traceFilesDir, f.cFileName);
            cout << "       inputFileName: " << inputFileName << endl;

            traceMerger->addFile(inputFileName);

            delete inputFileName;
        }
        while(FindNextFile(h, &f));
    }
    else
    {
        fprintf(stderr, "Directory or files not found\n");
    }

    if (traceMerger) delete traceMerger;

    cout << endl << "Making movements file end." << endl << endl;
    return 0;
}


int mainMain(int argc, char** argv)
{
    cout << "Program start!" << endl << endl;

    int result = -1;
    switch(argc)
    {
    case 1 :
        fprintf(stderr, "You must specify command:\n\t %s \n\t %s\n", WPFIND, STAT);
        exit(-444);

    case 2 :
    default:
        char* command = argv[1];
        if (strcmp(command, WPFIND) == 0) {
            result = mainForWPFinder(argc, argv);

        } else if (strcmp(command, STAT) == 0) {
            result = mainForAnalyzer(argc, argv);

        } else if (strcmp(command, WPGEN) == 0) {
            result = mainForGenerator(argc, argv);

        } else if (strcmp(command, MFMAKE) == 0) {
            result = mainForMovementsFile(argc, argv);

        } else {
            fprintf(stderr, "Unknown command %s. Permitted commands:\n\t %s \n\t %s \n\t %s \n\t %s\n", command, WPFIND, WPGEN, STAT, MFMAKE);
            exit(result = -555);
        }
    };

    cout << endl << "Program complete." << endl << endl;
    return result;
}

// ------------------------------------------  Program script  --------------------------------------------
int main(int argc, char** argv)
{
    argc = 2; argv = new char*[2] {"program", WPFIND};
    mainMain(argc, argv);
    argc = 3; argv = new char*[3] {"program", STAT, WPT};
    mainMain(argc, argv);
}
