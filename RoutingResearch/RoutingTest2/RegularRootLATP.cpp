#include "RegularRootLATP.h"

Define_Module(RegularRootLATP);

RegularRootLATP::RegularRootLATP()
{
    rc = NULL;

    rootPersistence = -1;

    firstRoot        = NULL;
    firstRootSnumber = NULL;
    firstRootCounter = NULL;

    homeHS = NULL;
    currentHSWaypointNum = 0;

    currentRoot        = NULL;
    currentRootSnumber = NULL;
    currentRootCounter = NULL;

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
//    ASSERT(h != NULL && Snum != -1);
    firstRoot->push_back(h);
    firstRootSnumber->push_back(Snum);
    firstRootCounter->push_back(1);
    firstRootWptsPerVisit->push_back(root->at(0).waypointNum);

    for(unsigned int i=1; i<root->size(); i++ ) {
        h = hsc->findHotSpotbyName(root->at(i).hotSpotName, Snum);
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
    // printFirstRoot();
}


void RegularRootLATP::printFirstRoot()
{
    if( firstRoot != NULL && hsc != NULL)
        for(unsigned int i=0; i<firstRoot->size(); i++) {
            std::cout << NodeID
                    << " First Root: " << (firstRoot->at(i))->hotSpotName
                    << " Snum=" << firstRootSnumber->at(i)
                    << " repeat=" << firstRootCounter->at(i)
                    << " wptsPerRepeat=" << firstRootWptsPerVisit->at(i) <<  endl;
        }
}


void RegularRootLATP::printCurrentRoot()
{
    if( currentRoot != NULL && hsc != NULL)
        for(unsigned int i=0; i<currentRoot->size(); i++) {
            std::cout << NodeID
                    << " Current Root: " << (currentRoot->at(i))->hotSpotName
                    << " Snum=" << currentRootSnumber->at(i)
                    << " repeat=" << currentRootCounter->at(i)
                    << " wptsPerRepeat=" << currentRootWptsPerVisit->at(i) << endl;
        }
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

    if (rootPersistence == -1)
        rootPersistence = getParentModule()->par("rootPersistence").doubleValue();

    if (!firstRoot) loadFirstRoot();

    if (!currentRoot) {
        // первый раз ходим по эталонному маршруту
        currentRoot = new vector<HotSpotData*>(*firstRoot);
        currentRootSnumber = new vector<unsigned int>(*firstRootSnumber);
        currentRootCounter = new vector<int>(*firstRootCounter);
        currentRootWptsPerVisit = new vector<int>(*firstRootWptsPerVisit);

        // начальнаЯ локациЯ - это перваЯ локациЯ текущего маршрута
        curRootIndex=0;
        RegularRootLATP::setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
        hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex); // todo remove. Made currentHSindex=currentRootSnumber->at(curRootIndex)
//        ASSERT(currentRootSnumber->at(curRootIndex) == currentHSindex);

        //printCurrentRoot();
    }

    if (!LocalProbMatrix) makeLocalProbMatrix(powA);
}


void RegularRootLATP::setCurrentHSbordersWith(HotSpotData* hsi)
{
    LevyHotSpotsLATP::setCurrentHSbordersWith(hsi);
    //нужно заново выставить текущий счётчик на среднее значение,
    //т.к. посещение этой локации может быть второй раз
    currentHSWaypointNum = currentRootWptsPerVisit->at(curRootIndex);
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
    if (currentRootCounter->at(curRootIndex) > 0) (*currentRootCounter)[curRootIndex]-=1;  // покидаЯ локацию, уменьшаем еЮ счЮтчик посещений

    unsigned int hh=0, ii;                               // находим сумму всех счЮтчиков посещений на маршруте,
    for(unsigned int i=0; i<currentRoot->size(); i++)    // чтобы определить, когда конец маршрута
        if (currentRootCounter->at(i) > 0) { ii=i; hh+=currentRootCounter->at(i); }

    if( hh == 0 ) return false;                          // маршрут кончилсЯ
    if( hh == currentRootCounter->at(ii) ) {             // осталась одна локациЯ (может быть, с несколькими посещениЯми)
        curRootIndex = ii;
        (*currentRootCounter)[curRootIndex]=1;           // если посещений несколько, заменЯем одним
        RegularRootLATP::setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
        hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex); // todo remove. Made currentHSindex=currentRootSnumber->at(curRootIndex)
//        ASSERT(currentRootSnumber->at(curRootIndex) == currentHSindex);
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
        if(rn <= pr) { curRootIndex=i; break; }
    }
//    ASSERT(rn <= pr);
    RegularRootLATP::setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
    hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex); // todo remove. Made currentHSindex=currentRootSnumber->at(curRootIndex)
//    ASSERT(currentRootSnumber->at(curRootIndex) == currentHSindex);
    //    cout << "findNextHotSpot: changing location to" << currentHSindex << endl;
    return true;
}


bool RegularRootLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound)
{
    ASSERT(currentHSWaypointNum >= 0);
    if (currentHSWaypointNum == 0) {
        //если счётчик равен 0, то пора менять локацию
        if (LevyHotSpotsLATP::findNextHotSpotAndTargetPosition()) {
            ASSERT(currentHSWaypointNum > 0);
            // уменьшаем счётчик количества путевых точек
            currentHSWaypointNum -= 1;
            return true;
        }

    } else {
        // идём по маршруту
        bool nextPosFound=LevyHotSpotsLATP::generateNextPosition(targetPosition, nextChange, true);
        // если счётчик нас пропустил дальше, то по любому должны найти путевую точку
        ASSERT(nextPosFound);
        // уменьшаем счётчик количества путевых точек
        currentHSWaypointNum -= 1;
        return true;
    }

    // маршрут кончился, идём домой
//    ASSERT(currentHSWaypointNum == 0);
    currentHSindex=0;
    RegularRootLATP::setCurrentHSbordersWith( homeHS );

    // проверЯем, не дома ли мы уже
    if( currentHSMin.x <= lastPosition.x &&  lastPosition.x <= currentHSMax.x &&
        currentHSMin.y <= lastPosition.y &&  lastPosition.y <= currentHSMax.y ) {

        ASSERT(isRootFinished());
        (check_and_cast<MobileHost*>(getParentModule()))->endRoute();
        return false;
    }

    // если нет - идЮм домой
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
    ASSERT(currentRoot->size() > 1);
    ASSERT(homeHS == currentRoot->at(0));
    bool finished = true;
    for (unsigned int i=1; i<currentRootCounter->size(); i++) {
        finished &= (currentRootCounter->at(i) == 0);
    }
    if (finished) {
        cout<<"NodeId="<<NodeID<<", size="<<currentRoot->size()<<": ";
        for (unsigned int i=0; i<currentRootCounter->size(); i++) cout<<currentRootCounter->at(i)<<", ";
        cout << endl;
    }
    return finished;
}


void RegularRootLATP::makeNewRoot()
{
    cout << "Making new root for NodeID: " << NodeID << endl;

    if(currentRoot != NULL) {
        deleteLocalProbMatrix();
        delete currentRoot;
        delete currentRootSnumber;
        delete currentRootCounter;
        delete currentRootWptsPerVisit;
    }

    currentRoot        = new vector<HotSpotData*>(*firstRoot);
    currentRootSnumber = new vector<unsigned int>(*firstRootSnumber);
    currentRootCounter = new vector<int>(*firstRootCounter);
    currentRootWptsPerVisit = new vector<int>(*firstRootWptsPerVisit);
    vector<int>* unusedWptsPerVisit = new vector<int>();

    unsigned int ii=0;
    for (unsigned int i=0; i<firstRoot->size(); i++) ii+=firstRootCounter->at(i);
    int replaceCount = ii - round( ii * rootPersistence);
//    ASSERT(0 <= replaceCount && replaceCount <= ii);
    // уменьшаем счЮтчики посещений у заданного числа случайных локаций на маршруте.
    // у первой локации (домашней), счЮтчик нулЮм не делаем.
    int rem, remCount=replaceCount;
    while( remCount > 0 ) {
        rem = rand() % currentRoot->size();
        if ( (rem == 0 && currentRootCounter->at(0) > 1) || (rem > 0 && currentRootCounter->at(rem) > 0) ) {
            currentRootCounter->at(rem)--;
            remCount--;
            // копим путевые точки, которые удаляем из маршрута: каждое удаление посещения локации
            // означает удаление из текущего маршрута среднего числа точек за это посещение,
            // - и сохранения для последующей вставки в новый маршрут
            unusedWptsPerVisit->push_back(currentRootWptsPerVisit->at(rem));
        }
    }
    // домашняя локация сохраняется
//    ASSERT(currentRootCounter->at(0) >= 1);
//    ASSERT(unusedWptsPerVisit->size() == replaceCount);

    // инициализируем набор возможных локаций всеми возможными номерами локаций в порЯдке возрастаниЯ
    vector<int> possibleReplace;
    for(unsigned int i=0; i< hsc->getHSData()->size(); i++) possibleReplace.push_back(i);
    vector<unsigned int>* cur = new vector<unsigned int> (*currentRootSnumber);
    // удалЯем номера, присутствующие в маршруте, из possibleReplace, начинаЯ с самого большого в cur,
    // чтобы корректно работал erase в possibleReplace - т.к. в таком случае удаление будет проходить с конца
    while( cur->size() > 0 ) {
        for(unsigned int i=1; i<cur->size(); i++) if( cur->at(0) < cur->at(i) ) {
            unsigned int tmp=cur->at(0); cur->at(0)=cur->at(i); cur->at(i)=tmp;
        }
        possibleReplace.erase( possibleReplace.begin() + cur->at(0) );
        cur->erase(cur->begin());
    }
    delete cur;
//    ASSERT(possibleReplace.size() == (hsc->getHSData()->size() - currentRootSnumber->size()));

    // генерируем нужное число случайных номеров новых локаций
    // из тех, что остались в possibleReplace в отсортированном по возрастанию виде
    vector<int> sortReplace;
    unsigned int ri = rand() % possibleReplace.size();
    sortReplace.push_back(possibleReplace[ri]);
    unsigned int replaceCountForCheck = replaceCount;
    replaceCount--;
    while( replaceCount > 0) {
        unsigned int i;
        ri = rand() % possibleReplace.size();
        for(i=0; i<sortReplace.size(); i++) {
            if( possibleReplace[ri] <= sortReplace[i] ) {
                sortReplace.insert(sortReplace.begin()+i, possibleReplace[ri]);
                break;
            }
        }
        if( i == sortReplace.size() ) sortReplace.push_back(possibleReplace[ri]);
        replaceCount--;
    }
//    ASSERT(sortReplace.size() == replaceCountForCheck);

    // добавляем нужное число новых локаций в маршрут
    for(unsigned int i=0; i<sortReplace.size(); i++) {
       unsigned int hsNumber=sortReplace[i];
       HotSpotData* hs = &(hsc->getHSData()->at(hsNumber));
       ASSERT(currentRootCounter->size() == currentRootWptsPerVisit->size());
       if( hsNumber != currentRootSnumber->back() ) {
           currentRoot->push_back(hs);
           currentRootSnumber->push_back(hsNumber);
           currentRootCounter->push_back(1);
           // Вставляем сохраненые посещения в новый маршрут: если локация "пустая" то пишем 1, иначе сохранённое число
           currentRootWptsPerVisit->push_back(hs->isHotSpotEmpty() ? 1 : unusedWptsPerVisit->front()); // todo если пишем 1, то остальные пропадают - исправить
       } else {
           currentRootCounter->back()++;
           // Вставляем сохраненые посещения в новый маршрут: если локация "пустая" то пишем 1, иначе сохранённое число
           currentRootWptsPerVisit->back() += (hs->isHotSpotEmpty() ? 1 : unusedWptsPerVisit->front()); // todo если пишем 1, то остальные пропадают - исправить
       }
       // удалёям использованный точки из "памяти"
       unusedWptsPerVisit->erase(unusedWptsPerVisit->begin());
    }
    delete unusedWptsPerVisit;

    // for debug
//    printFirstRoot();
//    printCurrentRoot();
//    unsigned int sumCurrent=0, sumFirst=0;
//    for(unsigned int i=0; i<currentRootCounter->size(); i++) sumCurrent += currentRootCounter->at(i);
//    for(unsigned int i=0; i<firstRootCounter->size();   i++) sumFirst +=   firstRootCounter->at(i);
//    ASSERT(sumCurrent == sumFirst);
//    // все структуры одинаковы по размеру
//    ASSERT((currentRoot->size() == currentRootSnumber->size()) && (currentRoot->size() == currentRootCounter->size()));

    makeLocalProbMatrix(powA);

    // начальнаЯ локациЯ - это перваЯ локациЯ текущего маршрута - она же домашняя
    curRootIndex=0;
//    ASSERT(homeHS == currentRoot->at(curRootIndex));
    RegularRootLATP::setCurrentHSbordersWith(homeHS);
    HotSpotData* hsi = hsc->findHotSpotbyName(homeHS->hotSpotName, currentHSindex); // todo remove. Made currentHSindex=currentRootSnumber->at(curRootIndex)
//    ASSERT(hsi);
    ASSERT(currentRootSnumber->at(curRootIndex) == currentHSindex);

    targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
    targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);
}
