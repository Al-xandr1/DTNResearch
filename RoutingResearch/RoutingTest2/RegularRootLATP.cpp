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

    currentRoot        = NULL;
    currentRootSnumber = NULL;
    currentRootCounter = NULL;

    isLProbReady = false;
    LocalProbMatrix = NULL;
}


void RegularRootLATP::loadFirstRoot()
{
    firstRoot = new vector<HotSpotShortInfo*>;
    firstRootCounter = new vector<int>;
    firstRootSnumber = new vector<unsigned int>;
    HotSpotShortInfo* h=NULL;
    int Snum=-1;

    // загрузка первого маршрута (эталона)
    h = hsc->findHotSpotbyName((rc->RootData[NodeID]).hotSpot[0], Snum);
//    ASSERT(h != NULL);
    firstRoot->push_back(h);
    firstRootSnumber->push_back(Snum);
    firstRootCounter->push_back(1);

    for(int i=1; i<(rc->RootData[NodeID]).length; i++ ) {
        h = hsc->findHotSpotbyName((rc->RootData[NodeID]).hotSpot[i], Snum);
        for(unsigned int j=0; j<firstRoot->size(); j++)
            if( firstRoot->at(j)==h ) { firstRootCounter->at(j)+=1; h=NULL; }
        if (h!=NULL) {
            firstRoot->push_back(h);
            firstRootSnumber->push_back(Snum);
            firstRootCounter->push_back(1);
        }
    }

    // загрузка домашней локации
    homeHS = firstRoot->at(0);
}


void RegularRootLATP::printFirstRoot()
{
    if( firstRoot != NULL && hsc != NULL)
        for(unsigned int i=0; i<firstRoot->size(); i++) {
            std::cout << NodeID << " First Root: "<< (firstRoot->at(i))->hotSpotName << " Snum=" << firstRootSnumber->at(i) <<" repeat=" << firstRootCounter->at(i) << endl;
        }
}


void RegularRootLATP::printCurrentRoot()
{
    if( currentRoot != NULL && hsc != NULL)
        for(unsigned int i=0; i<currentRoot->size(); i++) {
            std::cout << NodeID << " Current Root: "<< (currentRoot->at(i))->hotSpotName << " Snum=" << currentRootSnumber->at(i) <<" repeat=" << currentRootCounter->at(i) << endl;
        }
}


void RegularRootLATP::makeLocalProbMatrix(double powA)
{
    if(hsd->isMatrixReady && !isLProbReady) {
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
    if (rc==NULL) {
        rc = new RootsCollection();
        rc->readRootsInfo(DEF_TR_DIR, ALLROOTS_FILE);
        // rc->print();
    }

    if (rootPersistence == -1)
        rootPersistence = getParentModule()->par("rootPersistence").doubleValue();

    if (firstRoot == NULL) {
        loadFirstRoot();
        // printFirstRoot();
    }

    if (currentRoot == NULL) {
        // первый раз ходим по эталонному маршруту
        currentRoot = new vector<HotSpotShortInfo*>(*firstRoot);
        currentRootSnumber = new vector<unsigned int>(*firstRootSnumber);
        currentRootCounter = new vector<int>(*firstRootCounter);

        // начальнаЯ локациЯ - это перваЯ локациЯ текущего маршрута
        curRootIndex=0;
        LevyHotSpotsLATP::setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
        hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex);

        // printCurrentRoot();
    }

    if (LocalProbMatrix == NULL) makeLocalProbMatrix(powA);
}


bool RegularRootLATP::findNextHotSpot()
{
    if (currentRootCounter->at(curRootIndex) > 0) (*currentRootCounter)[curRootIndex]-=1;  // покидаЯ локацию, уменьшаем еЮ счЮтчик посещений

    unsigned int hh=0, ii;                               // находим сумму всех счЮтчиков посещений на маршруте,
    for(unsigned int i=0; i<currentRoot->size(); i++)    // чтобы определить, когда конец маршрута
        if (currentRootCounter->at(i) > 0) { ii=i; hh+=currentRootCounter->at(i); }

    if( hh == 0 ) return false;                          // маршрут кончилсЯ
    if( hh == currentRootCounter->at(ii) ) {              // осталась одна локациЯ (может быть, с несколькими посещениЯми)
        curRootIndex = ii;
        (*currentRootCounter)[curRootIndex]=1;           // если посещений несколько, заменЯем одним
        LevyHotSpotsLATP::setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
        hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex);
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
    LevyHotSpotsLATP::setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
    hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex);
    //    cout << "findNextHotSpot: changing location to" << currentHSindex << endl;
    return true;
}


bool RegularRootLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange)
{
    bool flag=LevyHotSpotsLATP::generateNextPosition(targetPosition, nextChange);
    if (flag) return true;   // идЮм по маршруту
    else {                   // маршрут кончилсЯ, идЮм домой
        currentHSindex=0;
        LevyHotSpotsLATP::setCurrentHSbordersWith( homeHS );

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
//        ASSERT(distance > 0);
        speed = kForSpeed * pow(distance, 1 - roForSpeed);
        travelTime = distance / speed;
        nextChange = simTime() + travelTime;
        return true;
    }
}


bool RegularRootLATP::isRootFinished() {
    //todo проверить правильность определения конца маршрута
    ASSERT(currentRoot->size() > 1);
    ASSERT(homeHS == currentRoot->at(0));
    //if (currentRootCounter->at(0) < 1) cout<<"!!! currentRootCounter->at(0)=" << currentRootCounter->at(0) << endl;
    //ASSERT(currentRootCounter->at(0) >= 1); тут падает ошибка...
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
    }

    currentRoot        = new vector<HotSpotShortInfo*>(*firstRoot);
    currentRootSnumber = new vector<unsigned int>(*firstRootSnumber);
    currentRootCounter = new vector<int>(*firstRootCounter);

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
        }
    }
    // домашняя локация сохраняется
//    ASSERT(currentRootCounter->at(0) >= 1);

    // инициализируем набор возможных локаций всеми возможными номерами локаций в порЯдке возрастаниЯ
    vector<int> possibleReplace;
    for(unsigned int i=0; i< HotSpotsCollection::HSData.size(); i++) possibleReplace.push_back(i);
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
//    ASSERT(possibleReplace.size() == (HotSpotsCollection::HSData.size() - currentRootSnumber->size()));

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

    // добавлЯем нужное число новых локаций в маршрут
    for(unsigned int i=0; i<sortReplace.size(); i++) {
       unsigned int hsNumber=sortReplace[i];
       if( hsNumber != currentRootSnumber->back() ) {
           currentRoot->push_back(&(HotSpotsCollection::HSData[hsNumber]));
           currentRootSnumber->push_back(hsNumber);
           currentRootCounter->push_back(1);
       } else currentRootCounter->back()++;
    }

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
    HotSpotShortInfo* homeHS = currentRoot->at(curRootIndex);
//    ASSERT(this->homeHS == homeHS);
    LevyHotSpotsLATP::setCurrentHSbordersWith(homeHS);
    HotSpotShortInfo* hsi = hsc->findHotSpotbyName(homeHS->hotSpotName, currentHSindex);
//    ASSERT(hsi);

    targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
    targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);
    //todo стоит ли менять время прибытия?

    emitMobilityStateChangedSignal();
}
