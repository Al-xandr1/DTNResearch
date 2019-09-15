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
            if (!mvnHistoryForRepeat) {
                mvnHistoryForRepeat = new MovementHistory(*getMovementHistory());

                // for debug
//                const char *wpsDir = "outTrace/first_day_wps";
//                const char *trsDir = "outTrace/first_day_trs";
//                if (CreateDirectory(wpsDir, NULL)) cout << "create output directory: " << wpsDir << endl;
//                else cout << "error create output directory: " << wpsDir << endl;
//                if (CreateDirectory(trsDir, NULL)) cout << "create output directory: " << trsDir << endl;
//                else cout << "error create output directory: " << trsDir << endl;
//                mvnHistoryForRepeat->save(wpsDir, trsDir);
//                log(string("RegularSelfSimLATP::setTargetPosition: first trace saved! repetitionOfTraceEnabled = ") + std::to_string(repetitionOfTraceEnabled));
                // for debug
            }
        }

        // очищают статус и планируем в бесконечность - чтобы приостановить, но не завершить
        endRoute();
        movementsFinished = false;
        nextChange = MAXTIME;
    }
}

bool RegularSelfSimLATP::generatePause(simtime_t &nextChange) {
    // если повторение маршрута ВЫКЛЮЧЕНО, то ходим по SLAW всё время
    if (!repetitionOfTraceEnabled)
        return SelfSimLATP::generatePause(nextChange);

    // если повторение маршрута ВКЛЮЧЕНО, то проверяем наличие сохранённого трейса
    // если трейс ещё не сохранён, то ходим по SLAW, для генерации трейса
    if (!mvnHistoryForRepeat)
        return SelfSimLATP::generatePause(nextChange);


    // если трейс уже сохранён, то ходим по нему, пока не закончиться. Берём паузу
    long step = (getStep() - 1) % mvnHistoryForRepeat->getSize();
    setWaitTime(mvnHistoryForRepeat->getOutTimes()->at(step) - mvnHistoryForRepeat->getInTimes()->at(step));
    nextChange = simTime() + getWaitTime();
    ASSERT(nextChange == (timeOffset + mvnHistoryForRepeat->getOutTimes()->at(step)));
    log(string("RegularSelfSimLATP::generateNextPosition: pause timeOffset = " + std::to_string(timeOffset.dbl())) + string(", step = ") + std::to_string(step));
}


bool RegularSelfSimLATP::generateNextPosition(Coord &targetPosition, simtime_t &nextChange) {
    // если повторение маршрута ВЫКЛЮЧЕНО, то ходим по SLAW всё время
    if (!repetitionOfTraceEnabled)
        return SelfSimLATP::generateNextPosition(targetPosition, nextChange);

    // если повторение маршрута ВКЛЮЧЕНО, то проверяем наличие сохранённого трейса
    // если трейс ещё не сохранён, то ходим по SLAW, для генерации трейса
    if (!mvnHistoryForRepeat)
        return SelfSimLATP::generateNextPosition(targetPosition, nextChange);


    // если трейс уже сохранён, то ходим по нему, пока не закончиться. Берём позицию
    long step = (getStep() - 1) % mvnHistoryForRepeat->getSize();
    if (step == 0)
        return false; // нулевую точку мы устанавливаем вручную. Считаем, что маршрут кончился.

    // сейчас НЕ пауза и мы выбираем новую точку из Трейса
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
    log(string("RegularSelfSimLATP::generateNextPosition: move timeOffset = " + std::to_string(timeOffset.dbl())) + string(", step = ") + std::to_string(step));

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
         log(string("Root made for day = ") + std::to_string(day));
     }
     isRootReady = true;


     if (repetitionOfTraceEnabled && mvnHistoryForRepeat) {
         // если при включённом повторении маршрута трейс уже сохранён, то берём начальную точку из него
         unsigned int day = RoutingDaemon::instance->getCurrentDay();
         ASSERT(day >= 1);

         timeOffset = (day-1) * RoutingDaemon::instance->getDayDuration();
         log(string("Repeatable Root made! day = ") + std::to_string(day)
             + string(", simTime = ") +  std::to_string(simTime().dbl())
             + string(", timeOffset = ") + std::to_string(timeOffset.dbl()));

         long step = (getStep() - 1) % mvnHistoryForRepeat->getSize();
         ASSERT(step == 0);
         lastPosition.x = mvnHistoryForRepeat->getXCoordinates()->at(step);
         lastPosition.y = mvnHistoryForRepeat->getYCoordinates()->at(step);
         lastPosition.z = 0;
         targetPosition = lastPosition;
     }
}
