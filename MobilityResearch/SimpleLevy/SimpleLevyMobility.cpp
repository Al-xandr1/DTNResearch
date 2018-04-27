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
    maxPermittedDistance = -1;

    waitTime = 0;
    currentHSindex = -1;

    movementsFinished = false;

    angle = -1;
    distance = -1;
    speed = -1;
    travelTime = 0;

    powA=2.0;

    mvnHistory = NULL;
}

void SimpleLevyMobility::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    double ciJ,aliJ,deltaXJ,joinJ, ciP,aliP,deltaXP,joinP;

    if (stage == 0) {
        stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0;

        constraintAreaMin.x = par("constraintAreaMinX").doubleValue();
        constraintAreaMax.x = par("constraintAreaMaxX").doubleValue();
        constraintAreaMin.y = par("constraintAreaMinY").doubleValue();
        constraintAreaMax.y = par("constraintAreaMaxY").doubleValue();

        maxPermittedDistance = (constraintAreaMax - constraintAreaMin).length();

        NodeID = (int) par("NodeID");

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

        if (hasPar("kForSpeed") && hasPar("roForSpeed")) {
            kForSpeed = par("kForSpeed").doubleValue();
            roForSpeed = par("roForSpeed").doubleValue();
        } else { cout << "It is necessary to specify ALL parameters for speed function"; exit(-212);}
    }

    if (jump  == NULL) jump  = new LeviJump(ciJ, aliJ, deltaXJ, joinJ);
    if (pause == NULL) pause = new LeviPause(ciP, aliP, deltaXP, joinP);

    // ��������� ������� - �� ����
    currentHSMin=getConstraintAreaMin();
    currentHSMax=getConstraintAreaMax();
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;

    if (!mvnHistory) mvnHistory = new MovementHistory(NodeID);
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
        waitTime = checkValue(pause->get_Levi_rv((MAXTIME - simTime()).dbl()), (MAXTIME - simTime()).dbl());
        ASSERT(waitTime > 0 && waitTime <= (MAXTIME - simTime()));
        nextChange = simTime() + waitTime;
    } else {
        mvnHistory->collect(simTime() - waitTime, simTime(), lastPosition.x, lastPosition.y);
        movementsFinished = !generateNextPosition(targetPosition, nextChange);

        if (movementsFinished) {nextChange = -1; return;};
        ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));
    }
    isPause = !isPause;
}

bool SimpleLevyMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    
    // ���������� ������ ���� ��� ������
    angle = uniform(0, 2 * PI);
    distance = checkValue(jump->get_Levi_rv(maxPermittedDistance), maxPermittedDistance);
    speed = kForSpeed * pow(distance, 1 - roForSpeed);
    Coord delta(distance * cos(angle), distance * sin(angle), 0);
    deltaVector = delta;
    travelTime = checkValue(distance / speed, (MAXTIME - simTime()).dbl());

    targetPosition = lastPosition + delta;
    ASSERT(targetPosition.x != lastPosition.x);
    nextChange = simTime() + travelTime;

    // ���� ����� �� ������� �������
    if (currentHSMin.x > targetPosition.x || targetPosition.x > currentHSMax.x || currentHSMin.y > targetPosition.y || targetPosition.y > currentHSMax.y) {
	// ��� ��������� ���������� ���������� ��������������� ����������
        double x, y, Xdir, Ydir, dir;
        bool flag = ( (y=getLastPosition().y) < currentHSCenter.y);

        // �������� ����� ������� �� ������� ������� ������� �������������� ������� ������� 
        // � ��������� ���������� ������� �� ������� ������� � ��� �������
        if ( (x=getLastPosition().x) < currentHSCenter.x ) {
            if (flag) { Xdir=currentHSMax.x-x; Ydir=currentHSMax.y-y; }
            else      { Xdir=currentHSMax.x-x; Ydir=currentHSMin.y-y; }
        } else {
            if (flag) { Xdir=currentHSMin.x-x; Ydir=currentHSMax.y-y; }
            else      { Xdir=currentHSMin.x-x; Ydir=currentHSMin.y-y; }
        }

        // ���������, ����� �� �������� � �������������� ������� �������, ���� ������� � �������� ���� ��������������
        if ( distance <= (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
            // ����� - �������
            delta.x = Xdir * distance/dir;
            delta.y = Ydir * distance/dir;
            targetPosition = getLastPosition() + delta;
        } else { // �� ����� - ���� ���������� � ������ �������
            if ( findNextHotSpot() ) {   // ����� ��������� ������� - ��� � � ��������� �����
               targetPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
               targetPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
            } else return false;  // �� ����� - ���������������
        }
    }

    return true;
}

void SimpleLevyMobility::saveStatistics() {
    const char *outDir = NamesAndDirs::getOutDir();
    const char *wpsDir = NamesAndDirs::getOutWpsDir();
    const char *trsDir = NamesAndDirs::getOutTrsDir();

    if (NodeID == 0 ) {//����� ��������� ������ ���� ����
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

void SimpleLevyMobility::log() {  // ���������� �������
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
