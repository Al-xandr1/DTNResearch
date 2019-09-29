#include "LevyHotSpotsLATP.h"

Define_Module(LevyHotSpotsLATP);

LevyHotSpotsLATP::LevyHotSpotsLATP() {
    NodeID = -1;

    // �������� ������� � �����, ����� �� "��������� ������" ������ ����� (�������� �������� � ���)
    // � �� ���, ����� ��� ������������� �������� �� � ��������� � ��� ������ ��������� ����� ������� �����
    isPause = true;

    // ������ ��� �������. ����� �� �� ���������, ��� �� ������ �������������,
    // � ������� ������ ������ (������� � ������� ����)
    step = 0;
    movementsFinished = false;
    countOfFirstSkippedLongFlight = -1;
    powA=2.0;

    movement = NULL;
    waitTime = -1;

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

        if (!hasPar("countOfFirstSkippedLongFlight")) {cout << "It is necessary to specify 'countOfFirstSkippedLongFlight' parameter"; exit(-122);}
        countOfFirstSkippedLongFlight = par("countOfFirstSkippedLongFlight").longValue();

        if (!hasPar("powA")) {cout << "It is necessary to specify 'powA' parameter"; exit(-112);}
        powA = par("powA").doubleValue();

        ASSERT(!hsc);
        // �������� ������ � ��������
        hsc = HotSpotsCollection::getInstance();
        double minX, maxX, minY, maxY;
        hsc->getTotalSize(minX, maxX, minY, maxY);
        constraintAreaMin.x=minX; constraintAreaMin.y=minY;
        constraintAreaMax.x=maxX; constraintAreaMax.y=maxY;

        ASSERT(!movement);
        movement = new Movement(this, (constraintAreaMax - constraintAreaMin).length());

        ASSERT(!hsd);
        hsd = HSDistanceMatrix::getInstance(powA);

        ASSERT(currentHSindex == -1);
        // ����� ��������� �������
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
//    ASSERT(isCorrectCoordinates(lastPosition.x, lastPosition.y));
}


bool LevyHotSpotsLATP::isHotSpotEmpty() {
    return currentHSMin.x == currentHSMax.x || currentHSMin.y == currentHSMax.y;
}


void LevyHotSpotsLATP::setTargetPosition() {
    if (movementsFinished) {
        log("LevyHotSpotsLATP::setTargetPosition: End of root! (before generation)");
        nextChange = -1;
        return;
    };

    //    ASSERT(isCorrectCoordinates(lastPosition.x, lastPosition.y));
    //    ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));

    // ��� ��� ������ ����� ���������� �� ����� �������������, �� ���� ����� �� � ����������
    if (step == 0) { step++; return; }

    if (isPause) {
        const bool success = generatePause(nextChange);
//        ASSERT(success);
        // ���������� ��� ����� ���� "������� � ���������"
        step++;
    } else {
//        ASSERT(simTime() >= getWaitTime());
        collectStatistics(simTime() - getWaitTime(), simTime(), lastPosition.x, lastPosition.y);
        // ���� ���������� ������ �������, ������� ����� ���������� ��� ������� ������� ������ ����,
        // �� ������������ ������ � ������ ������ �� �������
        bool regenerateIfOutOfBound = (countOfFirstSkippedLongFlight > 0);
        movementsFinished = !generateNextPosition(targetPosition, nextChange, regenerateIfOutOfBound);
        if (regenerateIfOutOfBound) countOfFirstSkippedLongFlight--;

        if (movementsFinished) {
            log("LevyHotSpotsLATP::setTargetPosition: End of root! (after generation)");
            nextChange = -1;
            return;
        };
        //        ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));
    }
    isPause = !isPause;
}


bool LevyHotSpotsLATP::generatePause(simtime_t &nextChange) {
    const bool success = movement->genPause( (string("DEBUG LevyHotSpotsLATP::setTargetPosition: NodeId = ") + std::to_string(NodeID)).c_str() );
    setWaitTime(movement->getWaitTime());
    nextChange = simTime() + getWaitTime();
    return success;
}


bool LevyHotSpotsLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound) {
    while (true) {
        const bool success = movement->genFlight( (string("DEBUG LevyHotSpotsLATP::generateNextPosition: NodeId = ") + std::to_string(NodeID)).c_str() );
//        ASSERT(success);

        const Coord remTargetPosition = targetPosition;             // ��������� ������ ��������
        targetPosition = lastPosition + movement->getDeltaVector(); // ���������� ����� ��������
        if (targetPosition == lastPosition) log();
//        ASSERT(targetPosition != lastPosition);
        nextChange = simTime() + movement->getTravelTime();

        // ���� ����� �� ������� �������
        if (!isCorrectCoordinates(targetPosition.x, targetPosition.y)) {
            if (isHotSpotEmpty()) { // ���� ������� ��������
                if (regenerateIfOutOfBound) {
                    // � ���� �������� (������� �� ����, � ������� ����������� - � ����� ������ ������ ���� ������ ���� �����)
                    // ����� ������� ����� �� �� �������
                    targetPosition = lastPosition;
                    break;

                } else if (findNextHotSpotAndTargetPosition()) return true; // �������� ��������� �������
                       else {// �� ����� - ��������������� � ���������� targetPosition �� �������������� ��������
                           targetPosition = remTargetPosition;
                           return false;
                       }
            }

            // ��� ��������� ���������� ���������� ��������������� ����������
            double x, y, Xdir, Ydir, dir;
            bool flag = ( (y=lastPosition.y) < currentHSCenter.y);

            // �������� ����� ������� �� ������� ������� ������� �������������� ������� �������
            // � ��������� ���������� ������� �� ������� ������� � ��� �������
            if ( (x=lastPosition.x) < currentHSCenter.x ) {
                if (flag) { Xdir=currentHSMax.x-x; Ydir=currentHSMax.y-y; }
                else      { Xdir=currentHSMax.x-x; Ydir=currentHSMin.y-y; }
            } else {
                if (flag) { Xdir=currentHSMin.x-x; Ydir=currentHSMax.y-y; }
                else      { Xdir=currentHSMin.x-x; Ydir=currentHSMin.y-y; }
            }

            // ���������, ����� �� �������� � �������������� ������� �������, ���� ������� � �������� ���� ��������������
            if ( movement->getDistance() > (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
                // �� ����� ��������
                if (regenerateIfOutOfBound) continue; // ���������� ������ ������
                else if (findNextHotSpotAndTargetPosition()) return true; // �������� ��������� �������
                     else {// �� ����� - ��������������� � ���������� targetPosition �� �������������� ��������
                         targetPosition = remTargetPosition;
                         return false;
                     }
            }

            // ����� �������� - �������
            Coord delta;
            delta.x = Xdir * movement->getDistance()/dir;
            delta.y = Ydir * movement->getDistance()/dir;
            targetPosition = lastPosition + delta;
        }
        break; //�������� ���������� targetPosition
    }

    return true;
}


bool LevyHotSpotsLATP::findNextHotSpotAndTargetPosition() {
    if (findNextHotSpot()) {   // ����� ��������� ������� - ��� � � ��������� �����
        // ������������ ��������� ���������� ������ ������� �������, ������� ���� ���������� ��� ����� �������
        countOfFirstSkippedLongFlight = par("countOfFirstSkippedLongFlight").longValue();

        targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
        targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);

        movement->setDistance(lastPosition.distance(targetPosition), (string("DEBUG RegularRootLATP::generateNextPosition: NodeId = ") + std::to_string(NodeID)).c_str());
        nextChange = simTime() + movement->getTravelTime();
        return true;
    }

    return false;
}


bool LevyHotSpotsLATP::findNextHotSpot() {
    int oldHSindex = currentHSindex;
    // �������� ����� �������
    double rn, pr=0;
    do { rn = ((double) rand()) / RAND_MAX; } while (rn == 0 || rn == 1);
    for(unsigned int i=0; i<hsc->getHSData()->size(); i++) {
        if(i != currentHSindex ) pr+=(hsd->getProbabilityMatrix())[currentHSindex][i];
        if(rn <= pr) {currentHSindex=i; break; }
    }
    setCurrentHSbordersWith( &(hsc->getHSData()->at(currentHSindex)) );

//    ASSERT(oldHSindex != currentHSindex);
    return true;
}


//-------------------------- Statistic collection ---------------------------------
void LevyHotSpotsLATP::collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y) {
    mvnHistory->collect(inTime, outTime, x, y);

    double generatedSumTime = (outTime - inTime).dbl();
//    ASSERT(generatedSumTime >= 0); // �� ����� ������������� ��������� ������� � ������� �������������
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

    if (NodeID == 0 ) {//����� ��������� ������ ���� ����
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

void LevyHotSpotsLATP::log() {  // ���������� �������
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
