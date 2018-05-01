#include "SimpleLevyMobility.h"

Define_Module(SimpleLevyMobility);

SimpleLevyMobility::SimpleLevyMobility() {
    NodeID = -1;

    // начинаем маршрут с паузы, чтобы мы "нормально прошли" первую точку (например посто€ли в ней)
    // а не так, чтобы при инициализации маршрута мы еЄ поставили и при первой генерации сразу выбрали новую
    isPause = true;

    // первый шаг нулевой. ƒалее на нЄм провер€ем, что мы прошли инициализацию,
    // и реально начали ходить (начина€ с первого шага)
    step = 0;
    movementsFinished = false;
    movement = NULL;

    currentHSMin = Coord::ZERO;
    currentHSMax = Coord::ZERO;
    currentHSCenter = Coord::ZERO;
    currentHSindex = -1;

    mvnHistory = NULL;
}

void SimpleLevyMobility::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == 0) {
        stationary = false;
        NodeID = (int) par("NodeID");

        double ciJ,aliJ,deltaXJ,joinJ, ciP,aliP,deltaXP,joinP;
        double kForSpeed, roForSpeed, maxPermittedDistance;
        maxPermittedDistance = (constraintAreaMax - constraintAreaMin).length();

        if (hasPar("ciJ") && hasPar("aliJ") && hasPar("deltaXJ") && hasPar("joinJ")
                && hasPar("ciP") && hasPar("aliP") && hasPar("deltaXP") && hasPar("joinP")) {
            ciJ  = par("ciJ").doubleValue();
            aliJ = par("aliJ").doubleValue();
            deltaXJ = par("deltaXJ").doubleValue();
            joinJ = par("joinJ").doubleValue();

            ciP  = par("ciP").doubleValue();
            aliP = par("aliP").doubleValue();
            deltaXP = par("deltaXP").doubleValue();
            joinP = par("joinP").doubleValue();
        } else { cout << "It is necessary to specify ALL parameters for length and pause Levy distribution"; exit(-112);}


        if (hasPar("kForSpeed") && hasPar("roForSpeed")) {
            kForSpeed = par("kForSpeed").doubleValue();
            roForSpeed = par("roForSpeed").doubleValue();
        } else { cout << "It is necessary to specify ALL parameters for speed function"; exit(-212);}

        ASSERT(!movement);
        movement = new Movement(kForSpeed,
                                roForSpeed,
                                maxPermittedDistance,
                                new LeviJump(ciJ, aliJ, deltaXJ, joinJ),
                                new LeviPause(ciP, aliP, deltaXP, joinP));

        // начальна€ локаци€ - всЄ поле
        currentHSMin=getConstraintAreaMin();
        currentHSMax=getConstraintAreaMax();
        currentHSCenter=(currentHSMin+currentHSMax)*0.5;

        ASSERT(!mvnHistory);
        mvnHistory = new MovementHistory(NodeID);
    }
}

void SimpleLevyMobility::setInitialPosition() {
    MobilityBase::setInitialPosition();

    lastPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
    lastPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
    targetPosition = lastPosition;
    ASSERT(isCorrectCoordinates(lastPosition.x, lastPosition.y));
}

bool SimpleLevyMobility::isHotSpotEmpty() {
    return currentHSMin.x == currentHSMax.x || currentHSMin.y == currentHSMax.y;
}

void SimpleLevyMobility::finish() {
    saveStatistics();
}

void SimpleLevyMobility::setTargetPosition() {
    if (movementsFinished) {nextChange = -1; return;};
    ASSERT(isCorrectCoordinates(lastPosition.x, lastPosition.y));
    ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));

    step++;
    if (isPause) {
        const bool success = movement->genPause( (string("DEBUG SimpleLevyMobility::setTargetPosition: NodeId = ") + std::to_string(NodeID)).c_str() );
        ASSERT(success);
        nextChange = simTime() + movement->getWaitTime();
    } else {
        mvnHistory->collect(simTime() - movement->getWaitTime(), simTime(), lastPosition.x, lastPosition.y);
        movementsFinished = !generateNextPosition(targetPosition, nextChange);

        if (movementsFinished) {nextChange = -1; return;};
        ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));
    }
    isPause = !isPause;
}

bool SimpleLevyMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    const bool success = movement->genFlight( (string("DEBUG SimpleLevyMobility::generateNextPosition: NodeId = ") + std::to_string(NodeID)).c_str() );
    ASSERT(success);
    
    targetPosition = lastPosition + movement->getDeltaVector();
    if (targetPosition == lastPosition) log();
    ASSERT(targetPosition != lastPosition);
    nextChange = simTime() + movement->getTravelTime();

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
        if ( movement->getDistance() <= (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
            // можем - прыгаем
            Coord delta;
            delta.x = Xdir * movement->getDistance()/dir;
            delta.y = Ydir * movement->getDistance()/dir;
            targetPosition = getLastPosition() + delta;
        } else { // не можем - надо переходить в другую локацию
            if ( findNextHotSpot() ) {   // нашли следующую локацию - идЄм в еЄ случайную точку
               targetPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
               targetPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
            } else return false;  // не нашли - останавливаемс€
        }
    }

    return true;
}

void SimpleLevyMobility::saveStatistics() {
    const char *outDir = NamesAndDirs::getOutDir();
    const char *wpsDir = NamesAndDirs::getOutWpsDir();
    const char *trsDir = NamesAndDirs::getOutTrsDir();

    if (NodeID == 0 ) {//чтобы записывал только один узел
        //--- Create output directories ---
        if (CreateDirectory(outDir, NULL)) cout << "create output directory: " << outDir << endl;
        else cout << "error create output directory: " << outDir << endl;

        if (CreateDirectory(wpsDir, NULL)) cout << "create output directory: " << wpsDir << endl;
        else cout << "error create output directory: " << wpsDir << endl;

        if (CreateDirectory(trsDir, NULL)) cout << "create output directory: " << trsDir << endl;
        else cout << "error create output directory: " << trsDir << endl;
    }

    //--- Write points ---
    mvnHistory->save(wpsDir, trsDir);
}

bool SimpleLevyMobility::isCorrectCoordinates(double x, double y) {
    if (currentHSMin.x <= x && x <= currentHSMax.x && currentHSMin.y <= y && y <= currentHSMax.y) return true;
    cout << "------------- ERROR! -------------" << endl;
    log();
    return false;
}

void SimpleLevyMobility::log() {  // ќтладочна€ функци€
    cout << "----------------------------- LOG --------------------------------" << endl;
    cout << "step = " << step << ", isPause = " << isPause << endl;
    cout << "simTime() = " << simTime() << endl;
    cout << "lastPosition = " << lastPosition << endl;

    cout << "currentHSindex = " << currentHSindex << endl;
    cout << "\t currentHSMin.x = " << currentHSMin.x << ", currentHSMax.x = " << currentHSMax.x << endl;
    cout << "\t currentHSMin.y = " << currentHSMin.y << ", currentHSMax.y = " << currentHSMax.y << endl;
    cout << "\t currentHSCenter.x = " << currentHSCenter.x << ", currentHSCenter.y = " << currentHSCenter.y << endl;
    cout << "\t isHotSpotEmpty = " << isHotSpotEmpty() << endl;

    if (isPause) {
        cout << "waitTime = " << movement->getWaitTime() << endl;
    } else {
        cout << "distance = " << movement->getDistance() << ", angle = " << movement->getAngle() << ", speed = " << movement->getSpeed() << endl;
        cout << "deltaVector = " << movement->getDeltaVector() << ", travelTime = " << movement->getTravelTime() << endl;
    }
    movement->log();

    cout << "targetPosition = " << targetPosition << endl;
    cout << "nextChange = " << nextChange << endl;

    cout << "movementsFinished = " << movementsFinished << endl;
    cout << "-------------------------------------------------------------" << endl << endl;
}
