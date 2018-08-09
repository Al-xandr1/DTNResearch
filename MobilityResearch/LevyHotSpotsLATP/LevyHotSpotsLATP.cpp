#include "LevyHotSpotsLATP.h"

Define_Module(LevyHotSpotsLATP);

LevyHotSpotsLATP::LevyHotSpotsLATP() {
    NodeID = -1;

    // начинаем маршрут с паузы, чтобы мы "нормально прошли" первую точку (например постояли в ней)
    // а не так, чтобы при инициализации маршрута мы её поставили и при первой генерации сразу выбрали новую
    isPause = true;

    // первый шаг нулевой. Далее на нём проверяем, что мы прошли инициализацию,
    // и реально начали ходить (начиная с первого шага)
    step = 0;
    movementsFinished = false;
    countOfFirstSkippedLongFlight = -1;
    powA=2.0;

    movement = NULL;

    currentHSMin = Coord::ZERO;
    currentHSMax = Coord::ZERO;
    currentHSCenter = Coord::ZERO;

    hsc=NULL;
    currentHSindex = -1;
    hsd=NULL;

    mvnHistory = NULL;
}


void LevyHotSpotsLATP::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == 0) {
        stationary = false;
        NodeID = (int) par("NodeID");

        double ciJ,aliJ,deltaXJ,joinJ, ciP,aliP,deltaXP,joinP;
        double kForSpeed_1, roForSpeed_1, distanceThreshold, kForSpeed_2, roForSpeed_2, maxPermittedDistance;
        maxPermittedDistance = (constraintAreaMax - constraintAreaMin).length();
        countOfFirstSkippedLongFlight = par("countOfFirstSkippedLongFlight").longValue();

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

           powA = par("powA").doubleValue();
        } else { cout << "It is necessary to specify ALL parameters for length and pause Levy distribution"; exit(-112);}

        if (hasPar("kForSpeed_1") && hasPar("roForSpeed_1") && hasPar("distanceThreshold") && hasPar("kForSpeed_2") && hasPar("roForSpeed_2")) {
            kForSpeed_1 = par("kForSpeed_1").doubleValue();
            roForSpeed_1 = par("roForSpeed_1").doubleValue();
            distanceThreshold = par("distanceThreshold").doubleValue();
            kForSpeed_2 = par("kForSpeed_2").doubleValue();
            roForSpeed_2 = par("roForSpeed_2").doubleValue();
        } else { cout << "It is necessary to specify ALL parameters for speed function"; exit(-212);}

        ASSERT(!movement);
        movement = new Movement(kForSpeed_1,
                                roForSpeed_1,
                                distanceThreshold,
                                kForSpeed_2,
                                roForSpeed_2,
                                maxPermittedDistance,
                                new LeviJump(ciJ, aliJ, deltaXJ, joinJ),
                                new LeviPause(ciP, aliP, deltaXP, joinP));

        ASSERT(!hsc);
        // загрузка данных о локациях
        hsc = HotSpotsCollection::getInstance();
        double minX, maxX, minY, maxY;
        hsc->getTotalSize(minX, maxX, minY, maxY);
        constraintAreaMin.x=minX; constraintAreaMin.y=minY;
        constraintAreaMax.x=maxX; constraintAreaMax.y=maxY;

        ASSERT(!hsd);
        hsd = HSDistanceMatrix::getInstance(powA);

        ASSERT(currentHSindex == -1);
        // выбор случайной локации
        currentHSindex=rand() % hsc->getHSData()->size();
        LevyHotSpotsLATP::setCurrentHSbordersWith( &(hsc->getHSData()->at(currentHSindex)) );

        ASSERT(!mvnHistory);
        mvnHistory = new MovementHistory(NodeID);
    }
}


void LevyHotSpotsLATP::setCurrentHSbordersWith(HotSpotData* hsi) {
    currentHSMin.x = hsi->Xmin;
    currentHSMin.y = hsi->Ymin;
    currentHSMax.x = hsi->Xmax;
    currentHSMax.y = hsi->Ymax;
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;
}


void LevyHotSpotsLATP::setInitialPosition() {
    MobilityBase::setInitialPosition();

    lastPosition.x = uniform(currentHSMin.x, currentHSMax.x);
    lastPosition.y = uniform(currentHSMin.y, currentHSMax.y);
    targetPosition = lastPosition;
    ASSERT(isCorrectCoordinates(lastPosition.x, lastPosition.y));
}


bool LevyHotSpotsLATP::isHotSpotEmpty() {
    return currentHSMin.x == currentHSMax.x || currentHSMin.y == currentHSMax.y;
}


void LevyHotSpotsLATP::setTargetPosition() {
    if (movementsFinished) {
        log(" End of root!");
        nextChange = -1;
        return;
    };

    ASSERT(isCorrectCoordinates(lastPosition.x, lastPosition.y));
    ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));

    // так как данный метод вызывается на этапе инициализации, то этот вызов мы и пропускаем
    if (step++ == 0) return;

    if (isPause) {
        const bool success = movement->genPause( (string("DEBUG LevyHotSpotsLATP::setTargetPosition: NodeId = ") + std::to_string(NodeID)).c_str() );
        ASSERT(success);
        nextChange = simTime() + movement->getWaitTime();
    } else {
        ASSERT(simTime() >= movement->getWaitTime());
        collectStatistics(simTime() - movement->getWaitTime(), simTime(), lastPosition.x, lastPosition.y);
        // если количество первых прыжков, которые нужно пропустить для текущей локации больше нуля,
        // то регенерируем прыжки в случае выхода за границу
        bool regenerateIfOutOfBound = (countOfFirstSkippedLongFlight > 0);
        movementsFinished = !generateNextPosition(targetPosition, nextChange, regenerateIfOutOfBound);
        if (regenerateIfOutOfBound) countOfFirstSkippedLongFlight--;

        if (movementsFinished) {
            log(" End of root!");
            nextChange = -1;
            return;
        };
        ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));
    }
    isPause = !isPause;
}


bool LevyHotSpotsLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound) {
    while (true) {
        const bool success = movement->genFlight( (string("DEBUG LevyHotSpotsLATP::generateNextPosition: NodeId = ") + std::to_string(NodeID)).c_str() );
        ASSERT(success);

        const Coord remTargetPosition = targetPosition;             // сохраняем старое значение
        targetPosition = lastPosition + movement->getDeltaVector(); // записываем новое значение
        if (targetPosition == lastPosition) log();
        ASSERT(targetPosition != lastPosition);
        nextChange = simTime() + movement->getTravelTime();

        // если вышли за пределы локации
        if (!isCorrectCoordinates(targetPosition.x, targetPosition.y)) {
            if (isHotSpotEmpty()) { // если локация точечная
                if (regenerateIfOutOfBound) {
                    // в этой ситуации (счётчик не нуль, а локация вырожденная - в такой обычно должна быть только одна точка)
                    // нужно выбрать снова ТУ ЖЕ позицию
                    targetPosition = lastPosition;
                    break;

                } else if (findNextHotSpotAndTargetPosition()) return true; // выбираем следующую локацию
                       else {// не нашли - останавливаемся и откатываем targetPosition на первоначальное значение
                           targetPosition = remTargetPosition;
                           return false;
                       }
            }

            // для ускорения вычислений определяем вспомогательные переменные
            double x, y, Xdir, Ydir, dir;
            bool flag = ( (y=lastPosition.y) < currentHSCenter.y);

            // выбираем самую дальнюю от текущей позиции вершину прямоугольника текущей локации
            // и вычисляем координаты вектора из текущей позиции в эту вершину
            if ( (x=lastPosition.x) < currentHSCenter.x ) {
                if (flag) { Xdir=currentHSMax.x-x; Ydir=currentHSMax.y-y; }
                else      { Xdir=currentHSMax.x-x; Ydir=currentHSMin.y-y; }
            } else {
                if (flag) { Xdir=currentHSMin.x-x; Ydir=currentHSMax.y-y; }
                else      { Xdir=currentHSMin.x-x; Ydir=currentHSMin.y-y; }
            }

            // проверяем, можем ли остаться в прямоугольнике текущей локации, если прыгать к дальнему углу прямоугольника
            if ( movement->getDistance() > (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
                // не можем остаться
                if (regenerateIfOutOfBound) continue; // генерируем заново прыжок
                else if (findNextHotSpotAndTargetPosition()) return true; // выбираем следующую локацию
                     else {// не нашли - останавливаемся и откатываем targetPosition на первоначальное значение
                         targetPosition = remTargetPosition;
                         return false;
                     }
            }

            // можем остаться - прыгаем
            Coord delta;
            delta.x = Xdir * movement->getDistance()/dir;
            delta.y = Ydir * movement->getDistance()/dir;
            targetPosition = lastPosition + delta;
        }
        break; //получили правильный targetPosition
    }

    return true;
}


bool LevyHotSpotsLATP::findNextHotSpotAndTargetPosition() {
    if (findNextHotSpot()) {   // нашли следующую локацию - идём в её случайную точку
        // перечитываем начальное количество первых длинных прижков, которые надо пропустить для новой локации
        countOfFirstSkippedLongFlight = par("countOfFirstSkippedLongFlight").longValue();

        targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
        targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);

        movement->setDistance(sqrt(  (targetPosition.x-lastPosition.x)*(targetPosition.x-lastPosition.x)
                                   + (targetPosition.y-lastPosition.y)*(targetPosition.y-lastPosition.y) ),
                              (string("DEBUG RegularRootLATP::generateNextPosition: NodeId = ") + std::to_string(NodeID)).c_str());
        nextChange = simTime() + movement->getTravelTime();
        return true;
    }

    return false;
}


bool LevyHotSpotsLATP::findNextHotSpot() {
    int oldHSindex = currentHSindex;
    // выбираем новую локацию
    double rn, pr=0;
    do { rn = ((double) rand()) / RAND_MAX; } while (rn == 0 || rn == 1);
    for(unsigned int i=0; i<hsc->getHSData()->size(); i++) {
        if(i != currentHSindex ) pr+=(hsd->getProbabilityMatrix())[currentHSindex][i];
        if(rn <= pr) {currentHSindex=i; break; }
    }
    setCurrentHSbordersWith( &(hsc->getHSData()->at(currentHSindex)) );

    ASSERT(oldHSindex != currentHSindex);
    return true;
}


//-------------------------- Statistic collection ---------------------------------
void LevyHotSpotsLATP::collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y) {
    mvnHistory->collect(inTime, outTime, x, y);

    double generatedSumTime = (outTime - inTime).dbl();
    ASSERT(generatedSumTime >= 0); // на этапе инициализации возникают отсчёты с нулевой длительностью
    hsc->getHSData()->at(currentHSindex).generatedSumTime += generatedSumTime;
    hsc->getHSData()->at(currentHSindex).generatedWaypointNum++;

    Waypoint h(x, y, inTime.dbl(), outTime.dbl(), mvnHistory->getWpFileName());
    hsc->getHSData()->at(currentHSindex).waypoints.push_back(h);
}


void LevyHotSpotsLATP::saveStatistics() {
    log("Start saving statistics...");
    const char *outDir  = NamesAndDirs::getOutDir();
    const char *wpsDir  = NamesAndDirs::getOutWpsDir();
    const char *trsDir  = NamesAndDirs::getOutTrsDir();
    const char *hsDir   = NamesAndDirs::getOutHsDir();
    const char *thRtDir = NamesAndDirs::getOutTheoryRtDir();
    const char *acRtDir = NamesAndDirs::getOutActualRtDir();
    const char *locs    = NamesAndDirs::getOutLocFile();

    if (NodeID == 0 ) {//чтобы записывал только один узел
        //--- Create output directories ---
        if (CreateDirectory(outDir, NULL)) cout << "create output directory: " << outDir << endl;
        else cout << "error create output directory: " << outDir << endl;

        if (CreateDirectory(wpsDir, NULL)) cout << "create output directory: " << wpsDir << endl;
        else cout << "error create output directory: " << wpsDir << endl;

        if (CreateDirectory(trsDir, NULL)) cout << "create output directory: " << trsDir << endl;
        else cout << "error create output directory: " << trsDir << endl;

        if (CreateDirectory(hsDir, NULL)) cout << "create output directory: " << hsDir << endl;
        else cout << "error create output directory: " << hsDir << endl;

        if (CreateDirectory(thRtDir, NULL)) cout << "create output directory: " << thRtDir << endl;
        else cout << "error create output directory: " << thRtDir << endl;

        if (CreateDirectory(acRtDir, NULL)) cout << "create output directory: " << acRtDir << endl;
        else cout << "error create output directory: " << acRtDir << endl;

        // --- Write Locations ---
        hsc->saveHotSpots(hsDir);

        // --- Write HotSpots ---
        hsc->saveLocationsFile(locs);

        // --- Write Roots for every node & every day ---
        RootsCollection::getInstance()->saveRoots(thRtDir, acRtDir);
    }

    //--- Write points ---
    mvnHistory->save(wpsDir, trsDir);
    log("Statistics saved");
}

bool LevyHotSpotsLATP::isCorrectCoordinates(double x, double y) {
    if (currentHSMin.x <= x && x <= currentHSMax.x && currentHSMin.y <= y && y <= currentHSMax.y) return true;
    //log();
    return false;
}

void LevyHotSpotsLATP::log(string log) {
    cout << "NodeId = " << NodeID << ": "  << log << endl;
}

void LevyHotSpotsLATP::log() {  // Отладочная функция
    cout << "----------------------------- LOG --------------------------------" << endl;
    cout << "step = " << step << ", isPause = " << isPause << endl;
    cout << "simTime() = " << simTime() << endl;
    cout << "lastPosition = " << lastPosition << endl;

    cout << "currentHSindex = " << currentHSindex << endl;
    cout << "\t currentHSMin.x = " << currentHSMin.x << ", currentHSMax.x = " << currentHSMax.x << endl;
    cout << "\t currentHSMin.y = " << currentHSMin.y << ", currentHSMax.y = " << currentHSMax.y << endl;
    cout << "\t currentHSCenter.x = " << currentHSCenter.x << ", currentHSCenter.y = " << currentHSCenter.y << endl;
    cout << "\t isHotSpotEmpty = " << isHotSpotEmpty() << endl;
    hsc->getHSData()->at(currentHSindex).print();

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
