#include "RegularRootLATP.h"

Define_Module(RegularRootLATP);

RegularRootLATP::RegularRootLATP()
{
    rc = NULL;
    rootStatistics = NULL;
    rootPersistence = -1;
    rootGenerator = NULL;

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

    // �������� ������� �������� (�������)
    vector<HotSpotDataRoot>* root = rc->getRootDataByNodeId(NodeID);
    // ��������� ������� � ������ ��������� �������
    h = hsc->findHotSpotbyName(root->at(0).hotSpotName, Snum);
    ASSERT(h != NULL && Snum != -1); // ���� ������ - �������� ������ � hotspotfiles/ & rootfiles/ �� �����������
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

    // ����������� ������� ����� ��������� ��������� ��� ������� ���������� ��������
    for(unsigned int i=0; i<firstRoot->size(); i++)
        firstRootWptsPerVisit->at(i) /= firstRootCounter->at(i); // todo ������� ���������� � ������� �������

    // �������� �������� �������
    homeHS = firstRoot->at(0);

    RootDataShort *rootDataShort = rc->getRootDataShortByNodeId(NodeID);
    ASSERT(rootDataShort);
    // ����������� �������������� �������� ������������ ���������������, ���� ��� ����
    if (rootDataShort->persistence) rootPersistence = *(rootDataShort->persistence);

    printFirstRoot();
}


void RegularRootLATP::printFirstRoot()
{
    ASSERT(firstRoot != NULL && hsc != NULL);
    int totalRepeats = 0,
        totalWPTS    = 0;
    for(unsigned int i=0; i<firstRoot->size(); i++) {
        cout << NodeID
                  << " First Root: " << (firstRoot->at(i))->hotSpotName
                  << " Snum=" << firstRootSnumber->at(i)
                  << " repeat=" << firstRootCounter->at(i)
                  << " wptsPerRepeat=" << firstRootWptsPerVisit->at(i) <<  endl;
        totalRepeats += firstRootCounter->at(i);
        totalWPTS += firstRootCounter->at(i) * firstRootWptsPerVisit->at(i);
    }
    int originTotalWPTS = 0;
    vector<HotSpotDataRoot>* root = rc->getRootDataByNodeId(NodeID);
    for (unsigned int i=0; i<root->size(); i++) originTotalWPTS += root->at(i).waypointNum;
    cout << NodeID << "\t\t\t\t totalRepeats=" << totalRepeats
         << " totalWPTS=" << totalWPTS << " originTotalWPTS=" << originTotalWPTS << endl;
    cout << "\t\t\t\t rootPersistence = " << rootPersistence << endl;
}


void RegularRootLATP::printCurrentRoot()
{
    ASSERT(currentRoot != NULL && hsc != NULL);
    int totalRepeats = 0,
        totalWPTS    = 0;
    for(unsigned int i=0; i<currentRoot->size(); i++) {
        cout << NodeID
                  << " Current Root: " << (currentRoot->at(i))->hotSpotName
                  << " Snum=" << currentRootSnumber->at(i)
                  << " repeat=" << currentRootCounter->at(i)
                  << " wptsPerRepeat=" << currentRootWptsPerVisit->at(i) << endl;
        totalRepeats += currentRootCounter->at(i);
        totalWPTS += currentRootCounter->at(i) * currentRootWptsPerVisit->at(i);
    }
    int originTotalWPTS = 0;
    vector<HotSpotDataRoot>* root = rc->getRootDataByNodeId(NodeID);
    for (unsigned int i=0; i<root->size(); i++) originTotalWPTS += root->at(i).waypointNum;
    cout << NodeID << "\t\t\t\t totalRepeats=" << totalRepeats
         << " totalWPTS=" << totalWPTS << " originTotalWPTS=" << originTotalWPTS << endl;
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

    // �������� ������ �� ��������� ���������
    if (!rc) rc = RootsCollection::getInstance();

    if (!rootStatistics) rootStatistics =
            check_and_cast<RootsPersistenceAndStatistics*>(getParentModule()->getParentModule()->getSubmodule("rootsStatistics"));
    ASSERT(rootStatistics);

    if (rootPersistence == -1)
        // ��� ������������� ���������� �������� �� ��������� ��� ����.
        // ����� ��������� ��������� �������������� �������� �� �������� ����� �� ������� ���������
        rootPersistence = rootStatistics->getPersistenceFromMassCenter();
    ASSERT(rootPersistence);
    ASSERT(hsc);

    if (!rootGenerator)
        if (getParentModule()->getParentModule()->par("useRootStatistics").boolValue())
            rootGenerator = new GenerationRootsByStatisticsStrategy(rootStatistics);
        else
            rootGenerator = new GenerationRootsByPersistenceStrategy(rootPersistence, hsc);


    if (!firstRoot) loadFirstRoot();

    if (!currentRoot) {
        // ������ ��� ����� �� ���������� ��������
        currentRoot = new vector<HotSpotData*>(*firstRoot);
        currentRootSnumber = new vector<unsigned int>(*firstRootSnumber);
        currentRootCounter = new vector<int>(*firstRootCounter);
        currentRootWptsPerVisit = new vector<int>(*firstRootWptsPerVisit);
        currentRootCounterSAVED = new vector<int>(*currentRootCounter);

        // ��������� ������� - ��� ������ ������� �������� ��������
        curRootIndex=0;
        currentHSindex = currentRootSnumber->at(curRootIndex);
        ASSERT(homeHS == currentRoot->at(curRootIndex));
        RegularRootLATP::setCurrentHSbordersWith( currentRoot->at(curRootIndex) );

        //printCurrentRoot();

        // ��������� ��� ���������� ��������������� ������� ��� ������� ��� � RootsCollection.
        const int currentDay = RoutingDaemon::instance->getCurrentDay();
        ASSERT(currentDay == 0);
        RootsCollection::getInstance()->collectTheoryRoot(currentRoot, currentRootSnumber, currentRootCounter, NodeID, currentDay+1);
    }

    if (!LocalProbMatrix) makeLocalProbMatrix(powA);

    useWaypointCounter = getParentModule()->par("useWaypointCounter").boolValue();
}


void RegularRootLATP::setCurrentHSbordersWith(HotSpotData* hsi)
{
    LevyHotSpotsLATP::setCurrentHSbordersWith(hsi);
    //����� ������ ��������� ������� ������� �� ������� ��������,
    //�.�. ��������� ���� ������� ����� ���� ������ ���
    currentHSWaypointNum = currentRootWptsPerVisit->at(curRootIndex);
}


void RegularRootLATP::handleMessage(cMessage * message)
{
    if (message->isSelfMessage())
        MobilityBase::handleMessage(message);
    else
        switch (message->getKind()) {
            // ������������ ��� "�����" ��� �����������, ����� ����� ������ ������
            case MOBILITY_START:{
                nextChange = simTime();
                MovingMobilityBase::scheduleUpdate();
                emitMobilityStateChangedSignal();
                ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));
                break;
            }
            default:
                ASSERT(false); //unreacheble statement
        }
}


void RegularRootLATP::setTargetPosition() {
    LevyHotSpotsLATP::setTargetPosition();

    if (movementsFinished) {
        // ������� ������ � ��������� � ������������� - ����� �������������, �� �� ���������
        movementsFinished = false;
        nextChange = MAXTIME;
    }
}


bool RegularRootLATP::findNextHotSpot()
{
    if (currentRootCounter->at(curRootIndex) > 0)
        (*currentRootCounter)[curRootIndex]-=1;  // ������� �������, ��������� �� ������� ���������

    unsigned int hh=0, ii;                               // ������� ����� ���� ��������� ��������� �� ��������,
    for(unsigned int i=0; i<currentRoot->size(); i++)    // ����� ����������, ����� ����� ��������
        if (currentRootCounter->at(i) > 0) {
            ii=i; hh+=currentRootCounter->at(i);
        }

    if( hh == 0 ) return false;                          // ������� ��������
    if( hh == currentRootCounter->at(ii) ) {             // �������� ���� ������� (����� ����, � ����������� �����������)
        curRootIndex = ii;
        (*currentRootCounter)[curRootIndex]=1;           // ���� ��������� ���������, �������� �����
        currentHSindex = currentRootSnumber->at(curRootIndex);
        RegularRootLATP::setCurrentHSbordersWith( currentRoot->at(curRootIndex) );

        return true;
    }

    if(currentRootCounter->at(curRootIndex) == 0) {       // ������������� ������� ������������ ���������, ���� ������� ��������� ���������
        for(unsigned int i=0; i<currentRoot->size(); i++) {
            LocalProbMatrix[i][curRootIndex]=0;
            double h=0;
            for(unsigned int j=0; j<currentRoot->size(); j++) h+=LocalProbMatrix[i][j];
            if ( h!=0 ) for(unsigned int j=0; j<currentRoot->size(); j++) LocalProbMatrix[i][j]/=h;
            else return false; // ��� ����������� �������� ��� ����, �� � ����� �������� (�� ������ ������)
        }
    }
    // ��������� ������� �������� ����� �������
    double rn, pr=0;
    rn=(double)rand()/RAND_MAX;
    for(unsigned int i=0; i<currentRoot->size(); i++) {
        if(curRootIndex != i) pr+=LocalProbMatrix[curRootIndex][i];
        if(rn <= pr) { curRootIndex=i; break; }
    }
    ASSERT(rn <= pr);
    currentHSindex = currentRootSnumber->at(curRootIndex);
    RegularRootLATP::setCurrentHSbordersWith( currentRoot->at(curRootIndex) );
    //    cout << "findNextHotSpot: changing location to" << currentHSindex << endl;
    return true;
}


bool RegularRootLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange, bool regenerateIfOutOfBound)
{
    if (useWaypointCounter) {
        // ���������� �������
        ASSERT(currentHSWaypointNum >= 0);
        if (currentHSWaypointNum != 0) {
            // ���� ������� ��� �� ����� 0, �� ��� �� ��������
            bool nextPosFound=LevyHotSpotsLATP::generateNextPosition(targetPosition, nextChange, true);
            // ���� ������� ��� ��������� ������, �� �� ������ ������ ����� ������� �����
            ASSERT(nextPosFound);
            // ��������� ������� ���������� ������� �����
            currentHSWaypointNum -= 1;
            return true;
        }

        ASSERT(currentHSWaypointNum == 0);
        //���� ������� ����� 0, �� ���� ������ �������
        if (LevyHotSpotsLATP::findNextHotSpotAndTargetPosition()) {
            // ...������ �������� �������
            ASSERT(currentHSWaypointNum > 0);
            // ��������� ������� ���������� ������� �����, �.�. ������ ��� ������� ������� �����
            currentHSWaypointNum -= 1;
            return true;
        }

    } else {
        // ���� �� ���������� �������, �� ���������� ����� ������� ����������
        if (LevyHotSpotsLATP::generateNextPosition(targetPosition, nextChange)) {
            // ... � ���� ������� �����, ����������� �����.
            return true;
        }
        // ���� �� ������� ����� (� �������, ����� ������� ���������� ������ ������), �� ��������� �������
    }

    // ������� ��������, ��� �����
    if (useWaypointCounter) ASSERT(currentHSWaypointNum == 0);
    currentHSindex=0;
    RegularRootLATP::setCurrentHSbordersWith( homeHS );

    // ���������, �� ���� �� �� ���
    if( currentHSMin.x <= lastPosition.x &&  lastPosition.x <= currentHSMax.x &&
        currentHSMin.y <= lastPosition.y &&  lastPosition.y <= currentHSMax.y ) {

        ASSERT(isRootFinished());
        (check_and_cast<MobileHost*>(getParentModule()))->ensureEndRoute();
        return false;
    }

    // ���� ��� - ���� �����
    targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
    targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);

    distance = sqrt( (targetPosition.x-lastPosition.x)*(targetPosition.x-lastPosition.x)+(targetPosition.y-lastPosition.y)*(targetPosition.y-lastPosition.y) );
    ASSERT(distance > 0);
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
    ASSERT(currentDay >= 2); // ���������� �������� ����� ��������� �� ������� ���
    cout << endl << "Making new root for NodeID: " << NodeID << " at day: " << currentDay << endl;


    if(currentRoot != NULL) {
        /* ��������� ��� ���������� ��������������� �������, ������� ������� ��� ������� (������� �� ����������� ���).
           ���������� ��� ���������� �� ������� ���. ������ ���� ����������� ����� ����� �������� � initialize.
         */
        for(unsigned int i = 0; i < currentRootCounterSAVED->size(); i++) {
            // ��������� ���������� ���������� ������� ��� actualCurrentRootCounter = currentRootCounterSAVED - currentRootCounter
            (*currentRootCounterSAVED)[i] -= (*currentRootCounter)[i];
            ASSERT((*currentRootCounterSAVED)[i] >= 0);
        }
        RootsCollection::getInstance()->collectActualRoot(currentRoot, currentRootSnumber, currentRootCounterSAVED, NodeID, currentDay - 1);
        cout << endl << "Saved old root for NodeID: " << NodeID << " at previous day: " << (currentDay - 1) << endl;

        // ������� ������ �������
        deleteLocalProbMatrix();
        delete currentRoot;
        delete currentRootSnumber;
        delete currentRootCounter;
        delete currentRootWptsPerVisit;
        delete currentRootCounterSAVED;
    }


    rootGenerator->generateNewRoot(
            firstRoot, firstRootSnumber, firstRootCounter, firstRootWptsPerVisit,
            currentRoot, currentRootSnumber, currentRootCounter, currentRootWptsPerVisit);


    // for debug
    printFirstRoot();
    printCurrentRoot();
    unsigned int sumCurrent=0, sumFirst=0;
    for(unsigned int i=0; i<currentRootCounter->size(); i++) sumCurrent += currentRootCounter->at(i);
    for(unsigned int i=0; i<firstRootCounter->size();   i++) sumFirst +=   firstRootCounter->at(i);
    ASSERT(sumCurrent == sumFirst);
    // ��� ��������� ��������� �� �������
    ASSERT((currentRoot->size() == currentRootSnumber->size()) && (currentRoot->size() == currentRootCounter->size()));


    makeLocalProbMatrix(powA);
    // ��������� ������� - ��� ������ ������� �������� �������� - ��� �� ��������
    curRootIndex=0;
    currentHSindex = currentRootSnumber->at(curRootIndex);
    ASSERT(homeHS == currentRoot->at(curRootIndex));
    RegularRootLATP::setCurrentHSbordersWith(homeHS);
    targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
    targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);


    /* ��������� ��� ���������� ��������������� ������� ��� �������� ��� � RootsCollection.
       ���������� ��� ���������� �� ������� ���. ������ ���� ����������� ����� ����� �������� � initialize.
       ��� ����������� ������ ��� ��������� ������, �� �������� ��� �� ������.
     */
    RootsCollection::getInstance()->collectTheoryRoot(currentRoot, currentRootSnumber, currentRootCounter, NodeID, currentDay);
    currentRootCounterSAVED = new vector<int>(*currentRootCounter);
}
