#include "SimpleLevyMobility.h"

Define_Module(SimpleLevyMobility);

SimpleLevyMobility::SimpleLevyMobility() {
    NodeID = -1;

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

    powA=2.0;

    waitTime = 0;

    wpFileName = NULL;
    trFileName = NULL;
}

void SimpleLevyMobility::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    double ciJ,aliJ,aciJ, ciP,aliP,aciP;

    if (stage == 0) {
        stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0;

        constraintAreaMin.x = par("constraintAreaMinX").doubleValue();
        constraintAreaMax.x = par("constraintAreaMaxX").doubleValue();
        constraintAreaMin.y = par("constraintAreaMinY").doubleValue();
        constraintAreaMax.y = par("constraintAreaMaxY").doubleValue();

        NodeID = (int) par("NodeID");

        if (hasPar("ciJ") && hasPar("aliJ") && hasPar("aciJ") && hasPar("ciP") && hasPar("aliP") && hasPar("aciP") && hasPar("powA")) {

            ciJ  = par("ciJ").doubleValue();
            aliJ = par("aliJ").doubleValue();
            aciJ = par("aciJ").doubleValue();

            ciP  = par("ciP").doubleValue();
            aliP = par("aliP").doubleValue();
            aciP = par("aciP").doubleValue();

            powA = par("powA").doubleValue();

        } else { cout << "It is necessary to specify ALL parameters for length and pause Levy distribution"; exit(-112);}
    }

    if (jump  == NULL) jump  = new LeviJump(ciJ, aliJ, aciJ);
    if (pause == NULL) pause = new LeviPause(ciP, aliP, aciP);



    // начальная локация - всё поле
    currentHSMin=getConstraintAreaMin();
    currentHSMax=getConstraintAreaMax();
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;



    if (wpFileName == NULL && trFileName == NULL) {
        wpFileName = new char[256];
        trFileName = new char[256];
        wpFileName = createFileName(wpFileName, 0, par("traceFileName").stringValue(),
                (int) ((par("NodeID"))), WAYPOINTS_TYPE);
        trFileName = createFileName(trFileName, 0, par("traceFileName").stringValue(),
                (int) ((par("NodeID"))), TRACE_TYPE);
    }
}


int SimpleLevyMobility::getNodeID() {
    return NodeID;
}

void SimpleLevyMobility::setInitialPosition() {
    MobilityBase::setInitialPosition();

    lastPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
    lastPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
    targetPosition = lastPosition;
//    ASSERT(isCorrectCoordinates(lastPosition.x, lastPosition.y));
}

bool SimpleLevyMobility::isHotSpotEmpty() {
    return currentHSMin.x == currentHSMax.x || currentHSMin.y == currentHSMax.y;
}

void SimpleLevyMobility::finish() {
    saveStatistics();
}

void SimpleLevyMobility::setTargetPosition() {
    if (movementsFinished) {nextChange = -1; return;};
//    ASSERT(isCorrectCoordinates(lastPosition.x, lastPosition.y));
//    ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));

    step++;
    if (isPause) {
        waitTime = (simtime_t) pause->get_Levi_rv();
//        ASSERT(waitTime > 0);
        nextChange = simTime() + waitTime;
    } else {
        collectStatistics(simTime() - waitTime, simTime(), lastPosition.x, lastPosition.y);
        movementsFinished = !generateNextPosition(targetPosition, nextChange);

        if (movementsFinished) {nextChange = -1; return;};
//        ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));
    }
    isPause = !isPause;
}

bool SimpleLevyMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    
    // генерируем прыжок Леви как обычно
    angle = uniform(0, 2 * PI);
    distance = jump->get_Levi_rv();
//    ASSERT(distance > 0);
    speed = kForSpeed * pow(distance, 1 - roForSpeed);
    Coord delta(distance * cos(angle), distance * sin(angle), 0);
    deltaVector = delta;
    travelTime = distance / speed;

    targetPosition = lastPosition + delta;
//    ASSERT(targetPosition.x != lastPosition.x);
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
            } else return false;  // не нашли - останавливаемся
        }
    }

    return true;
}


//-------------------------- Statistic collection ---------------------------------
void SimpleLevyMobility::collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y) {
    inTimes.push_back(inTime);
    outTimes.push_back(outTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
}

void SimpleLevyMobility::saveStatistics() {
    char *outDir = NamesAndDirs::getOutDir();
    char *wpsDir = NamesAndDirs::getWpsDir();
    char *trsDir = NamesAndDirs::getTrsDir();

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
    ofstream wpFile(buildFullName(wpsDir, wpFileName));
    ofstream trFile(buildFullName(trsDir, trFileName));
    for (unsigned int i = 0; i < outTimes.size(); i++) {
        simtime_t inTime = inTimes[i];
        simtime_t outTime = outTimes[i];
        double x = xCoordinates[i];
        double y = yCoordinates[i];

        wpFile << x << "\t" << y << "\t" << inTime << "\t" << outTime << endl;
        trFile << inTime << "\t" << x << "\t" << y << endl;
    }

    wpFile.close();
    trFile.close();
}

bool SimpleLevyMobility::isCorrectCoordinates(double x, double y) {
    if (currentHSMin.x <= x && x <= currentHSMax.x && currentHSMin.y <= y && y <= currentHSMax.y) return true;
    cout << "------------- ERROR! -------------" << endl;
    log();
    return false;
}

void SimpleLevyMobility::log() {  // Отладочная функция
    cout << "-------------------------------------------------------------" << endl;
    cout << "step = " << step << ", isPause = " << isPause << endl;
    cout << "simTime() = " << simTime() << endl;
    cout << "lastPosition = " << lastPosition << endl;

    cout << "currentHSindex = " << currentHSindex << endl;
    cout << "\t currentHSMin.x = " << currentHSMin.x << ", currentHSMax.x = " << currentHSMax.x << endl;
    cout << "\t currentHSMin.y = " << currentHSMin.y << ", currentHSMax.y = " << currentHSMax.y << endl;
    cout << "\t currentHSCenter.x = " << currentHSCenter.x << ", currentHSCenter.y = " << currentHSCenter.y << endl;
    cout << "\t isHotSpotEmpty = " << isHotSpotEmpty() << endl;

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
