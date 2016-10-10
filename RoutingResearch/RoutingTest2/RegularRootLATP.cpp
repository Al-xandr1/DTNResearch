#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <windows.h>

#include "RegularRootLATP.h"
#include "Messages.h"

#define TRACE_TYPE ".txt"
#define WAYPOINTS_TYPE ".wpt"


Define_Module(RegularRootLATP);

RegularRootLATP::RegularRootLATP()
{
    rc = NULL;

    rootPersistence = -1;

    firstRoot   = NULL;
    firstRootSnumber = NULL;
    firstRootCounter = NULL;

    currentRoot = NULL;
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

    // �������� ������� �������� (�������)
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
                if(i!=j) h += LocalProbMatrix[i][j] = pow(1/hsd->getDistance(ii,jj), powA);
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

    if (rc==NULL) {
        rc = new RootsCollection();
        char* TracesDir = DEF_TR_DIR;
        // �������� ������ � ���������
        rc->readRootsInfo(TracesDir, "allroots.roo");
        // rc->print();
    }

    if (rootPersistence == -1) {
        rootPersistence = getParentModule()->par("rootPersistence").doubleValue();
    }

    if (firstRoot == NULL) {
        loadFirstRoot();
        // printFirstRoot();
    }

    if (currentRoot == NULL) {
        // ������ ��� ����� �� ���������� ��������
        currentRoot = new vector<HotSpotShortInfo*>(*firstRoot);
        currentRootSnumber = new vector<unsigned int>(*firstRootSnumber);
        currentRootCounter = new vector<int>(*firstRootCounter);

        // ��������� ������� - ��� ������ ������� �������� ��������
        curRootIndex=0;
        currentHSMin.x=(currentRoot->at(curRootIndex))->Xmin;    //std::cout<<currentHSMin.x<<"\t";
        currentHSMin.y=(currentRoot->at(curRootIndex))->Ymin;    //std::cout<<currentHSMin.y<<"\t";
        currentHSMax.x=(currentRoot->at(curRootIndex))->Xmax;    //std::cout<<currentHSMax.x<<"\t";
        currentHSMax.y=(currentRoot->at(curRootIndex))->Ymax;    //std::cout<<currentHSMax.y<<"\n";
        currentHSCenter=(currentHSMin+currentHSMax)*0.5;
        hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex);

        // printCurrentRoot();
    }

    if (LocalProbMatrix == NULL) makeLocalProbMatrix(powA);
}


bool RegularRootLATP::findNextHotSpot()
{
    // ������� �������, ��������� � ������� ��������� � ������������� ������� ������������
    // ���������, ���� ������� ��������� ���������
    (currentRoot->at(curRootIndex))->counter-=1;
    if((currentRoot->at(curRootIndex))->counter == 0)
        for(int i=0; i<(int)currentRoot->size(); i++) {
            LocalProbMatrix[i][curRootIndex]=0;
            double h=0;
            for(int j=0; j<(int)currentRoot->size(); j++) h+=LocalProbMatrix[i][j];
            if (h==0) return false;   // ��� �������� ��������� ��� ����
            else for(int j=0; j<(int)currentRoot->size(); j++) LocalProbMatrix[i][j]/=h;
        }

    // �������� ����� �������
    double rn, pr=0;
    rn=(double)rand()/RAND_MAX;
    for(int i=0; i<(int)currentRoot->size(); i++) {
        if(curRootIndex != i) pr+=LocalProbMatrix[curRootIndex][i];
        if(rn <= pr) { curRootIndex=i; break; }
    }
    if (currentRoot->size() == 1 && curRootIndex == 0) {pr=LocalProbMatrix[curRootIndex][curRootIndex];} // if one hs in root
    if (rn > pr) {cout<<"rn="<<rn<<", pr="<<pr<<", currentRoot->size()="<<currentRoot->size()<<endl; exit(-987);}
    ASSERT(currentHSMin.x != (currentRoot->at(curRootIndex))->Xmin);
    currentHSMin.x=(currentRoot->at(curRootIndex))->Xmin;    //std::cout<<currentHSMin.x<<"\t";
    currentHSMin.y=(currentRoot->at(curRootIndex))->Ymin;    //std::cout<<currentHSMin.y<<"\t";
    currentHSMax.x=(currentRoot->at(curRootIndex))->Xmax;    //std::cout<<currentHSMax.x<<"\t";
    currentHSMax.y=(currentRoot->at(curRootIndex))->Ymax;    //std::cout<<currentHSMax.y<<"\n";
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;
    int oldHSindex = currentHSindex;
    hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex);
    ASSERT(oldHSindex != currentHSindex);

    //    cout << "findNextHotSpot: changing location to" << currentHSindex << endl;
    return true;
}

void RegularRootLATP::makeNewRoot()
{
    cout << "Making new root for NodeID: " << NodeID << endl;

    if(currentRoot != NULL) {
        delete currentRoot;
        delete currentRootSnumber;
        delete currentRootCounter;
    }

    currentRoot = new vector<HotSpotShortInfo*>(*firstRoot);
    currentRootSnumber = new vector<unsigned int>(*firstRootSnumber);
    currentRootCounter = new vector<int>(*firstRootCounter);

    const int nonReplaceble = round(firstRoot->size() * rootPersistence);
    const int replaceable = firstRoot->size() - nonReplaceble;
    ASSERT(nonReplaceble >= 0 && nonReplaceble <= firstRoot->size());

    int maxCount = 0;
    //remove HS from current root by random manner
    for (int i = 0; i < replaceable; i++) {
        int hsNumber = -1;
        HotSpotShortInfo* removedItem = HotSpotsCollection::randomRemove(currentRoot, hsNumber);
        ASSERT(hsNumber != -1 && removedItem != NULL);

        currentRootSnumber->erase(currentRootSnumber->begin() + hsNumber);
        if (maxCount < currentRootCounter->at(hsNumber)) maxCount = currentRootCounter->at(hsNumber);
        currentRootCounter->erase(currentRootCounter->begin() + hsNumber);
    }

    //copy all HS without remaining in currentRoot
    vector<HotSpotShortInfo*>* allHSWithoutInCurrentRoot = new vector<HotSpotShortInfo*>();
    for (unsigned int hsNumber = 0; hsNumber < HotSpotsCollection::HSData.size(); hsNumber++) {
        bool foundInCurrent = false;
        for (int i = 0; i < currentRootSnumber->size(); i++)
            if ( (foundInCurrent = (currentRootSnumber->at(i) == hsNumber)) ) break;

        if (!foundInCurrent) {
            allHSWithoutInCurrentRoot->push_back(&HotSpotsCollection::HSData[hsNumber]);
        }
    }

    for (int i = 0; i < replaceable; i++) {
        int hsNumber = -1;
        HotSpotShortInfo* removedItem = HotSpotsCollection::randomRemove(allHSWithoutInCurrentRoot, hsNumber);
        ASSERT(hsNumber != -1 && removedItem != NULL);

        HotSpotShortInfo* insertedItem = hsc->findHotSpotbyName(removedItem->hotSpotName, hsNumber = -1);
        ASSERT(hsNumber != -1 && insertedItem != NULL);
        for(unsigned int j=0; j<currentRoot->size(); j++) ASSERT(currentRoot->at(j) != insertedItem);

        currentRoot->push_back(insertedItem);
        currentRootSnumber->push_back(hsNumber);
        currentRootCounter->push_back((int) round(uniform(0, (double) maxCount)));
    }

    delete allHSWithoutInCurrentRoot;
    //printFirstRoot();
    //printCurrentRoot();

    deleteLocalProbMatrix();
    makeLocalProbMatrix(powA);

    // ��������� ������� - ��� ������ ������� �������� ��������
    curRootIndex=0;
    currentHSMin.x=(currentRoot->at(curRootIndex))->Xmin;
    currentHSMin.y=(currentRoot->at(curRootIndex))->Ymin;
    currentHSMax.x=(currentRoot->at(curRootIndex))->Xmax;
    currentHSMax.y=(currentRoot->at(curRootIndex))->Ymax;
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;
    hsc->findHotSpotbyName( (currentRoot->at(curRootIndex))->hotSpotName, currentHSindex);

    targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
    targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);
    //todo ����� �� ������ ����� ��������?

    cout << endl;
}
