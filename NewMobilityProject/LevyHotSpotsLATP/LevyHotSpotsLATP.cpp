#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "LevyHotSpotsLATP.h"

#define TRACE_TYPE ".txt"
#define WAYPOINTS_TYPE ".wpt"

Define_Module(LevyHotSpotsLATP);

LevyHotSpotsLATP::LevyHotSpotsLATP() {
    nextMoveIsWait = false;
    steps = 0;
    jump = NULL;
    pause = NULL;
    kForSpeed = 1;
    roForSpeed = 0;

    movementsFinished = false;

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


    if (hsc==NULL) hsc = new HotSpotsCollection();
    if (hsd==NULL) hsd = new HSDistanceMatrix();

    // загрузка данных о докациях
    char* TracesDir = DEF_TR_DIR ;
    double minX, maxX, minY, maxY;
    hsc->readHotSpotsInfo(TracesDir, minX, maxX, minY, maxY);
    constraintAreaMin.x=minX; constraintAreaMin.y=minY;
    constraintAreaMax.x=maxX; constraintAreaMax.y=maxY;

    hsd->makeDistanceMatrix();
    hsd->makeProbabilityMatrix(powA);

    // выбор случайной локации
    currentHSindex=rand() % (hsc->HSData).size();
    currentHSMin.x=((hsc->HSData)[currentHSindex]).Xmin;
    currentHSMin.y=((hsc->HSData)[currentHSindex]).Ymin;
    currentHSMax.x=((hsc->HSData)[currentHSindex]).Xmax;
    currentHSMax.y=((hsc->HSData)[currentHSindex]).Ymax;
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;
}

void LevyHotSpotsLATP::setInitialPosition() {
    MobilityBase::setInitialPosition();
    
    lastPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
    lastPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
    if (!isCorrectCoordinates(lastPosition.x, lastPosition.y)) exit(-666);
}

void LevyHotSpotsLATP::finish() {
    saveStatistics();
}

void LevyHotSpotsLATP::setTargetPosition() {
    if (!movementsFinished) {
        steps++;
        if (nextMoveIsWait) {
            waitTime = (simtime_t) pause->get_Levi_rv();
            nextChange = simTime() + waitTime;
        } else {
            if (!isCorrectCoordinates(lastPosition.x, lastPosition.y)) exit(-777);
            collectStatistics(simTime() - waitTime, simTime(), lastPosition.x, lastPosition.y);
            generateNextPosition(targetPosition, nextChange);
            if (!isCorrectCoordinates(targetPosition.x, targetPosition.y)) exit(-888);
        }
        nextMoveIsWait = !nextMoveIsWait;
    } else {
        // планирование в бесконечность - костыльная остановка перемещений
        nextChange = simTime() + 100000;
    }
}

void LevyHotSpotsLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    
    // генерируем прыжок Леви как обычно
    const double angle = uniform(0, 2 * PI);
    const double distance = jump->get_Levi_rv();
    const double speed = kForSpeed * pow(distance, 1 - roForSpeed);
    Coord delta(distance * cos(angle), distance * sin(angle), 0);
    simtime_t travelTime = distance / speed;

    targetPosition = lastPosition + delta;
    nextChange = simTime() + travelTime;

    // если вышли за пределы локации
    if (currentHSMin.x > targetPosition.x || targetPosition.x > currentHSMax.x || currentHSMin.y > targetPosition.y || targetPosition.y > currentHSMax.y) {
	// для ускорения вычислений определяем вспомогательные переменные
        double x, y, Xdir, Ydir, dir;
        bool flag = ( (y=getLastPosition().y) < currentHSCenter.y);

        // выбираем самую дальнюю от текущей позиции вершину прямоугольника текущей локации 
        // и вычисляем координаты вектора из текущей позиции в эту вершину
        if ( (x=getLastPosition().x) < currentHSCenter.x ) {
            if (flag) { Xdir=currentHSMax.x-x; Ydir=currentHSMax.y-y; }
            else      { Xdir=currentHSMax.x-x; Ydir=currentHSMin.y-y; }
        } else {
            if (flag) { Xdir=currentHSMin.x-x; Ydir=currentHSMax.y-y; }
            else      { Xdir=currentHSMin.x-x; Ydir=currentHSMin.y-y; }
        }

        // проверяем, можем ли остаться в прямоугольнике текущей локации, если прыгать к дальнему углу прямоугольника
        if ( distance <= (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
            // можем - прыгаем
	    delta.x = Xdir * distance/dir;
            delta.y = Ydir * distance/dir;
            targetPosition = getLastPosition() + delta;
        } else { // не можем - надо переходить в другую локацию
            if ( findNextHotSpot() ) {   // нашли следующую локацию - идём в её случайную точку
               targetPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
               targetPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
            } else movementsFinished = true;  // не нашли - останавливаемся
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

    //    cout << "changing location to" << currentHSindex << endl;
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

bool LevyHotSpotsLATP::isCorrectCoordinates(double x, double y) {
    if (currentHSMin.x <= x && x <= currentHSMax.x && currentHSMin.y <= y && y <= currentHSMax.y) {
        return true;
    }
    cout << "currentHSMin.x = " << currentHSMin.x << ", currentHSMax.x = " << currentHSMax.x << endl;
    cout << "currentHSMin.y = " << currentHSMin.y << ", currentHSMax.y = " << currentHSMax.y << endl;
    cout << " x = " << x << ", y = " << y << endl;
    cout << " steps = " << steps << endl;

    return false;
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
