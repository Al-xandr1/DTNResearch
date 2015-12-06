#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "LevyHotSpotsLATP.h"

#define TRACE_TYPE ".txt"
#define WAYPOINTS_TYPE ".wpt"

Define_Module(LevyHotSpotsLATP);

LevyHotSpotsLATP::LevyHotSpotsLATP() {
    isPause = false;
    step = 0;
    jump = NULL;
    pause = NULL;
    kForSpeed = 1;
    roForSpeed = 0;

    currentHSindex = -1;

    movementsFinished = false;

    angle = -1;
    distance = -1;
    speed = -1;
    travelTime = 0;

    hsc=NULL;
    hsd=NULL;

    powA=2.0;

    waitTime = 0;
}

void LevyHotSpotsLATP::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == 0) { stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0; }

    if (hasPar("ciJ") && hasPar("aliJ") && hasPar("aciJ") && hasPar("ciP") && hasPar("aliP") && hasPar("aciP") && hasPar("powA")) {

        double ciJ  = par("ciJ").doubleValue();
        double aliJ = par("aliJ").doubleValue();
        double aciJ = par("aciJ").doubleValue();

        double ciP  = par("ciP").doubleValue();
        double aliP = par("aliP").doubleValue();
        double aciP = par("aciP").doubleValue();

        if (jump == NULL || pause == NULL) {
            jump  = new LeviJump(ciJ, aliJ, aciJ);
            pause = new LeviPause(ciP, aliP, aciP);
        }

        powA = par("powA").doubleValue();
    } else { cout << "It is necessary to specify ALL parameters for length and pause Levy distribution"; exit(-112);}

    constraintAreaMin.x = par("constraintAreaMinX").doubleValue();
    constraintAreaMax.x = par("constraintAreaMaxX").doubleValue();
    constraintAreaMin.y = par("constraintAreaMinY").doubleValue();
    constraintAreaMax.y = par("constraintAreaMaxY").doubleValue();


    if (hsc==NULL) {
        hsc = new HotSpotsCollection();
        // загрузка данных о докаци€х
        char* TracesDir = DEF_TR_DIR ;
        double minX, maxX, minY, maxY;
        hsc->readHotSpotsInfo(TracesDir, minX, maxX, minY, maxY);
        constraintAreaMin.x=minX; constraintAreaMin.y=minY;
        constraintAreaMax.x=maxX; constraintAreaMax.y=maxY;
    }
    if (hsd==NULL) {
        hsd = new HSDistanceMatrix();
        hsd->makeDistanceMatrix();
        hsd->makeProbabilityMatrix(powA);
    }

    // выбор случайной локации
    if (currentHSindex == -1) {
        currentHSindex=rand() % (hsc->HSData).size();
        currentHSMin.x=((hsc->HSData)[currentHSindex]).Xmin;
        currentHSMin.y=((hsc->HSData)[currentHSindex]).Ymin;
        currentHSMax.x=((hsc->HSData)[currentHSindex]).Xmax;
        currentHSMax.y=((hsc->HSData)[currentHSindex]).Ymax;
        currentHSCenter=(currentHSMin+currentHSMax)*0.5;
//        cout << "initialize: changing location to" << currentHSindex << endl;
    }
}

void LevyHotSpotsLATP::setInitialPosition() {
    MobilityBase::setInitialPosition();
    
    lastPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
    lastPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
    log();
    if (!isCorrectCoordinates(lastPosition.x, lastPosition.y)) exit(-555);
}

bool LevyHotSpotsLATP::isHotSpotEmpty() {
    return currentHSMin.x == currentHSMax.x || currentHSMin.y == currentHSMax.y;
}

void LevyHotSpotsLATP::finish() {
    saveStatistics();
}

void LevyHotSpotsLATP::setTargetPosition() {
    if (!movementsFinished) {
        step++;
        if (isPause) {
            waitTime = (simtime_t) pause->get_Levi_rv();
            nextChange = simTime() + waitTime;
//            log();
            if (!isCorrectCoordinates(lastPosition.x, lastPosition.y)) exit(-666);
        } else {
            if (!isCorrectCoordinates(lastPosition.x, lastPosition.y)) exit(-777);
            collectStatistics(simTime() - waitTime, simTime(), lastPosition.x, lastPosition.y);
            generateNextPosition(targetPosition, nextChange);
//            log();
            if (!isCorrectCoordinates(targetPosition.x, targetPosition.y)) exit(-888);
        }
        isPause = !isPause;
    } else {
        // остановка перемещений по документации
        nextChange = -1;
        log();
    }
}

void LevyHotSpotsLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    
    // генерируем прыжок Ћеви как обычно
    angle = uniform(0, 2 * PI);
    distance = jump->get_Levi_rv();
    speed = kForSpeed * pow(distance, 1 - roForSpeed);
    Coord delta(distance * cos(angle), distance * sin(angle), 0);
    deltaVector = delta;
    travelTime = distance / speed;

    targetPosition = lastPosition + delta;
    nextChange = simTime() + travelTime;

    // если вышли за пределы локации
    if (currentHSMin.x >= targetPosition.x || targetPosition.x >= currentHSMax.x || currentHSMin.y >= targetPosition.y || targetPosition.y >= currentHSMax.y) {
        if (isHotSpotEmpty()) {
//            cout << "HotSpot is empty! select next" << endl;
            if ( findNextHotSpot() ) {   // нашли следующую локацию - идЄм в еЄ случайную точку
                targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
                targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);
            } else movementsFinished = true;  // не нашли - останавливаемс€
            return;
        }

        // дл€ ускорени€ вычислений определ€ем вспомогательные переменные
        double x, y, Xdir, Ydir, dir;
        bool flag = ( (y=getLastPosition().y) < currentHSCenter.y);

        // выбираем самую дальнюю от текущей позиции вершину пр€моугольника текущей локации 
        // и вычисл€ем координаты вектора из текущей позиции в эту вершину
        if ( (x=getLastPosition().x) < currentHSCenter.x ) {
            if (flag) { Xdir=currentHSMax.x-x; Ydir=currentHSMax.y-y; }
            else      { Xdir=currentHSMax.x-x; Ydir=currentHSMin.y-y; }
        } else {
            if (flag) { Xdir=currentHSMin.x-x; Ydir=currentHSMax.y-y; }
            else      { Xdir=currentHSMin.x-x; Ydir=currentHSMin.y-y; }
        }

        // провер€ем, можем ли остатьс€ в пр€моугольнике текущей локации, если прыгать к дальнему углу пр€моугольника
        if ( distance <= (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
            // можем - прыгаем
            delta.x = Xdir * distance/dir;
            delta.y = Ydir * distance/dir;
            targetPosition = getLastPosition() + delta;
        } else { // не можем - надо переходить в другую локацию
            if ( findNextHotSpot() ) {   // нашли следующую локацию - идЄм в еЄ случайную точку
               targetPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
               targetPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
            } else movementsFinished = true;  // не нашли - останавливаемс€
        }
    }
}

bool LevyHotSpotsLATP::findNextHotSpot()
{
    // выбираем новую локацию
    double rn, pr=0;
    rn=(double)rand()/RAND_MAX;
    for(unsigned int i=0; i<(hsc->HSData).size(); i++) {
        if(i != currentHSindex ) pr+=(hsd->ProbabilityMatrix)[currentHSindex][i];
        if(rn <= pr) {currentHSindex=i; break; }
    }
    currentHSMin.x=((hsc->HSData)[currentHSindex]).Xmin;
    currentHSMin.y=((hsc->HSData)[currentHSindex]).Ymin;
    currentHSMax.x=((hsc->HSData)[currentHSindex]).Xmax;
    currentHSMax.y=((hsc->HSData)[currentHSindex]).Ymax;
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;

//    cout << "findNextHotSpot: changing location to" << currentHSindex << endl;
    return true;
}


void LevyHotSpotsLATP::move() {
    LineSegmentsMobilityBase::move();
}

//-------------------------- Statistic collection ---------------------------------
void LevyHotSpotsLATP::collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y) {
    inTimes.push_back(inTime);
    outTimes.push_back(outTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
}

void LevyHotSpotsLATP::saveStatistics() {
    char outFileName[256];

    char *fileName = NULL;
    if (par("wayPointFormat").boolValue()) {
        fileName = createFileName(outFileName, 0,
                    par("traceFileName").stringValue(), (int) ((par("fileSuffix"))), WAYPOINTS_TYPE);
    } else {
        fileName = createFileName(outFileName, 0,
                    par("traceFileName").stringValue(), (int) ((par("fileSuffix"))), TRACE_TYPE);
    }

    ofstream* file = new ofstream(fileName);
    for (unsigned int i = 0; i < outTimes.size(); i++) {
        simtime_t inTime = inTimes[i];
        double x = xCoordinates[i];
        double y = yCoordinates[i];

        if (par("wayPointFormat").boolValue()) {
            simtime_t outTime = outTimes[i];
            (*file) << x << "\t" << y << "\t" << inTime << "\t" << outTime << endl;
        } else {
            (*file) << inTime << "\t" << x << "\t" << y << endl;
        }
    }

    file->close();
    delete file;
}

bool LevyHotSpotsLATP::isCorrectCoordinates(double x, double y) {
    if (currentHSMin.x <= x && x <= currentHSMax.x && currentHSMin.y <= y && y <= currentHSMax.y) return true;
    cout << "------------- ERROR! -------------" << endl;
    log();
    return false;
}

void LevyHotSpotsLATP::log() {
    cout << "-------------------------------------------------------------" << endl;
    cout << "step = " << step << ", isPause = " << isPause << endl;
    cout << "simTime() = " << simTime() << endl;
    cout << "lastPosition = " << lastPosition << endl;

    cout << "currentHSindex = " << currentHSindex << endl;
    cout << "\t currentHSMin.x = " << currentHSMin.x << ", currentHSMax.x = " << currentHSMax.x << endl;
    cout << "\t currentHSMin.y = " << currentHSMin.y << ", currentHSMax.y = " << currentHSMax.y << endl;
    cout << "\t currentHSCenter.x = " << currentHSCenter.x << ", currentHSCenter.y = " << currentHSCenter.y << endl;
    ((hsc->HSData)[currentHSindex]).print();

    if (isPause) {
        cout << "waitTime = " << waitTime << endl;
    } else {
        cout << "distance = " << distance << ", angle = " << angle << ", speed = " << speed << endl;
        cout << "deltaVector = " << deltaVector << ", travelTime = " << travelTime << endl;
    }

    cout << "targetPosition = " << targetPosition << endl;
    cout << "nextChange = " << nextChange << endl;

    cout << "movementsFinished = " << movementsFinished << endl;
    cout << "-------------------------------------------------------------" << endl << endl;
}
