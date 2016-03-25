#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <cmath>
#include <queue>
#include "Reader.h"
#include "Bounds.h"
#include "Histogram.cpp"
#include "Area.h"
#include "Statistics.h"

#ifndef WAYPOINTANALYZER_INCLUDED
#define WAYPOINTANALYZER_INCLUDED


class PointsAnalyzer {
    //todo доработать для испольщования для трасс

private:
    Bounds* bounds; //граница генерации путевых точек
    Statistics* commonStat;

public:
    PointsAnalyzer(char* boundsFileName)
    {
        this->bounds = new Bounds(boundsFileName);
        this->commonStat = new Statistics(this->bounds);
    }

    ~PointsAnalyzer()
    {
        delete this->bounds;
        delete this->commonStat;
    }

    void analyze(char* pointsFileName, char* statFileName)
    {
        cout << "\t" << "Points analyzing start...";
        Statistics* statPerFile = new Statistics(this->bounds);

        WayPointReader* reader = new WayPointReader(pointsFileName);
        WayPoint *point = NULL;
        int row = 1;
        while (reader->hasNext()) {
            point = reader->next();
            if (!statPerFile->addPoint(point))
            {
                cout << "\t" << row << "  "; point->print();
                statPerFile->printParams();
                exit(-222);
            }
            if (!commonStat->addPoint(point))
            {
                cout << "\t" << row << "  "; point->print();
                commonStat->printParams();
                exit(-223);
            }
            row++;
            delete point;
        }
        delete reader;

        statPerFile->write(statFileName);

        delete statPerFile;
        this->commonStat->resetStat();
        cout << endl << "\t" << "Points analyzed!" << endl << endl;
        cout << "-------------------------------------------------------------------------------------------------------------" << endl;
    }

    void writeStatistics(char* statFileName)
    {
        this->commonStat->write(statFileName);
    }
};

#endif //WAYPOINTANALYZER_INCLUDED
