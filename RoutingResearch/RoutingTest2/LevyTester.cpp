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

    flightLengthPDF = new cDoubleHistogram("FLIGHT-LENGTH-HISTOGRAM", 10000);
    flightLengthPDF->setRange(0.0, maxPermittedDistance);

    velocityPDF = new cDoubleHistogram("VELOCITY-HISTOGRAM", 5000);
    velocityPDF->setRange(0.0, 50);

    pausePDF = new cDoubleHistogram("PAUSE-HISTOGRAM", 1000);
    pausePDF->setRange(0.0, 100000);

    // генерируем выборку заданного объёма selectionVolume и формируем гистограмму
    for(unsigned int i = 0; i < selectionVolume; i++) {
        bool success = mvnt->genFlight("LevyTester::initialize");
        ASSERT(success);
        const double flightLength = mvnt->getDistance();
        flightLengthPDF->collect(flightLength);

        const double velocity = mvnt->getSpeed();
        velocityPDF->collect(velocity);

        success = mvnt->genPause("LevyTester::initialize");
        ASSERT(success);
        const simtime_t pause = mvnt->getWaitTime();
        pausePDF->collect(pause);
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
    if (!flightLengthPDF->isTransformed()) flightLengthPDF->transform();
    if (!velocityPDF->isTransformed()) velocityPDF->transform();
    if (!pausePDF->isTransformed()) pausePDF->transform();
    writeHistogramStatistics(statFile, (char*) "FLIGHT-LENGTH-HISTOGRAM", flightLengthPDF);
    writeHistogramStatistics(statFile, (char*) "VELOCITY-HISTOGRAM", velocityPDF);
    writeHistogramStatistics(statFile, (char*) "PAUSE-HISTOGRAM", pausePDF);
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
void LevyTester::writeHistogramStatistics(ofstream* out, char* tag, cDoubleHistogram* hist) {
    long double checkSum = 0; // todo это работает неправильно
    for(unsigned int i = 0; i < hist->getNumCells(); i++) {
        checkSum += hist->getPDF(i);
    }

    cout << "\t<" << tag << " checkSum=\"" << checkSum
                         << "\" totalValues=\"" << hist->getCount()
                         << "\" underflowValues=\"" << hist->getUnderflowCell()
                         << "\" overflowValues=\"" << hist->getOverflowCell() << "\">"<< endl;
    *out << "  <" << tag << " checkSum=\"" << checkSum
                         << "\" totalValues=\"" << hist->getCount()
                         << "\" underflowValues=\"" << hist->getUnderflowCell()
                         << "\" overflowValues=\"" << hist->getOverflowCell() << "\">"<< endl;

    //todo слишком большое количество ячеек (хвост может содержать много нулей)
    cout << "\t  <CELLS>" << hist->getNumCells() << "</CELLS>" << endl;
    *out << "    <CELLS>" << hist->getNumCells() << "</CELLS>" << endl;

    //todo ширина ячейки расходиться с шириной для длины от программы WaypointFinder
    cout << "\t  <CELL-WIDTH>" << hist->getCellSize() << "</CELL-WIDTH>" << endl;
    *out << "    <CELL-WIDTH>" << hist->getCellSize() << "</CELL-WIDTH>" << endl;
    cout << "\t  <LEFT-BOUND>" << hist->getBasepoint(0) << "</LEFT-BOUND>" << endl;
    *out << "    <LEFT-BOUND>" << hist->getBasepoint(0) << "</LEFT-BOUND>" << endl;
    cout << "\t  <RIGHT-BOUND>" << hist->getMax() << "</RIGHT-BOUND>" << endl;
    *out << "    <RIGHT-BOUND>" << hist->getMax() << "</RIGHT-BOUND>" << endl;

    //todo значения PDF, CDF, CCDF сильно расходятся со старой логикой. Попробовать использовать CDF "родную"

    cout << "\t  <PDF-VALS>" << endl;
    *out << "    <PDF-VALS>" << endl;
    vector<double>* pdf = toPDFVector(hist);
    for (int i = 0; i < pdf->size(); i++) {
        *out << (*pdf)[i]; if (i != pdf->size()-1) *out << "  "; else *out << endl;
    }
    cout << "\t  </PDF-VALS>" << endl;
    *out << "    </PDF-VALS>" << endl;

    cout << "\t  <CDF-VALS>" << endl;
    *out << "    <CDF-VALS>" << endl;
    vector<double>* cdf = toCDFVector(hist);
    for (int i = 0; i < cdf->size(); i++) {
        *out << (*cdf)[i]; if (i != cdf->size()-1) *out << "  "; else *out << endl;
    }
    cout << "\t  </CDF-VALS>" << endl;
    *out << "    </CDF-VALS>" << endl;

    cout << "\t  <CCDF-VALS>" << endl;
    *out << "    <CCDF-VALS>" << endl;
    vector<double>* ccdf = toCCDFVector(hist);
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

vector<double>* LevyTester::toPDFVector(cDoubleHistogram* hist) {
    vector<double>* pdf = new vector<double>();
    for (int i = 0; i < hist->getNumCells(); i++)
        pdf->push_back(hist->getCellPDF(i));
    return pdf;
}

vector<double>* LevyTester::toCDFVector(cDoubleHistogram* hist) {
    vector<double>* cdf = new vector<double>();
    vector<double>* pdf = toPDFVector(hist);
    for (unsigned int i = 0; i < pdf->size(); i++)
    {
        double val = 0;
        for (unsigned int j = 0; j <= i; j++) val += pdf->at(j);
        cdf->push_back(val);
    }
    delete pdf;
    return cdf;
}

vector<double>* LevyTester::toCCDFVector(cDoubleHistogram* hist) {
    vector<double>* ccdf = new vector<double>();
    vector<double>* cdf = toCDFVector(hist);
    for (unsigned int i = 0; i < cdf->size(); i++)
        ccdf->push_back(1 - cdf->at(i));
    delete cdf;
    return ccdf;
}

