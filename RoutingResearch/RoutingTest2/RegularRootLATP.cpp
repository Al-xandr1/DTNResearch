#include "RegularRootLATP.h"

Define_Module(RegularRootLATP);

RegularRootLATP::RegularRootLATP()
{
    rc = NULL;
    rootStatistics  = NULL;
    rootPersistence = -1;
    rootGenerator   = NULL;

    firstRoot        = NULL;
    firstRootSnumber = NULL;
    firstRootCounter = NULL;

    homeHS = NULL;
    currentHSWaypointNum = 0;
    useWaypointCounter = true;

    curRootIndex       = -1;
    currentRoot        = NULL;
    currentRootSnumber = NULL;
    currentRootCounter = NULL;

    currentRootActualTrack  = NULL;
    currentRootCounterSAVED = NULL;

    isLProbReady = false;
    LocalProbMatrix = NULL;
}


void RegularRootLATP::loadFirstRoot()
{
    firstRoot = new vector<HotSpotData*>;
    firstRootCounter = new vector<int>;
    firstRootSnumber = new vector<unsigned int>;
    firstRootWptsPerVisit = new vector<int>;
    HotSpotData* h=NULL;
    int Snum=-1;

    // загрузка первого маршрута (эталона)
    vector<HotSpotDataRoot>* root = rc->getRootDataByNodeId(NodeID);
    // получение индекса в полной коллекции локаций
    h = hsc->findHotSpotbyName(root->at(0).hotSpotName, Snum);
//    ASSERT(h != NULL && Snum != -1); // если падает - возможно данные в hotspotfiles/ & rootfiles/ не согласованы
    firstRoot->push_back(h);
    firstRootSnumber->push_back(Snum);
    firstRootCounter->push_back(1);
    firstRootWptsPerVisit->push_back(root->at(0).waypointNum);

    for(unsigned int i=1; i<root->size(); i++ ) {
        h = hsc->findHotSpotbyName(root->at(i).hotSpotName, Snum);
        ASSERT(h); // обязаны найти эту локацию в полном наборе
        for(unsigned int j=0; j<firstRoot->size(); j++)
            if( firstRoot->at(j)==h ) {
                firstRootCounter->at(j)+=1;
                firstRootWptsPerVisit->at(j)+=(root->at(i).waypointNum);
                h=NULL;
            }
        if (h!=NULL) {
            firstRoot->push_back(h);
            firstRootSnumber->push_back(Snum);
            firstRootCounter->push_back(1);
            firstRootWptsPerVisit->push_back(root->at(i).waypointNum);
        }
    }

    // расчитываем среднее число эталонных посещений для локаций эталонного маршрута
    for(unsigned int i=0; i<firstRoot->size(); i++)
        firstRootWptsPerVisit->at(i) /= firstRootCounter->at(i); // todo сделать округление в бОльшую сторону

    // загрузка домашней локации
    homeHS = firstRoot->at(0);

    RootDataShort *rootDataShort = rc->getRootDataShortByNodeId(NodeID);
//    ASSERT(rootDataShort);
    // проставляем индивидуальное значение коэффициента персистентности, если оно есть
    if (rootDataShort->persistence) rootPersistence = *(rootDataShort->persistence);

    printFirstRoot();
}


void RegularRootLATP::printFirstRoot() {
    printRoot("First Root: ", firstRoot, firstRootSnumber, firstRootCounter, firstRootWptsPerVisit);
}


void RegularRootLATP::printCurrentRoot() {
    printRoot("Current Root: ", currentRoot, currentRootSnumber, currentRootCounter, currentRootWptsPerVisit);
}


void RegularRootLATP::printRoot(
        const char* lable,
        vector<HotSpotData*>* root,
        vector<unsigned int>* rootSnumber,
        vector<int>* rootCounter,
        vector<int>* rootWptsPerVisit)
{
    ASSERT(root != NULL && hsc != NULL);
    int totalRepeats = 0,
        totalWPTS    = 0;
    for(unsigned int i = 0; i < root->size(); i++) {
        cout << "NodeID = " << NodeID << ": "
                  << lable << (root->at(i))->hotSpotName
                  << " Snum=" << rootSnumber->at(i)
                  << " repeat=" << rootCounter->at(i)
                  << " wptsPerRepeat=" << rootWptsPerVisit->at(i) <<  endl;
        totalRepeats += rootCounter->at(i);
        totalWPTS += rootCounter->at(i) * rootWptsPerVisit->at(i);
    }
    int originTotalWPTS = 0;
    vector<HotSpotDataRoot>* originRoot = rc->getRootDataByNodeId(NodeID);
    for (unsigned int i = 0; i < originRoot->size(); i++)
        originTotalWPTS += originRoot->at(i).waypointNum;
    cout << "NodeID = " << NodeID << ":\t\t\t\t totalRepeats=" << totalRepeats
         << " totalWPTS=" << totalWPTS << " originTotalWPTS=" << originTotalWPTS << endl;
    cout << "NodeID = " << NodeID << ":\t\t\t\t rootPersistence = " << rootPersistence << endl;
    RootDataShort* originRootShort = rc->getRootDataShortByNodeId(NodeID);
    cout << "NodeID = " << NodeID << ":\t\t\t\t root filename = " << originRootShort->RootName << endl << endl;
}


void RegularRootLATP::makeLocalProbMatrix(double powA)
{
    if(!isLProbReady) {
        LocalProbMatrix = new double*[currentRoot->size()];
        for(unsigned int i=0; i<currentRoot->size(); i++) {
            LocalProbMatrix[i]= new double[currentRoot->size()];
            double h=0;
            for(unsigned int j=0; j<currentRoot->size(); j++) {
                unsigned int ii=currentRootSnumber->at(i);
                unsigned int jj=currentRootSnumber->at(j);
                if(  (i!=j)  && (currentRootCounter->at(j) > 0)  )
                    h += LocalProbMatrix[i][j] = pow(1/hsd->getDistance(ii,jj), powA);
                else LocalProbMatrix[i][j]=0;
            }
            for(unsigned int j=0; j<currentRoot->size(); j++) LocalProbMatrix[i][j]/=h;
        }
        isLProbReady=true;
    }
}


void RegularRootLATP::deleteLocalProbMatrix()
{
    if(isLProbReady) {
        for(unsigned int i=0; i<currentRoot->size(); i++) {
            delete[] LocalProbMatrix[i];
        }
        delete[] LocalProbMatrix;

        isLProbReady=false;
    }
}


void RegularRootLATP::initialize(int stage) {
    LevyHotSpotsLATP::initialize(stage);


    // загрузка данных об эталонных маршрутах
    if (!rc) rc = RootsCollection::getInstance();


    if (!rootStatistics) {
        // сначала читаем ИНДИВИДУАЛЬНЫЕ настройки
        // TODO ПРОВЕРИТЬ СООТВЕТСТВИЕ ЭТИХ НАСТРОЕК и ПРОЧИТАННОГО ЭТАЛОННОГО МАРШРУТА !!!
        rootStatistics = RootsPersistenceAndStatisticsCollection::getInstance()->findPersonalRootStatistics(NodeID);

        // сейчас делаем так, что ВСЕ должны прочитать индивидуальные настройки
        ASSERT(rootStatistics);
//        if (!rootStatistics)
//            // ... и если не смогли прочитать, то читаем ОБЩИЕ настройки
//            rootStatistics = RootsPersistenceAndStatisticsCollection::getInstance()->getCommonRootStatistics();
    }
    ASSERT(rootStatistics);


    if (rootPersistence == -1) {
        // при инициализации выставляем значение по умолчанию для узла.
        // Далее попробуем прочитать индивидуальное значение из названия файла со средним маршрутом
        rootPersistence = rootStatistics->getPersistenceFromMassCenter();
    }
    ASSERT(rootPersistence != -1);


    ASSERT(hsc);
    if (!rootGenerator) {
        if (getParentModule()->getParentModule()->par("useRootStatistics").boolValue())
            rootGenerator = new GenerationRootsByStatisticsStrategy(rootStatistics, hsc);
        else
            rootGenerator = new GenerationRootsByPersistenceStrategy(rootPersistence, hsc);
    }


    if (!firstRoot) loadFirstRoot();
    if (!currentRoot) {
        // первый раз ходим по эталонному маршруту
        currentRoot = new vector<HotSpotData*>(*firstRoot);
        currentRootSnumber = new vector<unsigned int>(*firstRootSnumber);
        currentRootCounter = new vector<int>(*firstRootCounter);
        currentRootWptsPerVisit = new vector<int>(*firstRootWptsPerVisit);
        currentRootCounterSAVED = new vector<int>(*currentRootCounter);
        currentRootActualTrack = new vector<unsigned int>();

        // начальнаЯ локациЯ - это перваЯ локациЯ текущего маршрута
        setCurRootIndex(0, true);
        //printCurrentRoot();

        // Сохраняем для статистики сгенерированный маршрут для первого дня в RootsCollection.
        const int currentDay = RoutingDaemon::instance->getCurrentDay();
        ASSERT(currentDay == 0);
        RootsCollection::getInstance()->collectTheoryRoot(currentRoot, currentRootSnumber, currentRootCounter, NodeID, currentDay+1);
    }

    if (!LocalProbMatrix) makeLocalProbMatrix(powA);

    useWaypointCounter = getParentModule()->par("useWaypointCounter").boolValue();
}


void RegularRootLATP::setCurRootIndex(unsigned int currentRootIndex, bool writeIndexToTrack)
{
    curRootIndex = currentRootIndex;
    currentHSindex = currentRootSnumber->at(curRootIndex);

    LevyHotSpotsLATP::setCurrentHSbordersWith(currentRoot->at(curRootIndex));
    //нужно заново выставить текущий счётчик на среднее значение,
    //т.к. посещение этой локации может быть второй раз
    currentHSWaypointNum = currentRootWptsPerVisit->at(curRootIndex);

    // локация в нулевом индексе - это должна быть домашняя локация
    if (curRootIndex == 0) ASSERT(homeHS == currentRoot->at(curRootIndex));

    if (writeIndexToTrack) {
        ASSERT(currentRootActualTrack);
        ASSERT(curRootIndex >= 0);
        currentRootActualTrack->push_back(curRootIndex);
    }
}


void RegularRootLATP::handleMessage(cMessage * message)
{
    if (message->isSelfMessage())
        MobilityBase::handleMessage(message);
    else
        switch (message->getKind()) {
            // используется для "пинка" для мобильности, чтобы снова начать ходить
            case MOBILITY_START:{
                nextChange = simTime();
                MovingMobilityBase::scheduleUpdate();
                emitMobilityStateChangedSignal();
//                ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));
                break;
            }
            default:
                ASSERT(false); //unreacheble statement
        }
}


void RegularRootLATP::setTargetPosition() {
    LevyHotSpotsLATP::setTargetPosition();

    if (movementsFinished) {
        // очищают статус и планируем в бесконечность - чтобы приостановить, но не завершить
        movementsFinished = false;
        nextChange = MAXTIME;
    }
}


bool RegularRootLATP::findNextHotSpot()
{
    if (currentRootCounter->at(curRootIndex) > 0)
        (*currentRootCounter)[curRootIndex]-=1;  // покидаЯ локацию, уменьшаем еЮ счЮтчик посещений

    unsigned int hh=0, ii;                               // находим сумму всех счЮтчиков посещений на маршруте,
    for(unsigned int i=0; i<currentRoot->size(); i++)    // чтобы определить, когда конец маршрута
        if (currentRootCounter->at(i) > 0) {
            ii=i; hh+=currentRootCounter->at(i);
        }

    if( hh == 0 ) return false;                          // маршрут кончилсЯ
    if( hh == currentRootCounter->at(ii) ) {             // осталась одна локациЯ (может быть, с несколькими посещениЯми)
        setCurRootIndex(ii, true);
        (*currentRootCounter)[curRootIndex]=1;           // если посещений несколько, заменЯем одним

        return true;
    }

    if(currentRootCounter->at(curRootIndex) == 0) {       // пересчитываем матрицу вероЯтностей переходов, если счЮтчик посещений обнулилсЯ
        for(unsigned int i=0; i<currentRoot->size(); i++) {
            LocalProbMatrix[i][curRootIndex]=0;
            double h=0;
            for(unsigned int j=0; j<currentRoot->size(); j++) h+=LocalProbMatrix[i][j];
            if ( h!=0 ) for(unsigned int j=0; j<currentRoot->size(); j++) LocalProbMatrix[i][j]/=h;
            else return false; // все вероЯтности перехода уже нули, мы в конце маршрута (на всЯкий случай)
        }
    }
    // случайным образом выбираем новую локацию
    double rn, pr=0;
    rn=(double)rand()/RAND_MAX;
    for(unsigned int i=0; i<currentRoot->size(); i++) {
        if(curRootIndex != i) pr+=LocalProbMatrix[curRootIndex][i];
        if(rn <= pr) {
            setCurRootIndex(i, true);
            break;
        }
    }
//    ASSERT(rn <= pr);

    //    cout << "findNextHotSpot: changing location to" << currentHSindex << endl;
    return true;
}


bool RegularRootLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound)
{
    if (useWaypointCounter) {
        // используем счётчки
        ASSERT(currentHSWaypointNum >= 0);
        if (currentHSWaypointNum != 0) {
            // если счётчик ещё не равен 0, то идём по маршруту
            bool nextPosFound=LevyHotSpotsLATP::generateNextPosition(targetPosition, nextChange, true);
            // если счётчик нас пропустил дальше, то по любому должны найти путевую точку
            ASSERT(nextPosFound);
            // уменьшаем счётчик количества путевых точек
            currentHSWaypointNum -= 1;
            return true;
        }

        ASSERT(currentHSWaypointNum == 0);
        //если счётчик равен 0, то пора менять локацию
        if (LevyHotSpotsLATP::findNextHotSpotAndTargetPosition()) {
            // ...смогли поменять локацию
            ASSERT(currentHSWaypointNum > 0);
            // уменьшаем счётчик количества путевых точек, т.к. только что выбрали путевую точку
            currentHSWaypointNum -= 1;
            return true;
        }
        // ... если не смогли поменять локацию, то маршрут закончился - идём домой

    } else {
        // если не используем счётчик, то генерируем через базовую реализацию
        if (LevyHotSpotsLATP::generateNextPosition(targetPosition, nextChange)) {
            // ... и если находим точку, заканчиваем метод.
            return true;
        }
        // если не находим точку (и локацию, поиск которой происходит внутри метода), то завершаем маршрут
    }

    // маршрут кончился, идём домой
    if (useWaypointCounter) ASSERT(currentHSWaypointNum == 0);
    setCurRootIndex(0, false);

    // проверЯем, не дома ли мы уже
    if( currentHSMin.x <= lastPosition.x &&  lastPosition.x <= currentHSMax.x &&
        currentHSMin.y <= lastPosition.y &&  lastPosition.y <= currentHSMax.y ) {

        ASSERT(isRootFinished());
        (check_and_cast<MobileHost*>(getParentModule()))->ensureEndRoute();
        return false;
    }

    // если нет - идём домой
    targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
    targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);

    distance = sqrt( (targetPosition.x-lastPosition.x)*(targetPosition.x-lastPosition.x)+(targetPosition.y-lastPosition.y)*(targetPosition.y-lastPosition.y) );
//    ASSERT(distance > 0);
    speed = kForSpeed * pow(distance, 1 - roForSpeed);
    travelTime = distance / speed;
    nextChange = simTime() + travelTime;
    return true;
}


bool RegularRootLATP::isRootFinished() {
    ASSERT(currentRoot->size() > 0);
    ASSERT(homeHS == currentRoot->at(0));
    bool finished = true;
    for (unsigned int i=1; i<currentRootCounter->size(); i++) {
        finished &= (currentRootCounter->at(i) == 0);
    }
    if (!finished) {
        cout<<"NodeId="<<NodeID<<", size="<<currentRoot->size()<<": ";
        for (unsigned int i=0; i<currentRootCounter->size(); i++) cout<<currentRootCounter->at(i)<<", ";
        cout << endl;
    }
    return finished;
}


void RegularRootLATP::makeNewRoot()
{
    const int currentDay = RoutingDaemon::instance->getCurrentDay();
    ASSERT(currentDay >= 2); // Начинается создание новых маршрутов со второго дня
    cout << endl << "Making new root for NodeID: " << NodeID << " at day: " << currentDay << endl;


    if(currentRoot != NULL) {
        /* Сохраняем для статистики сгенерированный маршрут, который реально был пройден (маршрут от предыдущего дня).
           Сохранение тут происходит со второго дня. Первый день сохраняется сразу после создания в initialize.
         */
        for(unsigned int i = 0; i < currentRootCounterSAVED->size(); i++) {
            // вычисляем фактически пройденный маршрут как actualCurrentRootCounter = currentRootCounterSAVED - currentRootCounter
            (*currentRootCounterSAVED)[i] -= (*currentRootCounter)[i];
//            ASSERT((*currentRootCounterSAVED)[i] >= 0);
        }

        const int counterSum = getSum(*currentRootCounterSAVED);
        ASSERT(counterSum >= 0);
        // Каждое посещение локации фиксируется в currentRootActualTrack.
        // Однако в RegularRootLATP::findNextHotSpot последняя локация с несколькими посещениесми заменяется одним посещением
        // Поэтому сумма всех посещений, должна быть больше или равна длине трека
        ASSERT(((unsigned int) counterSum) >= currentRootActualTrack->size());

        RootsCollection::getInstance()->collectActualRoot(currentRoot, currentRootSnumber, currentRootCounterSAVED, currentRootActualTrack, NodeID, currentDay - 1);
        cout << endl << "Saved old root for NodeID: " << NodeID << " at previous day: " << (currentDay - 1) << endl;

        // удаляем старый маршрут
        deleteLocalProbMatrix();
        myDelete(currentRoot);
        myDelete(currentRootSnumber);
        myDelete(currentRootCounter);
        myDelete(currentRootWptsPerVisit);
        myDelete(currentRootCounterSAVED);
        myDelete(currentRootActualTrack);
    }


    rootGenerator->generateNewRoot(
            firstRoot, firstRootSnumber, firstRootCounter, firstRootWptsPerVisit,
            currentRoot, currentRootSnumber, currentRootCounter, currentRootWptsPerVisit);


    // for debug
//    ASSERT(currentRoot && currentRootSnumber && currentRootCounter && currentRootWptsPerVisit);
//    printFirstRoot();
//    printCurrentRoot();
//    // суммы посещений должно быть больше нуля в маршрутах
//    ASSERT(getSum(*currentRootCounter) > 0);
//    ASSERT(getSum(*firstRootCounter) > 0);
//    // все структуры одинаковы по размеру
//    ASSERT((currentRoot->size() == currentRootSnumber->size())
//            && (currentRoot->size() == currentRootCounter->size())
//            && (currentRoot->size() == currentRootWptsPerVisit->size()));


    makeLocalProbMatrix(powA);
    // начальнаЯ локациЯ - это перваЯ локациЯ текущего маршрута - она же домашняя
    currentRootActualTrack = new vector<unsigned int>();
    setCurRootIndex(0, true);
    targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
    targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);


    /* Сохраняем для статистики сгенерированный маршрут для текущего дня в RootsCollection.
       Сохранение тут происходит со второго дня. Первый день сохраняется сразу после создания в initialize.
       Тут сохраняется только что созданный маршут, по которому ещё не ходили.
     */
    RootsCollection::getInstance()->collectTheoryRoot(currentRoot, currentRootSnumber, currentRootCounter, NodeID, currentDay);
    currentRootCounterSAVED = new vector<int>(*currentRootCounter);
}
