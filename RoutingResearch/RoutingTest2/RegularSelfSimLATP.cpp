#include <RegularSelfSimLATP.h>

Define_Module(RegularSelfSimLATP);

RegularSelfSimLATP::RegularSelfSimLATP() {
    mvnHistoryForRepeat = NULL;
    repetitionOfTraceEnabled = false;

    isPause = false;
    step = 0;
    timeOffset = 0;
    distance = -1;
    speed = -1;
    travelTime = 0;
}

void RegularSelfSimLATP::initialize(int stage) {
    SelfSimLATP::initialize(stage);

    if (hasPar("repetitionOfTraceEnabled")) repetitionOfTraceEnabled = par("repetitionOfTraceEnabled");
    cout << "RegularSelfSimLATP::initialize: repetitionOfTraceEnabled = " << repetitionOfTraceEnabled << endl;
}

void RegularSelfSimLATP::handleMessage(cMessage *message) {
    if (message->isSelfMessage())
        MobilityBase::handleMessage(message);
    else
        switch (message->getKind()) {
            // используется для "пинка" для мобильности, чтобы снова начать ходить
            case MOBILITY_START:{
                ASSERT(RoutingDaemon::instance->getCurrentDay() >= 1);
                // для первого дня маршрут построен при инициализации мобильности
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
    // Перемещение по модели вызываем в любом случае
    SelfSimLATP::setTargetPosition();

    if (movementsFinished) {
        if (repetitionOfTraceEnabled) {
            // при окончании маршрута и при вклчённом повторении мы сохраняем текущий трейс
            // сохраняем только один раз (при первом обращении)
            if (!mvnHistoryForRepeat) mvnHistoryForRepeat = new MovementHistory(*getMovementHistory());
        }

        // очищают статус и планируем в бесконечность - чтобы приостановить, но не завершить
        endRoute();
        movementsFinished = false;
        nextChange = MAXTIME;
    }
}

bool RegularSelfSimLATP::generateNextPosition(Coord &targetPosition, simtime_t &nextChange) {
    // если повторение маршрута ВЫКЛЮЧЕНО, то ходим по SLAW всё время
    if (!repetitionOfTraceEnabled)
        return SelfSimLATP::generateNextPosition(targetPosition, nextChange);

    // если повторение маршрута ВКЛЮЧЕНО, то проверяем наличие сохранённого трейса
    // если трейс ещё не сохранён, то ходим по SLAW, для генерации трейса
    if (!mvnHistoryForRepeat)
        return SelfSimLATP::generateNextPosition(targetPosition, nextChange);

    // если трейс уже сохранён, то ходим по нему, пока не закончиться

    ASSERT(0 <= step);
    if (step >= mvnHistoryForRepeat->getXCoordinates()->size())
        return false; // маршрут кончился

    if (isPause) {
        nextChange = simTime() + (mvnHistoryForRepeat->getOutTimes()->at(step) - mvnHistoryForRepeat->getInTimes()->at(step));
        ASSERT(nextChange == (timeOffset + mvnHistoryForRepeat->getOutTimes()->at(step)));

        // увеличивам шаг после пары "перешли и подождали"
        step++;
    } else {
        // сейчас НЕ пауза и мы выбираем новыую точку из Трейса
        const simtime_t previousNaxtChange = nextChange;
        nextChange = timeOffset + mvnHistoryForRepeat->getInTimes()->at(step);
        targetPosition.x = mvnHistoryForRepeat->getXCoordinates()->at(step);
        targetPosition.y = mvnHistoryForRepeat->getYCoordinates()->at(step);

        distance = lastPosition.distance(targetPosition);
        ASSERT(distance >= 0);

        travelTime = nextChange - previousNaxtChange;
        ASSERT(travelTime > 0);

        speed = distance / travelTime;
        ASSERT(speed >= 0);
    }
    isPause = !isPause;

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
     // для того, чтобы метод SelfSimLATP::makeNewRoot() отработал, нужно даже при влключённом повторении построить currentRoot.
     ASSERT(!currentRoot);
     if (!isRootReady) {
         unsigned int day = RoutingDaemon::instance->getCurrentDay();
         ASSERT(day >= 1);
         RootsCollection *collection = NULL;

         vector<RootsCollection *> *dailyRoot = rc->getDailyRoot();
         if (dailyRoot) {
             // если наборы маршрутов для КАЖДОГО дня есть, то ходим по ним

             // Тут номер дня может быть больше чем наборов для дней.
             // Поэтому, например, последний маршрут повторяем бесконечно
             if (day > dailyRoot->size()) day = dailyRoot->size();
             collection = dailyRoot->at(day-1);

         } else {
             // если наборо для каждого дня нет, то ходим КАЖДЫЙ день по одному ОБЩЕМУ набору
             collection = rc;
         }

         RootNumber = NodeID % collection->getRootsData()->size();
         ASSERT(RootNumber >= 0 && RootNumber < collection->getRootsData()->size());
         currentRoot = new vector<HotSpotData *>();
         for (unsigned int i = 0; i < collection->getRootsData()->at(RootNumber)->size(); i++) {
             currentRoot->push_back(new HotSpotData(collection->getRootsData()->at(RootNumber)->at(i)));
         }
         cout << "NodeId = " << NodeID << ": "  << "Root made for day " << day << endl;
     }
     isRootReady = true;


     if (repetitionOfTraceEnabled && mvnHistoryForRepeat) {
         // если при включённом повторении маршрута трейс уже сохранён, то берём начальную точку из него и выходим из функции
         unsigned int day = RoutingDaemon::instance->getCurrentDay();
         ASSERT(day >= 1);

         timeOffset = (day-1) * RoutingDaemon::instance->getDayDuration();
         step = 0;
         isPause = false;
         cout << "Repeatable Root made! day = " << day << ", NodeID = " << NodeID << ", simTime = " << simTime() << ", timeOffset = " << timeOffset << endl;

//         lastPosition.x = currentTrace->at(0).X;
//         lastPosition.y = currentTrace->at(0).Y;
//         lastPosition.z = 0;
//         targetPosition = lastPosition;
     }
}
