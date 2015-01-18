/*
 * LeavyWalkMobility.cpp
 *
 *  Created on: 02 окт. 2014 г.
 *      Author: Alexander
 */

#include <LeavyWalkMobility.h>

Define_Module(LeavyWalkMobility);

LeavyWalkMobility::LeavyWalkMobility() {
}

LeavyWalkMobility::~LeavyWalkMobility() {
}

void LeavyWalkMobility::initialize(int stage) {
    initializeStatisticsFile();

    LineSegmentsMobilityBase::initialize(0);
    LineSegmentsMobilityBase::initialize(1);

    c_lenght = par("c_lenght");
    c_pause = par("c_pause");
    alpha = par("alpha");
    betta = par("betta");

    tauL = par("tauL");
    tauP = par("tauP");

    k = par("k");
    ro = par("ro");

    stageOfFlight = 0;

    direction = 0;
    lenght = 0;
    flightTime = 0;
    pauseTime = 0;

    cout << "initializing LeavyWalkMobility stage: " << stage << endl//    EV_TRACE
                    << "           c_lenght = " << c_lenght << endl
                    << "            c_pause = " << c_pause << endl
                    << "              alpha = " << alpha << endl
                    << "              betta = " << betta << endl
                    << "               tauL = " << tauL << endl
                    << "               tauP = " << tauP << endl
                    << "                  k = " << k << endl
                    << "                 ro = " << ro << endl;
}

void LeavyWalkMobility::finish() {
    closeStatisticsFile();
}

void LeavyWalkMobility::setTargetPosition() {
    static int count = 0;
    cout << "count " << count++ << endl;
    if (count == 2) {
        throw 20;
    }

    collectStatistics(2, 2, simTime(), simTime(), simTime()); //todo REMOVE

    if (stageOfFlight == 0) {
        generateNextStep();
        setFlight();
        stageOfFlight = 1;

        collectStatistics(1, 1, simTime(), simTime(), simTime()); //todo REMOVE

    } else if (stageOfFlight == 1) {
        setPause();
        stageOfFlight = 0;

        collectStatistics(0, 0, simTime(), simTime(), simTime()); //todo make collection
    }
}

void LeavyWalkMobility::generateNextStep() {
    bool correctStep = false;

    while (!correctStep) {
        direction = generateDirection();
        lenght = generateLenght();
        flightTime = generateFlightTime(lenght);
        pauseTime = generatePauseTime();

        correctStep = isCorrectLeavyValue(lenght, tauL)
                && isCorrectLeavyValue(pauseTime, tauP);
    }
}

void LeavyWalkMobility::setFlight() {
    targetPosition.x = lastPosition.x + lenght * cos(direction);
    targetPosition.y = lastPosition.y + lenght * sin(direction);
    nextChange += (lastUpdate + flightTime);
}

void LeavyWalkMobility::setPause() {
    targetPosition = lastPosition;
    nextChange += (lastUpdate + pauseTime);
}

double LeavyWalkMobility::generateDirection() {
    return uniform(0, 2 * PI);
}

simtime_t LeavyWalkMobility::generateFlightTime(double flyLenght) {
    return k * pow(flyLenght, 1 - ro);
}

double LeavyWalkMobility::generateLenght() {
    return generateLeavyRandomValue(c_lenght, alpha);
}

simtime_t LeavyWalkMobility::generatePauseTime() {
    return generateLeavyRandomValue(c_pause, betta);
}

double LeavyWalkMobility::generateLeavyRandomValue(double normCoef,
        double powCoef) {

    double sum = 0.5; // т.к. распределение симметричное, то будем рассматривать только правую часть
    double p = uniform(0, 1);

    double x = 0;
    int index = 1;

    double h = 0.001;

    do {
        if (sum >= p)
            break;
        x += index * h;
        sum += (pow(x, -1 - powCoef) * h);
    } while (true);

    if (sum < 0.5) {
        return -1; //если так, то считаем что любое число меньшее нулЯ,
                   //какое конкретно - не имеет значениЯ
    } else if (sum == 0.5) {
        return 0;
    }

    return x;
}

bool LeavyWalkMobility::isCorrectLeavyValue(simtime_t value,
        simtime_t trucateFactor) {

    return isCorrectLeavyValue(value.dbl(), trucateFactor.dbl());
}

bool LeavyWalkMobility::isCorrectLeavyValue(double value,
        double trucateFactor) {

    if (value < 0 || value > trucateFactor) {
        return false;
    }

    return true;
}

//-------------------------- Statistic collection ---------------------------------

void LeavyWalkMobility::initializeStatisticsFile() {
//    char outFileName[256];
//    DevelopmentHelper *helper = new DevelopmentHelper();

//    char *fileName = helper->createFileName(outFileName, "",
//            par("traceFileName").stringValue(), (int) ((par("fileSuffix"))));

    out = new ofstream("mobility_results.txt");

    (*out) << "x  y  appearenceTime  waitTime  flightTime" << endl << endl;
}

void LeavyWalkMobility::collectStatistics(double x, double y,
        simtime_t appearenceTime, simtime_t waitTime, simtime_t flightTime) {

    (*out) << x << "  " << y << "  " << appearenceTime.dbl() << "  "
            << waitTime.dbl() << "  " << flightTime.dbl() << endl;
}

void LeavyWalkMobility::closeStatisticsFile() {
    out->close();
}
