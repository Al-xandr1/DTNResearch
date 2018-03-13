#include <GenerationRootsStrategy.h>


//----------------------------------- GenerationRootsByPersistenceStrategy ---------------------------------------------------


bool GenerationRootsByPersistenceStrategy::generateNewRoot(
            vector<HotSpotData*>* firstRoot, vector<unsigned int>* firstRootSnumber, vector<int>* firstRootCounter, vector<int>* firstRootWptsPerVisit,
            vector<HotSpotData*>*& currentRoot, vector<unsigned int>*& currentRootSnumber, vector<int>*& currentRootCounter, vector<int>*& currentRootWptsPerVisit) {

    // ������ ����� ������� ������� ���������
    currentRoot        = new vector<HotSpotData*>(*firstRoot);
    currentRootSnumber = new vector<unsigned int>(*firstRootSnumber);
    currentRootCounter = new vector<int>(*firstRootCounter);
    currentRootWptsPerVisit = new vector<int>(*firstRootWptsPerVisit);
    vector<int>* unusedWptsPerVisit = new vector<int>();


    unsigned int totalCount=0;
    for (unsigned int i=0; i<firstRoot->size(); i++) totalCount+=firstRootCounter->at(i);
    ASSERT(firstRootCounter->at(0) >= 1);   // ��� ������� ���� ��������� ������ ������� ������ ������������
    ASSERT(totalCount >= 1);                // ����� ���������� �������, ��� �������, �� ��������� �������� �������
    totalCount--;                           // ����������� ���� ��������� ��� �������� �������
    int replaceCount = totalCount - round( totalCount * rootPersistence);
    ASSERT(0 <= replaceCount && replaceCount <= totalCount);
    // ��������� �������� ��������� � ��������� ����� ��������� ������� �� ��������.
    // � ������ ������� (��������), ������� ����� �� ������.
    int rem, remCount=replaceCount;
    while( remCount > 0 ) {
        rem = rand() % currentRoot->size();
        if ( (rem == 0 && currentRootCounter->at(0) > 1) || (rem > 0 && currentRootCounter->at(rem) > 0) ) {
            currentRootCounter->at(rem)--;
            remCount--;
            // ����� ������� �����, ������� ������� �� ��������: ������ �������� ��������� �������
            // �������� �������� �� �������� �������� �������� ����� ����� �� ��� ���������,
            // - � ���������� ��� ����������� ������� � ����� �������
            unusedWptsPerVisit->push_back(currentRootWptsPerVisit->at(rem));
        }
    }
    // �������� ������� �����������
    ASSERT(currentRootCounter->at(0) >= 1);
    ASSERT(unusedWptsPerVisit->size() == replaceCount);


    // �������������� ����� ��������� ������� ����� ���������� �������� ������� � ������� �����������
    vector<int> possibleReplace;
    for(unsigned int i=0; i< hsc->getHSData()->size(); i++) possibleReplace.push_back(i);
    vector<unsigned int>* cur = new vector<unsigned int> (*currentRootSnumber);
    // ������� ������, �������������� � ��������, �� possibleReplace, ������� � ������ �������� � cur,
    // ����� ��������� ������� erase � possibleReplace - �.�. � ����� ������ �������� ����� ��������� � �����
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


    // ���������� ������ ����� ��������� ������� ����� �������
    // �� ���, ��� �������� � possibleReplace � ��������������� �� ����������� ����
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


    // ��������� ������ ����� ����� ������� � �������
    int remainingWPTS = 0;
    unsigned int wpts = 0;
    for(unsigned int i=0; i<sortReplace.size(); i++) {
       ASSERT(currentRootCounter->size() == currentRootWptsPerVisit->size());
       unsigned int hsNumber=sortReplace[i];
       HotSpotData* hs = &(hsc->getHSData()->at(hsNumber));
       // ��������� ���������� ��������� � ����� �������: ���� ������� "������" �� ����� 1, ����� ���������� �����
       if (hs->isHotSpotEmpty()) {
           wpts = 1;
           // ����������������� ������� ���������
           remainingWPTS += unusedWptsPerVisit->front() - 1;
           ASSERT(remainingWPTS >= 0);
       } else {
           // ����������������� ������� ��������� ��� ������ �����������
           wpts = unusedWptsPerVisit->front() + remainingWPTS;
           remainingWPTS = 0;
       }
       // ������ �������������� ����� �� "������"
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

    //���������� ��������� ������ ����������� � ����� ��������
    ASSERT(getSum(*currentRootCounter) == getSum(*firstRootCounter));

    return true;
}



//----------------------------------- GenerationRootsByStatistiscStrategy ---------------------------------------------------


bool GenerationRootsByStatisticsStrategy::generateNewRoot(
        vector<HotSpotData*>* firstRoot, vector<unsigned int>* firstRootSnumber, vector<int>* firstRootCounter, vector<int>* firstRootWptsPerVisit,
        vector<HotSpotData*>*& currentRoot, vector<unsigned int>*& currentRootSnumber, vector<int>*& currentRootCounter, vector<int>*& currentRootWptsPerVisit) {

    ASSERT(hsc->getHSData()->size() == rootStatistics->getSummarizedIndicatorVectorPDF()->size());

    currentRoot        = new vector<HotSpotData*>();
    currentRootSnumber = new vector<unsigned int>();
    currentRootCounter = new vector<int>();
    currentRootWptsPerVisit = new vector<int>();

    int dimension;
    while ((dimension = generateRootDimension()) <= 0);
    ASSERT(dimension > 0);

    int currentDimension = 0;

    // �������� �������� ������� � ����������� �� �������� ������ useFixedHomeLocation
    HotSpotData* homeHotSpot = NULL;
    int homeHotSpotIndexPST = -1;
    int homeHotSpotIndexHSC = -1;
    if (useFixedHomeLocation) {
        homeHotSpot = firstRoot->at(0);
        homeHotSpotIndexHSC = firstRootSnumber->at(0);
        homeHotSpotIndexPST = rootStatistics->findHotSpotIndexByName(firstRoot->at(0)->hotSpotName);
    } else {
        homeHotSpotIndexPST = generateHomeHotSpotIndexPST();
        const char* homeHotSpotName = rootStatistics->getHotSpots()->at(homeHotSpotIndexPST);
        // �� ����� ������� ������� ������ ������ � hsc->getHSData()
        homeHotSpot = hsc->findHotSpotbyName(homeHotSpotName, homeHotSpotIndexHSC);
    }
    ASSERT(homeHotSpot && homeHotSpotIndexHSC != -1 && homeHotSpotIndexPST != -1);

    // ��������� �������� ������� � ����� �������
    currentRoot->push_back(homeHotSpot);
    // ... ��������� � ������������ ������
    currentRootSnumber->push_back(homeHotSpotIndexHSC);
    // ... � ���������� ��������� �� ������ ������ � ������� ���������� �� ����� *.pst (�� ������ �� ����� *.pst)
    currentRootCounter->push_back(getHotSpotCount(homeHotSpotIndexPST));
    currentDimension++;

    while (currentDimension < dimension) {
        // �������� ��������� ������ �� ����� *.pst
        const unsigned int currentIndexPST = generateHotSpotIndexPST();
        // �� currentIndexPST �������� ��� ������� �� *.pst,
        const char* hotSpotName = rootStatistics->getHotSpots()->at(currentIndexPST);

        int signedCurrentIndexHSC = -1;
        // �� ����� ������� ������� ������ ������ � hsc->getHSData()
        hsc->findHotSpotbyName(hotSpotName, signedCurrentIndexHSC);
        // ������� �� ������� ����� ����������� ������ �������!
        ASSERT(signedCurrentIndexHSC >= 0 && ((unsigned int)signedCurrentIndexHSC < hsc->getHSData()->size()));
        unsigned int currentIndexHSC = signedCurrentIndexHSC;

        if (contains(*(currentRootSnumber), currentIndexHSC)) {
            //���� ��� ������ ������� ��������, �� �������� ��� ���
            continue;
        }

        // ������������ ������ �������
        currentRootSnumber->push_back(currentIndexHSC);                     // ������ ������ �� HotSpotsCollection
        currentRoot->push_back(&(hsc->getHSData()->at(currentIndexHSC)));
        currentRootCounter->push_back(getHotSpotCount(currentIndexPST));    // ������ ������ �� ����� *.pst!
        currentDimension++;
    }

    for (unsigned int i = 0; i < currentRoot->size(); i++) {
        //todo ������� ���������� � ������������ ����� ����� �� ���� ��������� ������� currentRootWptsPerVisit
        currentRootWptsPerVisit->push_back(10);
    }

    return true;
}


unsigned int GenerationRootsByStatisticsStrategy::generateRootDimension() {
    int dimension = generate(rootStatistics->getRootsDimensionHistogramPDF());
    ASSERT(dimension >= 0 && ((unsigned int)dimension) < rootStatistics->getHotSpots()->size());
    return dimension;
}

unsigned int GenerationRootsByStatisticsStrategy::generateHotSpotIndexPST() {
    int hotSpotIndex = generate(rootStatistics->getSummarizedIndicatorVectorPDF());
    ASSERT(hotSpotIndex >= 0 && ((unsigned int)hotSpotIndex) < rootStatistics->getHotSpots()->size());
    return hotSpotIndex;
}

unsigned int GenerationRootsByStatisticsStrategy::getHotSpotCount(unsigned int hotSpotIndexPST) {
    ASSERT(hotSpotIndexPST >= 0 && hotSpotIndexPST < rootStatistics->getAverageCounterVector()->size());
    double averageCount = rootStatistics->getAverageCounterVector()->at(hotSpotIndexPST);
    //� rootStatistics->getAverageCounterVector() �������� ������������, � ��� �����. ���������:
    int averageCountInt = round(averageCount);
    ASSERT(averageCountInt >= 0);
    return averageCountInt;
}

unsigned int GenerationRootsByStatisticsStrategy::generateHomeHotSpotIndexPST() {
    int homeHotSpotIndex = generate(rootStatistics->getHomeHotspotHistogramPDF());
    ASSERT(homeHotSpotIndex >= 0 && ((unsigned int)homeHotSpotIndex) < rootStatistics->getHotSpots()->size());
    return homeHotSpotIndex;
}

int GenerationRootsByStatisticsStrategy::generate(vector<double>* pdf)
{
    int random = -1;
    double rn, probability = 0;
    rn = ((double) rand()) / RAND_MAX;
    if (rn <= 0 || rn >=1) {cout << "1 WARNING: rn=" << rn << endl; exit(-441);};

    for(unsigned int i = 0; i < pdf->size(); i++) {
        probability += pdf->at(i);
        //todo rn ����� ���������� 0 ��� 1: ����� ����� �������� ������ 0 � -1 �������������� (�.�. ��-�� ����������� ����� PDF ����� ���� ������ 1)
        if (rn <= probability) {
            random = i;
            if (random <= 0) {
                cout << "2 WARNING: rn=" << rn << endl;
                cout << "2 WARNING: probability=" << probability << endl;
                cout << "2 WARNING: random=" << random << endl;
                cout << "2 WARNING: i=" << i << endl;
            };
            break;
        }
    }
    if (random <= 0) {
        cout << "3 WARNING: rn=" << rn << endl;
        cout << "3 WARNING: probability=" << probability << endl;
        cout << "3 WARNING: random=" << random << endl;
    };

    return random;
}
