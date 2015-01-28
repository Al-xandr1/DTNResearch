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
#include "WaypointAnalyzer.h"
#include "WaypointGenerator.h"

using namespace std;


#define DEF_TRACE_DIR "./tracefiles" //Директория по умолчанию для трасс
#define DEF_WP_DIR "./waypointfiles" //Директория по умолчанию для путевых точек
#define DEF_BND_FILE_NAME "bounds.bnd" //Имя файла по умолчанию с границами
#define DEF_STAT_FILE_NAME "statistics.stat" //Имя файла по умолчанию со статистикой


char* buildWayPointFileName(char* name) {
    char* buffer = new char[256];
    strcpy(buffer, name);
    return strcat(buffer, ".wpt");
}

char* buildBoundsFileName(char* name) {
    char* buffer = new char[256];
    strcpy(buffer, name);
    return strcat(name, ".bnd");
}

char* buildStatisticFileName(char* name) {
    char* buffer = new char[256];
    strcpy(buffer, name);
    return strcat(name, ".stat");
}

char* buildFullName(char* dir, char* fileName) {
    char* buffer = new char[256];
    strcpy(buffer, dir);
    strcat(buffer, "/");
    strcat(buffer, fileName);
}



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
        traceFilesDir = argv[1];
        wayPointFilesDir = DEF_WP_DIR;
        break;
    case 4 :
    default:
        traceFilesDir=argv[1];
        wayPointFilesDir=argv[2];
        break;
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

            //запись в отдельный файл границы ТРАССЫ
            test.traceBounds.write(buildFullName(traceFilesDir, buildBoundsFileName(f.cFileName)));
            //запись в отдельный файл границы ПУТЕВЫХ ТОЧЕК
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
        fileDir = DEF_WP_DIR;
        break;
    case 3 :
    default:
        fileDir=argv[2];
        break;
    }

    WIN32_FIND_DATA f;
    if (FindFirstFile(fileDir, &f) == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "File directory for file analyzing not found.\n");
        exit(-777);
    }

    char* fileNamePattern = buildFullName(fileDir, "*.wpt"); //todo или "*.txt" в параметр ком строки
    cout << "   fileNamePattern: " << fileNamePattern << endl << endl;

    WaypointAnalyzer analyzer(buildFullName(fileDir, DEF_BND_FILE_NAME)); //todo 1000 в параметр ком строки и ГРАНИЦУ в параметр ком строки

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



#define STAT "-stat" //команда для сбора статистики
#define WP "-wp" //команда для нахождения путевых точек

int main(int argc, char** argv)
{
    argc = 2; argv = new char*[2] {"program", STAT} ; //REMOVE HARDCORE!

    cout << "Program start!" << endl << endl;

    int result = -1;
    switch(argc)
    {
    case 1 :
        fprintf(stderr, "You must specify command:\n\t %s \n\t %s\n", WP, STAT);
        exit(-444);

    case 2 :
    default:
        char* command = argv[1];
        if (strcmp(command, WP) == 0) {
            result = mainForWPFinder(argc, argv);

        } else if (strcmp(command, STAT) == 0) {
            result = mainForAnalyzer(argc, argv);

        } else {
            fprintf(stderr, "Unknown command %s. Permitted commands:\n\t %s \n\t %s\n", command, WP, STAT);
            exit(result = -555);
        }
    };

    cout << endl << "Program complete." << endl << endl;
    return result;
}
