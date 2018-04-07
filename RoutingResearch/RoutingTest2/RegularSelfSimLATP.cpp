#include <RegularSelfSimLATP.h>

Define_Module(RegularSelfSimLATP);

void RegularSelfSimLATP::initialize(int stage) {
    SelfSimLATP::initialize(stage);
}

void RegularSelfSimLATP::handleMessage(cMessage *message) {
    if (message->isSelfMessage())
        MobilityBase::handleMessage(message);
    else
        switch (message->getKind()) {
            // используется для "пинка" для мобильности, чтобы снова начать ходить
            case MOBILITY_START:{
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
    SelfSimLATP::setTargetPosition();

    if (movementsFinished) {
        // очищают статус и планируем в бесконечность - чтобы приостановить, но не завершить
        movementsFinished = false;
        nextChange = MAXTIME;
    }
}

 void RegularSelfSimLATP::makeRoot() {
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
}
