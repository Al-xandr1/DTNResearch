#include "RealMobility.h"

Define_Module(RealMobility);

RealMobility::RealMobility() {
    NodeID = -1;

    step = 0;

    movementsFinished = false;

    traces = NULL;
    currentTrace = NULL;

    timeOffset = 0;
    distance = -1;
    speed = -1;
    travelTime = 0;

    mvnHistory = NULL;
}


void RealMobility::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == 0) {
        stationary = false;
        NodeID = (int) par("NodeID");

        lastPosition.x = 0;
        lastPosition.y = 0;
        lastPosition.z = 0;

        ASSERT(!traces);
        traces = TracesCollection::getInstance();
        makeNewRoot();

        ASSERT(!mvnHistory);
        mvnHistory = new MovementHistory(NodeID);
    }
}

void RealMobility::makeNewRoot() {
    if (!currentTrace) currentTrace = traces->getTraces()->at(NodeID);
    ASSERT(currentTrace->size() > 0);

    step = 0;

    lastPosition.x = currentTrace->at(0).X;
    lastPosition.y = currentTrace->at(0).Y;
    lastPosition.z = 0;

    targetPosition = lastPosition;
}

void RealMobility::endRoute() {
    cMessage* msg = new cMessage("END_ROUTE", END_ROUTE);
    take(msg);
    sendDirect(msg, getParentModule()->gate("in"));
    nextChange = -1;
}

void RealMobility::setInitialPosition() {
    MobilityBase::setInitialPosition();

    lastPosition.x = currentTrace->at(0).X;
    lastPosition.y = currentTrace->at(0).Y;
    lastPosition.z = 0;

    targetPosition = lastPosition;
}

void RealMobility::setTargetPosition() {
    if (movementsFinished) {
        endRoute();
        return;
    };

    step++;
    mvnHistory->collect(simTime(), simTime(), lastPosition.x, lastPosition.y);
    movementsFinished = !generateNextPosition(targetPosition, nextChange);

    if (movementsFinished) {
        endRoute();
        return;
    };
}


// √енерирует следующую позицию в зависимости от того, включено использование гор€чих точек или нет
bool RealMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    ASSERT(0 < step);
    if (step >= currentTrace->size()) return false; //маршрут кончилс€

    simtime_t previousNaxtChange = nextChange;
    nextChange = timeOffset + currentTrace->at(step).T;
    targetPosition.x = currentTrace->at(step).X;
    targetPosition.y = currentTrace->at(step).Y;

    distance = lastPosition.distance(targetPosition);
    ASSERT(distance >= 0);

    travelTime = nextChange - previousNaxtChange;
    ASSERT(travelTime > 0);

    speed = distance / travelTime;
    ASSERT(speed >= 0);

    return true;
}

void RealMobility::saveStatistics() {
    log("Start saving statistics...");
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
    log("Statistics saved");
}

void RealMobility::log(string log) {
    cout << "NodeId = " << NodeID << ": "  << log << endl;
}

void RealMobility::log() {  // ќтладочна€ функци€
    cout << "----------------------------- LOG --------------------------------" << endl;
    cout << "NodeID = " << NodeID << endl;
    cout << "simTime() = " << simTime() << endl;
    cout << "lastPosition = " << lastPosition << endl;
    cout << "distance = " << distance << ", speed = " << speed << ", travelTime = " << travelTime << endl;
    cout << "targetPosition = " << targetPosition << endl;
    cout << "nextChange = " << nextChange << endl;
    cout << "movementsFinished = " << movementsFinished << endl;
    cout << "------------------------------------------------------------------" << endl << endl;
}
