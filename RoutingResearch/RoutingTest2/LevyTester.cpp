#include <LevyTester.h>

Define_Module(LevyTester);

LevyTester::LevyTester() {
}

LevyTester::~LevyTester() {
}

void LevyTester::initialize() {
    cout << "LevyTester::initialize() is started!" << endl << endl;

    Coord constraintAreaMin, constraintAreaMax;
    constraintAreaMin.x = par("constraintAreaMinX");
    constraintAreaMin.y = par("constraintAreaMinY");
    constraintAreaMin.z = par("constraintAreaMinZ");
    constraintAreaMax.x = par("constraintAreaMaxX");
    constraintAreaMax.y = par("constraintAreaMaxY");
    constraintAreaMax.z = par("constraintAreaMaxZ");
    const double maxPermittedDistance = (constraintAreaMax - constraintAreaMin).length();

    Movement* mvnt = new Movement(this, maxPermittedDistance);

    selectionVolume = par("selectionVolume").longValue();
    cout << "LevyTester::initialize: selectionVolume = " << selectionVolume << endl;

    lengthHist =   new Histogram(10000, maxPermittedDistance);
    velocityHist = new Histogram(5000, 50);
    pauseHist =    new Histogram(1000, 100000);

    // генерируем выборку заданного объёма selectionVolume и формируем гистограмму
    for(unsigned int i = 0; i < selectionVolume; i++) {
        bool success = mvnt->genFlight("LevyTester::initialize");
        ASSERT(success);
        lengthHist->put(mvnt->getDistance());

        velocityHist->put(mvnt->getSpeed());

        success = mvnt->genPause("LevyTester::initialize");
        ASSERT(success);
        pauseHist->put(mvnt->getWaitTime().dbl());
    }

    cout << "LevyTester::initialize() is finished!" << endl << endl;
}

void LevyTester::handleMessage(cMessage *msg) {
    cout << "LevyTester::handleMessage() is started!" << endl << endl;
    cout << "LevyTester::handleMessage() is finished!" << endl << endl;
}

/*
 * Скопированный кусок кода из \DTNResearch\Utils\WaypointFinder\src\Statistics.h с исправлениями для работоспособности
 */
void LevyTester::finish() {
    cout << "LevyTester::finish() is started!" << endl << endl;

    const char* statFileName = buildFullName(OUT_DIR, STAT_STAT_FILE);
    ofstream* statFile = new ofstream(statFileName);
    if (!statFile) {
        cout << endl << "\t" << "Statistics write(): Output file " << statFileName << " opening failed." << endl;
        exit(-333);
    }
    (*statFile) << "<?xml version=\'1.0' ?>" << endl;
    (*statFile) << "<STATISTICS>" << endl;
    (*statFile) << "  <SAMPLED-POINTS>" << selectionVolume << "</SAMPLED-POINTS>" << endl;
    writeAreaStatistics(statFile);
    lengthHist->truncate();
    velocityHist->truncate();
    pauseHist->truncate();
    writeHistogramStatistics(statFile, (char*) "FLIGHT-LENGTH-HISTOGRAM", lengthHist);
    writeHistogramStatistics(statFile, (char*) "VELOCITY-HISTOGRAM", velocityHist);
    writeHistogramStatistics(statFile, (char*) "PAUSE-HISTOGRAM", pauseHist);
    (*statFile) << "</STATISTICS>" << endl;
    statFile->close();
    delete statFile;

    cout << "LevyTester::finish() is finished!" << endl << endl;
}

/*
 * Скопированный кусок кода из \DTNResearch\Utils\WaypointFinder\src\Statistics.h с исправлениями для работоспособности
 */
void LevyTester::writeAreaStatistics(ofstream* out) {
    //double** ExDxPerLevel = Area::computeExDx(rootArea);
    double areasCount = 4/*Area::getSubAreasCount()*/;
    int levels = 9/*Area::getLevels()*/;
    cout << endl << endl;
    cout << "\t<EX-DX-STAT>" << endl;
    *out << "  <EX-DX-STAT>" << endl;
    *out << "    <BASE>" << areasCount << "</BASE>" << endl;
    *out << "    <LEVELS>" << levels << "</LEVELS>" << endl;
    *out << "    <EX>";
    for(int l = 0; l < levels; l++)
    {
        cout << "\t" << "Level= " << (l+1) << "  areas= " << areasCount << "\tEX=" << 0/*ExDxPerLevel[0][l]*/ << "\tDX=" << 0/*ExDxPerLevel[1][l]*/<< endl;
        areasCount *= areasCount;
        *out << 0/*ExDxPerLevel[0][l]*/;
        if (l != levels-1) *out << "  ";
    }
    *out << "</EX>" << endl;
    *out << "    <DX>";
    for(int l = 0; l < levels; l++)
    {
        *out << 0/*ExDxPerLevel[1][l]*/;
        if (l != levels-1) *out << "  ";
    }
    *out << "</DX>" << endl;
    *out << "  </EX-DX-STAT>" << endl;
    cout << "\t</EX-DX-STAT>" << endl;
    //delete ExDxPerLevel[0];
    //delete ExDxPerLevel[1];
    //delete ExDxPerLevel;
}

/*
 * Скопированный кусок кода из \DTNResearch\Utils\WaypointFinder\src\Statistics.h с исправлениями для работоспособности
 */
void LevyTester::writeHistogramStatistics(ofstream* out, char* tag, Histogram* hist)
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
    for (int i = 0; i < pdf->size(); i++) {
        *out << (*pdf)[i]; if (i != pdf->size()-1) *out << "  "; else *out << endl;
    }
    cout << "\t  </PDF-VALS>" << endl;
    *out << "    </PDF-VALS>" << endl;

    cout << "\t  <CDF-VALS>" << endl;
    *out << "    <CDF-VALS>" << endl;
    vector<double>* cdf = hist->toCDFVector();
    for (int i = 0; i < cdf->size(); i++) {
        *out << (*cdf)[i]; if (i != cdf->size()-1) *out << "  "; else *out << endl;
    }
    cout << "\t  </CDF-VALS>" << endl;
    *out << "    </CDF-VALS>" << endl;

    cout << "\t  <CCDF-VALS>" << endl;
    *out << "    <CCDF-VALS>" << endl;
    vector<double>* ccdf = hist->toCCDFVector();
    for (int i = 0; i < ccdf->size(); i++) {
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

