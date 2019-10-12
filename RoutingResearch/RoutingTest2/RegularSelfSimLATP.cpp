#include <RegularSelfSimLATP.h>

Define_Module(RegularSelfSimLATP);

RegularSelfSimLATP::RegularSelfSimLATP() {
    mvnHistoryForRepeat = NULL;
    repetitionOfTraceEnabled = false;

    timeOffset = 0;
    distance = -1;
    speed = -1;
    travelTime = 0;
}

void RegularSelfSimLATP::initialize(int stage) {
    SelfSimLATP::initialize(stage);

    if (hasPar("repetitionOfTraceEnabled")) repetitionOfTraceEnabled = par("repetitionOfTraceEnabled");
    log(string("RegularSelfSimLATP::initialize: repetitionOfTraceEnabled = ") + std::to_string(repetitionOfTraceEnabled) + string(" stage = ") + std::to_string(stage));
}

void RegularSelfSimLATP::handleMessage(cMessage *message) {
    if (message->isSelfMessage())
        MobilityBase::handleMessage(message);
    else
        switch (message->getKind()) {
            // ������������ ��� "�����" ��� �����������, ����� ����� ������ ������
            case MOBILITY_START:{
                ASSERT(RoutingDaemon::instance->getCurrentDay() >= 1);
                // ��� ������� ��� ������� �������� ��� ������������� �����������
                if (RoutingDaemon::instance->getCurrentDay() > 1) {
                    SelfSimLATP::makeNewRoot();
                }

                nextChange = simTime();
                MovingMobilityBase::scheduleUpdate();
                emitMobilityStateChangedSignal();
                myDelete(message);
                break;
            }
            default:
                ASSERT(false); //unreacheble statement
        }
}

void RegularSelfSimLATP::setTargetPosition() {
    // ����������� �� ������ �������� � ����� ������
    SelfSimLATP::setTargetPosition();

    if (movementsFinished) {
        if (repetitionOfTraceEnabled) {
            // ��� ��������� �������� � ��� ��������� ���������� �� ��������� ������� �����
            // ��������� ������ ���� ��� (��� ������ ���������)
            if (!mvnHistoryForRepeat) {
                mvnHistoryForRepeat = buildFirstTrace();
                log(string("RegularSelfSimLATP::setTargetPosition: First trace is built! repetitionOfTraceEnabled = ") + std::to_string(repetitionOfTraceEnabled));
            }
        }

        // ������� ������ � ��������� � ������������� - ����� �������������, �� �� ���������
        endRoute();
        movementsFinished = false;
        nextChange = MAXTIME;
    }
}

bool RegularSelfSimLATP::generatePause(simtime_t &nextChange) {
    // ���� ���������� �������� ���������, �� ����� �� SLAW �� �����
    if (!repetitionOfTraceEnabled)
        return SelfSimLATP::generatePause(nextChange);

    // ���� ���������� �������� ��������, �� ��������� ������� ����������� ������
    // ���� ����� ��� �� �������, �� ����� �� SLAW, ��� ��������� ������
    if (!mvnHistoryForRepeat)
        return SelfSimLATP::generatePause(nextChange);


    // ���� ����� ��� �������, �� ����� �� ����, ���� �� �����������. ���� �����
    long step = calculateStep();
    setWaitTime(mvnHistoryForRepeat->getOutTimes()->at(step) - mvnHistoryForRepeat->getInTimes()->at(step));
    nextChange = simTime() + getWaitTime();
    //    ASSERT(nextChange == (timeOffset + mvnHistoryForRepeat->getOutTimes()->at(step)));
    //    log(string("RegularSelfSimLATP::generatePause timeOffset = " + std::to_string(timeOffset.dbl())) + string(", step = ") + std::to_string(step));
    return true;
}


bool RegularSelfSimLATP::generateNextPosition(Coord &targetPosition, simtime_t &nextChange) {
    // ���� ���������� �������� ���������, �� ����� �� SLAW �� �����
    if (!repetitionOfTraceEnabled)
        return SelfSimLATP::generateNextPosition(targetPosition, nextChange);

    // ���� ���������� �������� ��������, �� ��������� ������� ����������� ������
    // ���� ����� ��� �� �������, �� ����� �� SLAW, ��� ��������� ������
    if (!mvnHistoryForRepeat)
        return SelfSimLATP::generateNextPosition(targetPosition, nextChange);


    // ���� ����� ��� �������, �� ����� �� ����, ���� �� �����������. ���� �������
    long step = calculateStep();
    if (step == 0)
        return false; // ������� ����� �� ������������� �������. �������, ��� ������� ��������.

    // ������ �� ����� � �� �������� ����� ����� �� ������
    const simtime_t previousNextChange = nextChange;
    nextChange = timeOffset + mvnHistoryForRepeat->getInTimes()->at(step);
    targetPosition.x = mvnHistoryForRepeat->getXCoordinates()->at(step);
    targetPosition.y = mvnHistoryForRepeat->getYCoordinates()->at(step);

    distance = lastPosition.distance(targetPosition);
    ASSERT(distance >= 0);

    travelTime = nextChange - previousNextChange;
    ASSERT(travelTime > 0);

    speed = distance / travelTime;
    ASSERT(speed >= 0);
    //    log(string("RegularSelfSimLATP::generateNextPosition timeOffset = " + std::to_string(timeOffset.dbl())) + string(", step = ") + std::to_string(step));

    return true;
}

void RegularSelfSimLATP::nodeTurnedOff() {
    emitMobilityStateChangedSignal();
}

void RegularSelfSimLATP::endRoute() {
    cMessage* msg = new cMessage("END_ROUTE", END_ROUTE);
    take(msg);
    sendDirect(msg, getParentModule()->gate("in"));
}

 void RegularSelfSimLATP::makeRoot() {
     unsigned int day = RoutingDaemon::instance->getCurrentDay();
     ASSERT(day >= 1);
     cout << endl;
     log(string("RegularSelfSimLATP::makeRoot: Start root making for day = ") + std::to_string(day)
             + string(", isRootReady = ") + std::to_string(isRootReady)
             + string(", repetitionOfTraceEnabled = ") + std::to_string(repetitionOfTraceEnabled)
             + string(", mvnHistoryForRepeat = ") + std::to_string((mvnHistoryForRepeat != NULL)));

     // ��� ����, ����� ����� SelfSimLATP::makeNewRoot() ���������, ����� ���� ��� ����������� ���������� ��������� currentRoot.
     ASSERT(!currentRoot);
     if (!isRootReady) {
         RootsCollection *collection = NULL;
         vector<RootsCollection *> *dailyRoot = rc->getDailyRoot();
         if (dailyRoot) {
             // ���� ������ ��������� ��� ������� ��� ����, �� ����� �� ���

             // ��� ����� ��� ����� ���� ������ ��� ������� ��� ����.
             // �������, ��������, ��������� ������� ��������� ����������
             if (day > dailyRoot->size()) day = dailyRoot->size();
             collection = dailyRoot->at(day-1);

         } else {
             // ���� ������ ��� ������� ��� ���, �� ����� ������ ���� �� ������ ������ ������
             collection = rc;
         }

         RootNumber = NodeID % collection->getRootsData()->size();
         ASSERT(RootNumber >= 0 && RootNumber < collection->getRootsData()->size());
         currentRoot = new vector<HotSpotData *>();
         for (unsigned int i = 0; i < collection->getRootsData()->at(RootNumber)->size(); i++) {
             currentRoot->push_back(new HotSpotData(collection->getRootsData()->at(RootNumber)->at(i)));
         }
         log(string("RegularSelfSimLATP::makeRoot: Root made for day = ") + std::to_string(day) + string(", RootNumber = ") + std::to_string(RootNumber));
     }
     isRootReady = true;


     if (repetitionOfTraceEnabled && !mvnHistoryForRepeat) {
         // � ������, ����� ������ ����� ������� (��-�� ������ ���), � ������ ������� ��� ���, �� ��������� �� ������ ����, ��� ����.
         mvnHistoryForRepeat = buildFirstTrace();
         log(string("RegularSelfSimLATP::makeRoot: First trace is built! repetitionOfTraceEnabled = ") + std::to_string(repetitionOfTraceEnabled));
     }

     if (repetitionOfTraceEnabled && mvnHistoryForRepeat) {
         // ���� ��� ���������� ���������� �������� ����� ��� �������, �� ���� ��������� ����� �� ����
         timeOffset = (day-1) * RoutingDaemon::instance->getDayDuration();
         log(string("RegularSelfSimLATP::makeRoot: Repeatable root is made! day = ") + std::to_string(day)
             + string(", simTime = ") +  std::to_string(simTime().dbl())
             + string(", timeOffset = ") + std::to_string(timeOffset.dbl()));

         long step = calculateStep();
         log("RegularSelfSimLATP::makeRoot: step = " + std::to_string(step)
                 + ", getStep() = " + std::to_string(getStep())
                 + ", mvnHistoryForRepeat->getSize() = " + std::to_string(mvnHistoryForRepeat->getSize()));
         // � ������, ����� ������ ����� ������� (��-�� ������ ���), �� ����� ����� �� ���� ������ ��� ����� (�.� ����� ��� ���� ���������� �� ����� �����).
         if (step > 0) {
             decreaseStep();
             step = calculateStep();
         }
         ASSERT(step == 0);
         lastPosition.x = mvnHistoryForRepeat->getXCoordinates()->at(step);
         lastPosition.y = mvnHistoryForRepeat->getYCoordinates()->at(step);
         lastPosition.z = 0;
         targetPosition = lastPosition;
     }
}

 MovementHistory* RegularSelfSimLATP::buildFirstTrace() {
     MovementHistory* history = new MovementHistory(*getMovementHistory());
     // for debug
     //                const char *wpsDir = "outTrace/first_day_wps";
     //                const char *trsDir = "outTrace/first_day_trs";
     //                if (CreateDirectory(wpsDir, NULL)) cout << "create output directory: " << wpsDir << endl;
     //                else cout << "error create output directory: " << wpsDir << endl;
     //                if (CreateDirectory(trsDir, NULL)) cout << "create output directory: " << trsDir << endl;
     //                else cout << "error create output directory: " << trsDir << endl;
     //                mvnHistoryForRepeat->save(wpsDir, trsDir);
     // for debug
     return history;
 }

 long RegularSelfSimLATP::calculateStep() {
     ASSERT(mvnHistoryForRepeat);
     return (getStep() - 1) % mvnHistoryForRepeat->getSize();
 }
