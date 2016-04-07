#include "SimpleLevyMobility.h"

#define TRACE_TYPE ".txt"

Define_Module(SimpleLevyMobility);

SimpleLevyMobility::SimpleLevyMobility() {
    nextMoveIsWait = false;
    jump = NULL;
    pause = NULL;
    kForSpeed = 1;
    roForSpeed = 0;

    movementsFinished = false;
}

void SimpleLevyMobility::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == 0) { stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0; }

    if (hasPar("ciJ") && hasPar("aliJ") && hasPar("aciJ") && hasPar("ciP") && hasPar("aliP") && hasPar("aciP")) {

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

    } else { cout << "It is necessary to specify ALL parameters for length and pause Levy distribution"; exit(-112);}

    // начальна€ локаци€ - всЄ поле
    currentHSMin=getConstraintAreaMin();
    currentHSMax=getConstraintAreaMax();
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;

    // TODO
    // par("powA").doubleValue()
}



void SimpleLevyMobility::setInitialPosition() {
    MobilityBase::setInitialPosition();

    lastPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
    lastPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
}

void SimpleLevyMobility::finish() {
    saveStatistics();
}

void SimpleLevyMobility::setTargetPosition() {
    if (!movementsFinished) {
        if (nextMoveIsWait) {
            simtime_t waitTime = (simtime_t) pause->get_Levi_rv();
            nextChange = simTime() + waitTime;
        } else {
            collectStatistics(simTime(), lastPosition.x, lastPosition.y);
            generateNextPosition(targetPosition, nextChange);
        }
        nextMoveIsWait = !nextMoveIsWait;
    } else {
        // планирование в бесконечность - костыльна€ остановка перемещений
        nextChange = simTime() + 100000;
    }
}

void SimpleLevyMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    
    // генерируем прыжок Ћеви как обычно
    const double angle = uniform(0, 2 * PI);
    const double distance = jump->get_Levi_rv();
    const double speed = kForSpeed * pow(distance, 1 - roForSpeed);
    Coord delta(distance * cos(angle), distance * sin(angle), 0);
    simtime_t travelTime = distance / speed;

    targetPosition = lastPosition + delta;
    nextChange = simTime() + travelTime;

    // если вышли за пределы локации
    if (currentHSMin.x > targetPosition.x || targetPosition.x > currentHSMax.x || currentHSMin.y > targetPosition.y || targetPosition.y > currentHSMax.y) {
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

void SimpleLevyMobility::move() {
    LineSegmentsMobilityBase::move();
}

//-------------------------- Statistic collection ---------------------------------
void SimpleLevyMobility::collectStatistics(simtime_t appearenceTime, double x, double y) {
    times.push_back(appearenceTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
}

void SimpleLevyMobility::saveStatistics() {
    char outFileName[256];
    char *fileName = createFileName(outFileName, 0, par("traceFileName").stringValue(),
            (int) ((par("fileSuffix"))), TRACE_TYPE);

    ofstream* file = new ofstream(fileName);
    for (int i = 0; i < times.size(); i++) {
        simtime_t time = times[i];
        double x = xCoordinates[i];
        double y = yCoordinates[i];
        (*file) << time << "\t" << x << "\t" << y << endl;
    }

    file->close();
    delete file;
}
