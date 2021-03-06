#include "../libs/pugixml-1.5/src/pugixml.hpp"
#include "../libs/Tokenizer.h"
#include <iostream>
#include <string>
#include "DevelopmentHelper.h"

#ifndef STATISTICS_H_INCLUDED
#define STATISTICS_H_INCLUDED

using namespace pugi;
using namespace std;


class Statistics
{
private:
    //���� ��� ������� ����� � ������� �����
    Area* areaTree;         //������ �������� ��� ������� ��������� ������ �����
    Histogram* lengthHist;
    Histogram* velocityHist;
    Histogram* pauseHist;
    WayPoint *previous;

    //����������� ���������� �� �����
    xml_document loadedStatistics;

public:
    Statistics(Bounds* bounds)
    {
        this->areaTree = Area::createTreeStructure(bounds);
        this->lengthHist =   new Histogram(10000, bounds->getDiagLength());
        this->velocityHist = new Histogram(5000, 50);
        this->pauseHist =    new Histogram(1000, 100000);
        this->previous = NULL;
    }

    Statistics(char* statFileName)
    {
        this->areaTree = NULL;
        this->lengthHist = NULL;
        this->velocityHist = NULL;
        this->pauseHist = NULL;
        this->previous = NULL;
        read(statFileName);
    }

    ~Statistics()
    {
        delete this->areaTree;
        delete this->lengthHist;
        delete this->velocityHist;
        delete this->pauseHist;
        if (this->previous) delete this->previous;
    }

    int getSampledPoints() {return loadedStatistics.select_node("/STATISTICS/SAMPLED-POINTS").node().text().as_int();}
    int getBase() {return loadedStatistics.select_node("/STATISTICS/EX-DX-STAT/BASE").node().text().as_int();}
    int getLevels() {return loadedStatistics.select_node("/STATISTICS/EX-DX-STAT/LEVELS").node().text().as_int();}

    vector<double>* getDXperLevel() { return parseDoubleVector(loadedStatistics.select_node("/STATISTICS/EX-DX-STAT/DX").node().text().as_string());}
    vector<double>* getEXperLevel() { return parseDoubleVector(loadedStatistics.select_node("/STATISTICS/EX-DX-STAT/EX").node().text().as_string());}

    double getRealDX(int level)
    {
        if (level < 0 || level > getLevels()) {return -1;}
        vector<double>* dxPerLevel = getDXperLevel();
        double dx = (*dxPerLevel)[level];
        delete dxPerLevel;
        return dx;
    }

    void printParams() { this->areaTree->getBounds()->print();}

    bool addPoint(WayPoint* point)
    {
        bool success = areaTree->putInArea(point);

        double pause = point->pauseDuration();
        if (pause < 0)
        {
            cout << endl << "\t" << "Statistics addPoint(): unexpected magnitude of value: " << endl;
            cout << "\t\tpause= " << pause << endl;
            cout << "\t\tpoint:  ";  point->print();
            exit(334);
        //} else if (pause == 0) {
        //    cout << endl << "\t" << "Statistics addPoint(): WARNING: pause time is zero" << endl;
        //    cout << "\t\tpoint:  ";  point->print(); cout << endl;
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
//                exit(335);
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
        if (this->previous) delete this->previous;
        this->previous = NULL;
    }

    void write(char* statFileName)
    {
        ofstream* statFile = new ofstream(statFileName);
        if (!statFile)
        {
            cout << endl << "\t" << "Statistics write(): Output file " << statFileName << " opening failed." << endl;
            exit(333);
        }
        (*statFile) << "<?xml version=\'1.0' ?>" << endl;
        (*statFile) << "<STATISTICS>" << endl;
        (*statFile) << "  <SAMPLED-POINTS>" << areaTree->getN() << "</SAMPLED-POINTS>" << endl;
        writeAreaStatistics(statFile, areaTree);
        this->lengthHist->truncate();
        this->velocityHist->truncate();
        this->pauseHist->truncate();
        writeHistogramStatistics(statFile, "FLIGHT-LENGTH-HISTOGRAM",   this->lengthHist);
        writeHistogramStatistics(statFile, "VELOCITY-HISTOGRAM", this->velocityHist);
        writeHistogramStatistics(statFile, "PAUSE-HISTOGRAM",    this->pauseHist);
        (*statFile) << "</STATISTICS>" << endl;
        statFile->close();
        delete statFile;
    }

private:
    void writeAreaStatistics(ofstream* out, Area* rootArea)
    {
        double b, c, H;
        double** ExDxPerLevel = Area::computeExDxH(rootArea, b, c, H);
        double areasCount = Area::getSubAreasCount();
        int levels = Area::getLevels();
        cout << endl << endl;
        cout << "\t<EX-DX-STAT>" << endl;
        *out << "  <EX-DX-STAT>" << endl;
        *out << "    <BASE>" << areasCount << "</BASE>" << endl;
        *out << "    <LEVELS>" << levels << "</LEVELS>" << endl;
        *out << "    <EX>";
        for(int l = 0; l < levels; l++)
        {
            cout << "\t" << "Level= " << (l+1) << "  areas= " << areasCount << "\tEX=" << ExDxPerLevel[0][l] << "\tDX=" << ExDxPerLevel[1][l] << endl;
            areasCount *= areasCount;
            *out << ExDxPerLevel[0][l];
            if (l != levels - 1) *out << "  ";
        }
        *out << "</EX>" << endl;
        *out << "    <DX>";
        for(int l = 0; l < levels; l++)
        {
            *out << ExDxPerLevel[1][l];
            if (l != levels - 1) *out << "  ";
        }
        *out << "</DX>" << endl;
        *out << "    <RECTANGLE-X>";
        for(int l = 0; l < levels; l++)
        {
            *out << ExDxPerLevel[2][l];
            if (l != levels - 1) *out << "  ";
        }
        *out << "</RECTANGLE-X>" << endl;
        *out << "    <RECTANGLE-Y>";
        for(int l = 0; l < levels; l++)
        {
            *out << ExDxPerLevel[3][l];
            if (l != levels - 1) *out << "  ";
        }
        *out << "</RECTANGLE-Y>" << endl;
        *out << "    <B>" << b << "</B>" << endl;
        *out << "    <C>" << c << "</C>" << endl;
        *out << "    <H>" << H << "</H>" << endl;
        *out << "  </EX-DX-STAT>" << endl;
        cout << "\t</EX-DX-STAT>" << endl;
        delete ExDxPerLevel[0];
        delete ExDxPerLevel[1];
        delete ExDxPerLevel[2];
        delete ExDxPerLevel[3];
        delete ExDxPerLevel;
    }

    void writeHistogramStatistics(ofstream* out, char* tag, Histogram* hist)
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

    void read(char* statFileName)
    {
        ifstream* statFile = new ifstream(statFileName);
        if (!statFile)
        {
            cout << endl << "\t" << "Statistics read(): Input file " << statFileName << " opening failed." << endl;
            exit(398);
        }

        xml_parse_result result = loadedStatistics.load(*statFile);

        if (!result)
        {
            cout << "XML [" << statFileName << "] parsed with errors: \n";
            cout << "\tError description: " << result.description() << "\n";
            cout << "\tError offset: " << result.offset << " (error at [..." << (statFileName + result.offset) << "]\n\n";
        }
        statFile->close();
        delete statFile;
    }
};

#endif // STATISTICS_H_INCLUDED
