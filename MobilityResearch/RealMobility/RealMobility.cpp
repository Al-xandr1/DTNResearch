#include "RealMobility.h"

Define_Module(RealMobility);

RealMobility::RealMobility() {
    NodeID = -1;

    isPause = false;
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
            //todo Заглушка. Делать окончание и начало дня как для RegularRootLATP::handleMessage ИЛИ НЕТ! т.е. тут просто заглушка
        }
}


void RealMobility::setInitialPosition() {
    MobilityBase::setInitialPosition();

    lastPosition.x = currentTrace->at(0).X;
    lastPosition.y = currentTrace->at(0).Y;
    lastPosition.z = 0;

    targetPosition = lastPosition;
}

#define ROUTE_ENDED          10 // сообщение об окончании маршрута
void RealMobility::setTargetPosition() {
    if (movementsFinished) {
        sendDirect(new cMessage("End of route", ROUTE_ENDED), getParentModule()->gate("in"));
        nextChange = -1;
        return;
    };
    step++;
    if (isPause) {
        waitTime = (simtime_t) 1;//pause->get_Levi_rv(); //todo использование трассы точнее времени между точками
        ASSERT(waitTime > 0);
        nextChange = simTime() + waitTime;
    } else {
        collectStatistics(simTime() - waitTime, simTime(), lastPosition.x, lastPosition.y);
        movementsFinished = !generateNextPosition(targetPosition, nextChange);

        if (movementsFinished) {
            sendDirect(new cMessage("End of route", ROUTE_ENDED), getParentModule()->gate("in"));
            nextChange = -1;
            return;
        };
    }
    isPause = !isPause;
}


// Генерирует следующую позицию в зависимости от того, включено использование горячих точек или нет
bool RealMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    //есть  step ==  1, 3, 5, 7, 9...
    unsigned int index = (step / 2) + 1;
    //нужно index == 1, 2, 3, 4, 5...currentTrace->size()-1
    ASSERT(0 < index);
    if (index >= currentTrace->size()) return false; //маршрут кончился

    simtime_t previousNaxtChange = nextChange;
    nextChange = currentTrace->at(index).T;
    targetPosition.x = currentTrace->at(index).X;
    targetPosition.y = currentTrace->at(index).Y;

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


void RealMobility::log() {  // Отладочная функция
    cout << "----------------------------- LOG --------------------------------" << endl;
    cout << "NodeID = " << NodeID << endl;
    cout << "step = " << step << ", isPause = " << isPause << endl;
    cout << "simTime() = " << simTime() << endl;
    cout << "lastPosition = " << lastPosition << endl;

    if (isPause) {
        cout << "waitTime = " << waitTime << endl;
    } else {
        cout << "distance = " << distance << ", speed = " << speed << ", travelTime = " << travelTime << endl;
    }

    cout << "targetPosition = " << targetPosition << endl;
    cout << "nextChange = " << nextChange << endl;

    cout << "movementsFinished = " << movementsFinished << endl;
    cout << "------------------------------------------------------------------" << endl << endl;
}
