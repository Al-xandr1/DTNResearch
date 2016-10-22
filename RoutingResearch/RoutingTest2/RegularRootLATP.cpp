#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <windows.h>
#include <vector>

#include "RegularRootLATP.h"
#include "Messages.h"

#define TRACE_TYPE ".txt"
#define WAYPOINTS_TYPE ".wpt"


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
    if (h!=NULL) {
        firstRoot->push_back(h);
        firstRootSnumber->push_back(Snum);
        firstRootCounter->push_back(1);
    } else exit(-235);

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
                if(  (i!=j)  && ((currentRoot->at(j))->counter > 0)  )
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
        char* TracesDir = DEF_TR_DIR;
        rc->readRootsInfo(TracesDir, "allroots.roo");
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

        // начальная локация - это первая локация текущего маршрута
        curRootIndex=0;
        setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
        hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex);

        // printCurrentRoot();
    }

    if (LocalProbMatrix == NULL) makeLocalProbMatrix(powA);
}


bool RegularRootLATP::findNextHotSpot()
{
    // ASSERT((currentRoot->at(curRootIndex))->counter>0);
    if ( (currentRoot->at(curRootIndex))->counter>0 ) (currentRoot->at(curRootIndex))->counter-=1;  // покидая локацию, уменьшаем её счётчик посещений

    unsigned int hh=0, ii;                               // находим сумму всех счётчиков посещений на маршруте,
    for(unsigned int i=0; i<currentRoot->size(); i++)    // чтобы определить, когда конец маршрута
        if ( (currentRoot->at(i))->counter > 0) { ii=i; hh+=(currentRoot->at(i))->counter; }

    if( hh == 0 ) return false;                          // маршрут кончился
    if( hh == (currentRoot->at(ii))->counter ) {         // осталась одна локация (может быть, с несколькими посещениями)
        curRootIndex = ii;
        (currentRoot->at(curRootIndex))->counter = 1;    // если посещений несколько, заменяем одним
        setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
        hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex);
        return true;
    }

    if((currentRoot->at(curRootIndex))->counter == 0) {  // пересчитываем матрицу вероятностей переходов, если счётчик посещений обнулился
        for(unsigned int i=0; i<currentRoot->size(); i++) {
            LocalProbMatrix[i][curRootIndex]=0;
            double h=0;
            for(unsigned int j=0; j<currentRoot->size(); j++) h+=LocalProbMatrix[i][j];
            if ( h!=0 ) for(unsigned int j=0; j<currentRoot->size(); j++) LocalProbMatrix[i][j]/=h;
            else return false; // все вероятности перехода уже нули, мы в конце маршрута (на всякий случай)
            }
    }
    // случайным образом выбираем новую локацию
    double rn, pr=0;
    rn=(double)rand()/RAND_MAX;
    for(unsigned int i=0; i<currentRoot->size(); i++) {
        if(curRootIndex != i) pr+=LocalProbMatrix[curRootIndex][i];
        if(rn <= pr) { curRootIndex=i; break; }
    }
    if (rn > pr) {cout<<"rn="<<rn<<", pr="<<pr<<", currentRoot->size()="<<currentRoot->size()<<endl; exit(-987);}
    setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
    hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex);
    //    cout << "findNextHotSpot: changing location to" << currentHSindex << endl;
    return true;
}


bool RegularRootLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange)
{
    bool flag=LevyHotSpotsLATP::generateNextPosition(targetPosition, nextChange);
    if (flag) return true;   // идём по маршруту
    else {                   // маршрут кончился, идём домой
        currentHSindex=0;
        setCurrentHSbordersWith( homeHS );

        // проверяем, не дома ли мы уже
        if( currentHSMin.x <= lastPosition.x &&  lastPosition.x <= currentHSMax.x &&
            currentHSMin.y <= lastPosition.y &&  lastPosition.y <= currentHSMax.y ) {
            // TO DO послать сообщение, что день окончен!
            return false;
        }

        // если нет - идём домой
        targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
        targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);

        distance = sqrt( (targetPosition.x-lastPosition.x)*(targetPosition.x-lastPosition.x)+(targetPosition.y-lastPosition.y)*(targetPosition.y-lastPosition.y) );
        ASSERT(distance > 0);
        speed = kForSpeed * pow(distance, 1 - roForSpeed);
        travelTime = distance / speed;
        nextChange = simTime() + travelTime;
        return true;
    }
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
    // уменьшаем счётчики посещений у заданного числа случайных локаций на маршруте.
    // у первой локации (домашней), счётчик нулём не делаем.
    int rem, remCount=replaceCount;
    while( remCount > 0 ) {
        rem = rand() % currentRoot->size();
        if ( (rem == 0 && currentRootCounter->at(0) > 1) || (rem > 0 && currentRootCounter->at(rem) > 0) ) {
        currentRootCounter->at(rem)--;
        remCount--;
        }
    }

    // инициализируем набор возможных локаций всеми возможными номерами локаций в порядке возрастания
    vector<int> possibleReplace;
    for(unsigned int i=0; i< HotSpotsCollection::HSData.size(); i++) possibleReplace.push_back(i);
    vector<unsigned int>* cur = new vector<unsigned int> (*currentRootSnumber);
    // удаляем номера, присутствующие в маршруте, из possibleReplace, начиная с самого большого,
    // чтобы корректно работал erase в possibleReplace
    while( cur->size() > 0 ) {
        for(unsigned i=1; i<cur->size(); i++) if( cur->at(0) < cur->at(i) ) {
                unsigned int tmp=cur->at(0); cur->at(0)=cur->at(i); cur->at(i)=tmp;
            }
        possibleReplace.erase( possibleReplace.begin() + cur->at(0) );
        cur->erase(cur->begin());
    }
    delete cur;

    // генерируем нужное число случайных номеров новых локаций в отсортированном виде
    vector<int> sortReplace;
    unsigned ri = rand() % possibleReplace.size();
    sortReplace.push_back(possibleReplace[ri]);
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

    // добавляем нужное число новых локаций в маршрут
    for(unsigned int i=0; i<sortReplace.size(); i++) {
       unsigned int hsNumber=sortReplace[i];
       if( hsNumber != currentRootSnumber->back() ) {
           currentRoot->push_back(&(HotSpotsCollection::HSData[hsNumber]));
           currentRootSnumber->push_back(hsNumber);
           currentRootCounter->push_back(1);
       } else currentRootCounter->back()++;

    }

    // printFirstRoot();
    // printCurrentRoot();

    makeLocalProbMatrix(powA);

    // начальная локация - это первая локация текущего маршрута
    curRootIndex=0;
    setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
    hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex);

    targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
    targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);
    //todo стоит ли менять время прибытия?

}
