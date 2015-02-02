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

#ifndef WAYPOINTANALYZER_INCLUDED
#define WAYPOINTANALYZER_INCLUDED


class Statistics
{
private:
    Area* areaTree; //дерево площадей для аналиха дисперсии многих трасс
    Histogram* lengthHist;
    Histogram* velocityHist;
    Histogram* pauseHist;
    WayPoint *previous;

public:
    Statistics(Bounds* bounds)
    {
        this->areaTree = Area::createTreeStructure(bounds);
        this->lengthHist =   new Histogram(10000, bounds->getDiagLength());
        this->velocityHist = new Histogram(1000, 10);
        this->pauseHist =    new Histogram(1000, 100000);
        this->previous = NULL;
    }

    ~Statistics()
    {
        delete this->areaTree;
        delete this->lengthHist;
        delete this->velocityHist;
        delete this->pauseHist;
        if (previous != NULL) delete previous;
    }

    bool addPoint(WayPoint* point)
    {
        bool success = areaTree->putInArea(point);

        double pause = point->pauseDuration();
        if (pause <= 0)
        {
            cout << endl << "\t" << "Statistics addPoint(): unexpected magnitude of value: " << endl;
            cout << "\t\tpause= " << pause << endl;
            cout << "\t\tpoint:  ";  point->print();
            exit(334);
        }
        pauseHist->put(pause);

        if (previous)
        {
            double dist = previous->distance(point);
            double flyDur = previous->flyDuration(point);
            if (dist<0 || flyDur<=0)
            {
                cout << endl << "\t" << "Statistics addPoint(): unexpected magnitude of value: " << endl;
                cout << "\t\tdist= " << dist << ",   flyDur= " << flyDur << endl;
                cout << "\t\tprevious:  ";   previous->print();
                cout << "\t\tpoint:  ";      point->print();
                exit(335);
            }
            lengthHist->put(dist);
            velocityHist->put(dist/flyDur);
            delete previous;
        }
        previous = new WayPoint(point);

        return success;
    }

    void resetStat()
    {
        if (this->previous) delete previous;
        previous = NULL;
    }

    void printParams()
    {
        this->areaTree->getBounds()->print();
    }

    void writeStatistics(ofstream* out, char* tag, Histogram* hist)
    {
        cout << "\t<" << tag << " checkSum=\"" << hist->getCheckSum()
                             << "\" totalValues=\"" << hist->getTotalValues()
                             << "\" underflowValues=\"" << hist->getUnderflowValues()
                             << "\" overflowValues=\"" << hist->getOverflowValues() << "\">"<< endl;
        *out << "  <" << tag << " checkSum=\"" << hist->getCheckSum()
                             << "\" totalValues=\"" << hist->getTotalValues()
                             << "\" underflowValues=\"" << hist->getUnderflowValues()
                             << "\" overflowValues=\"" << hist->getOverflowValues() << "\">"<< endl;

        cout << "\t  <CELLS>" << hist->getCells() << "</CELLS>" << endl;
        *out << "    <CELLS>" << hist->getCells() << "</CELLS>" << endl;
        cout << "\t  <CELL-WIDTH>" << hist->getWidthOfCell() << "</CELL-WIDTH>" << endl;
        *out << "    <CELL-WIDTH>" << hist->getWidthOfCell() << "</CELL-WIDTH>" << endl;
        cout << "\t  <LEFT-BOUND>" << hist->getLeftBound() << "</LEFT-BOUND>" << endl;
        *out << "    <LEFT-BOUND>" << hist->getLeftBound() << "</LEFT-BOUND>" << endl;
        cout << "\t  <RIGHT-BOUND>" << hist->getRightBound() << "</RIGHT-BOUND>" << endl;
        *out << "    <RIGHT-BOUND>" << hist->getRightBound() << "</RIGHT-BOUND>" << endl;

        cout << "\t  <PDF-VALS>" << endl;
        *out << "    <PDF-VALS>" << endl;
        vector<double>* pdf = hist->toPDFVector();
        for (int i = 0; i < pdf->size(); i++)
        {
//            cout << (*pdf)[i]; if (i != pdf->size()-1) cout << "  "; else cout << endl;
            *out << (*pdf)[i]; if (i != pdf->size()-1) *out << "  "; else *out << endl;
        }
        cout << "\t  </PDF-VALS>" << endl;
        *out << "    </PDF-VALS>" << endl;

        cout << "\t  <CDF-VALS>" << endl;
        *out << "    <CDF-VALS>" << endl;
        vector<double>* cdf = hist->toCDFVector();
        for (int i = 0; i < cdf->size(); i++)
        {
//            cout << (*cdf)[i]; if (i != cdf->size()-1) cout << "  "; else cout << endl;
            *out << (*cdf)[i]; if (i != cdf->size()-1) *out << "  "; else *out << endl;
        }
        cout << "\t  </CDF-VALS>" << endl;
        *out << "    </CDF-VALS>" << endl;

        cout << "\t  <CCDF-VALS>" << endl;
        *out << "    <CCDF-VALS>" << endl;
        vector<double>* ccdf = hist->toCCDFVector();
        for (int i = 0; i < ccdf->size(); i++)
        {
//            cout << (*ccdf)[i]; if (i != ccdf->size()-1) cout << "  "; else cout << endl;
            *out << (*ccdf)[i]; if (i != ccdf->size()-1) *out << "  "; else *out << endl;
        }
        cout << "\t  </CCDF-VALS>" << endl;
        *out << "    </CCDF-VALS>" << endl;

        cout << "\t</" << tag << ">" << endl;
        *out << "  </" << tag << ">" << endl;
        delete pdf;
        delete cdf;
        delete ccdf;
    }

    void write(char* statFileName)
    {
        ofstream* statFile = new ofstream(statFileName);
        if (statFile == NULL)
        {
            cout << endl << "\t" << "Statistics write(): Output file " << statFileName << " opening failed." << endl;
            exit(333);
        }
        (*statFile) << "<?xml version=\'1.0' ?>" << endl;
        (*statFile) << "<STATISTICS>" << endl;
        Area::writeStatistics(statFile, areaTree);
        this->lengthHist->truncate();
        this->velocityHist->truncate();
        this->pauseHist->truncate();
        writeStatistics(statFile, "FLIGHT-LENGTH-HISTOGRAM",   this->lengthHist);
        writeStatistics(statFile, "VELOCITY-HISTOGRAM", this->velocityHist);
        writeStatistics(statFile, "PAUSE-HISTOGRAM",    this->pauseHist);
        (*statFile) << "</STATISTICS>" << endl;
        statFile->close();
        delete statFile;
    }
};



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
