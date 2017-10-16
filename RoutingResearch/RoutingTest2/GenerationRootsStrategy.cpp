#include <GenerationRootsStrategy.h>


//----------------------------------- GenerationRootsByPersistenceStrategy ---------------------------------------------------


bool GenerationRootsByPersistenceStrategy::generateNewRoot(
            vector<HotSpotData*>* firstRoot, vector<unsigned int>* firstRootSnumber, vector<int>* firstRootCounter, vector<int>* firstRootWptsPerVisit,
            vector<HotSpotData*>*& currentRoot, vector<unsigned int>*& currentRootSnumber, vector<int>*& currentRootCounter, vector<int>*& currentRootWptsPerVisit) {

    // создаём новый маршрут копируя эталонный
    currentRoot        = new vector<HotSpotData*>(*firstRoot);
    currentRootSnumber = new vector<unsigned int>(*firstRootSnumber);
    currentRootCounter = new vector<int>(*firstRootCounter);
    currentRootWptsPerVisit = new vector<int>(*firstRootWptsPerVisit);
    vector<int>* unusedWptsPerVisit = new vector<int>();


    unsigned int totalCount=0;
    for (unsigned int i=0; i<firstRoot->size(); i++) totalCount+=firstRootCounter->at(i);
    ASSERT(firstRootCounter->at(0) >= 1);   // как минимум одно посещение данной локации должно существовать
    ASSERT(totalCount >= 1);                // общее количество состоит, как минимум, из посещения домашней локации
    totalCount--;                           // резервируем одно посещение для домашней локации
    int replaceCount = totalCount - round( totalCount * rootPersistence);
    ASSERT(0 <= replaceCount && replaceCount <= totalCount);
    // уменьшаем счётчики посещений у заданного числа случайных локаций на маршруте.
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
    ASSERT(currentRootCounter->at(0) >= 1);
    ASSERT(unusedWptsPerVisit->size() == replaceCount);


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
    ASSERT(possibleReplace.size() == (hsc->getHSData()->size() - currentRootSnumber->size()));
    ASSERT(possibleReplace.size() != 0);


    // генерируем нужное число случайных номеров новых локаций
    // из тех, что остались в possibleReplace в отсортированном по возрастанию виде
    vector<int> sortReplace;
    unsigned int ri = rand() % possibleReplace.size();
    const unsigned int replaceCountForCheck = replaceCount;
    if (replaceCount > 0) {
        sortReplace.push_back(possibleReplace[ri]);
        replaceCount--;
    }
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
    ASSERT(sortReplace.size() == replaceCountForCheck);


    // добавляем нужное число новых локаций в маршрут
    int remainingWPTS = 0;
    unsigned int wpts = 0;
    for(unsigned int i=0; i<sortReplace.size(); i++) {
       ASSERT(currentRootCounter->size() == currentRootWptsPerVisit->size());
       unsigned int hsNumber=sortReplace[i];
       HotSpotData* hs = &(hsc->getHSData()->at(hsNumber));
       // Вставляем сохраненые посещения в новый маршрут: если локация "пустая" то пишем 1, иначе сохранённое число
       if (hs->isHotSpotEmpty()) {
           wpts = 1;
           // неизрасходованные локации сохраняем
           remainingWPTS += unusedWptsPerVisit->front() - 1;
           ASSERT(remainingWPTS >= 0);
       } else {
           // неизрасходованные локации расходуем при первой возможности
           wpts = unusedWptsPerVisit->front() + remainingWPTS;
           remainingWPTS = 0;
       }
       // удалёям использованный точки из "памяти"
       unusedWptsPerVisit->erase(unusedWptsPerVisit->begin());
       if( hsNumber != currentRootSnumber->back() ) {
           currentRoot->push_back(hs);
           currentRootSnumber->push_back(hsNumber);
           currentRootCounter->push_back(1);
           currentRootWptsPerVisit->push_back(wpts);
       } else {
           currentRootCounter->back()++;
           currentRootWptsPerVisit->back() += wpts;
       }
       wpts = 0;
    }
    delete unusedWptsPerVisit;
    if (remainingWPTS > 0) {
        for (unsigned int i=currentRoot->size()-1; i>=0; i--)
            if (!currentRoot->at(i)->isHotSpotEmpty()) {
                currentRootWptsPerVisit->at(i) += remainingWPTS;
                remainingWPTS = 0;
                break;
            }
    }

    return true;
}



//----------------------------------- GenerationRootsByStatistiscStrategy ---------------------------------------------------


bool GenerationRootsByStatisticsStrategy::generateNewRoot(
        vector<HotSpotData*>* firstRoot, vector<unsigned int>* firstRootSnumber, vector<int>* firstRootCounter, vector<int>* firstRootWptsPerVisit,
        vector<HotSpotData*>*& currentRoot, vector<unsigned int>*& currentRootSnumber, vector<int>*& currentRootCounter, vector<int>*& currentRootWptsPerVisit) {
    exit(-202);
    return true;
}


int GenerationRootsByStatisticsStrategy::generateRootDimension() {
    int dimension = generate(this->rootStatistics->getRootsDimensionHistogramPDF());
    ASSERT(dimension >= 0);
    return dimension;
}

int GenerationRootsByStatisticsStrategy::generateHotSpotIndex() {
    int hotSpotIndex = generate(this->rootStatistics->getSummarizedIndicatorVectorPDF());
    ASSERT(hotSpotIndex >= 0);
    return hotSpotIndex;
}

int GenerationRootsByStatisticsStrategy::generateHotSpotCount() {
    int hotSpotCount = generate(this->rootStatistics->getSummarizedRootPDF());
    ASSERT(hotSpotCount >= 0);
    return hotSpotCount;
}

int GenerationRootsByStatisticsStrategy::generate(vector<double>* pdf)
{
    int random = -1;
    double rn, probability = 0;
    rn = (double) rand() / RAND_MAX;

    for(unsigned int i = 0; i < pdf->size(); i++) {
        probability += pdf->at(i);
        if (rn <= probability) {
            random = i; // todo или -1 делать???
            break;
        }
    }
    return random;
}
