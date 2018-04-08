#include "RealMobility.h"

Define_Module(RealMobility);

RealMobility::RealMobility() {
    NodeID = -1;

    step = 0;

    movementsFinished = false;

    traces = NULL;
    currentTrace = NULL;

    distance = -1;
    speed = -1;
    travelTime = 0;

    wpFileName = NULL;
    trFileName = NULL;
}


void RealMobility::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == 0) {
        stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0;
        NodeID = (int) par("NodeID");

        lastPosition.x = 0;
        lastPosition.y = 0;
        lastPosition.z = 0;
    }

    if (!traces) {
        traces = TracesCollection::getInstance();
        currentTrace = traces->getTraces()->at(NodeID);
        ASSERT(currentTrace->size() > 0);
    }

    if (wpFileName == NULL && trFileName == NULL) {
        wpFileName = new char[256];
        trFileName = new char[256];
        wpFileName = createFileName(wpFileName, 0, par("traceFileName").stringValue(),
                (int) ((par("NodeID"))), WAYPOINTS_TYPE);
        trFileName = createFileName(trFileName, 0, par("traceFileName").stringValue(),
                (int) ((par("NodeID"))), TRACE_TYPE);
    }
}


void RealMobility::handleMessage(cMessage * message)
{
    if (message->isSelfMessage())
        MobilityBase::handleMessage(message);
    else
        switch (message->getKind()) {
            // используется для "пинка" для мобильности, чтобы снова начать ходить
            case MOBILITY_START:{
                //todo Заглушка. Делать окончание и начало дня как для RegularRootLATP::handleMessage ИЛИ НЕТ! т.е. тут просто заглушка
                myDelete(message);
                break;
            }
            default:
                ASSERT(false); //unreacheble statement
        }
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
        cMessage* msg = new cMessage("ROUTE_ENDED", ROUTE_ENDED);
        take(msg);
        sendDirect(msg, getParentModule()->gate("in"));
        nextChange = -1;
        return;
    };

    step++;
    collectStatistics(simTime(), simTime(), lastPosition.x, lastPosition.y);
    movementsFinished = !generateNextPosition(targetPosition, nextChange);

    if (movementsFinished) {
        cMessage* msg = new cMessage("ROUTE_ENDED", ROUTE_ENDED);
        take(msg);
        sendDirect(msg, getParentModule()->gate("in"));
        nextChange = -1;
        return;
    };
}


// Генерирует следующую позицию в зависимости от того, включено использование горячих точек или нет
bool RealMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    ASSERT(0 < step);
    if (step >= currentTrace->size()) return false; //маршрут кончился

    simtime_t previousNaxtChange = nextChange;
    nextChange = currentTrace->at(step).T;
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


//-------------------------- Statistic collection ---------------------------------
void RealMobility::collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y) {
    inTimes.push_back(inTime);
    outTimes.push_back(outTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
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
    log("Statistics saved");
}

void RealMobility::log(string log) {
    cout << "NodeId = " << NodeID << ": "  << log << endl;
}

void RealMobility::log() {  // Отладочная функция
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
