#include <iostream>
#include <math.h>
#include <stdlib.h>

#include "SelfSimLATP.h"
#include "SelfSimMap.h"

Define_Module(SelfSimLATP);

SelfSimLATP::SelfSimLATP() {
    nextMoveIsWait = false;
    kForSpeed = 1;
    roForSpeed = 0;

    movementsFinished = false;

    hsc=NULL;
    hsd=NULL;

    pause = NULL;

    powA=2.0;

    rc=NULL;
    RootNumber=0;
    isRootReady=false;
    isDstMatrixReady=false;

    isWptLoaded=false;
    gen=NULL;
    isWptMatrixReady=false;
}

void SelfSimLATP::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);


    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 1" <<endl;


    if (stage == 0) {
        stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0;


    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 2" <<endl;


    if (hasPar("powA") && hasPar("ciP") && hasPar("aliP") && hasPar("aciP") ) {

        double ciP  = par("ciP").doubleValue();
        double aliP = par("aliP").doubleValue();
        double aciP = par("aciP").doubleValue();

        if ( pause == NULL) pause = new LeviPause(ciP, aliP, aciP);

        powA = par("powA").doubleValue();

    } else { cout << "It is necessary to specify ALL parameters"; exit(-112);}


    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 3" <<endl;


    constraintAreaMin.x = par("constraintAreaMinX").doubleValue();
    constraintAreaMax.x = par("constraintAreaMaxX").doubleValue();
    constraintAreaMin.y = par("constraintAreaMinY").doubleValue();
    constraintAreaMax.y = par("constraintAreaMaxY").doubleValue();


    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 4" <<endl;


    if (hsc==NULL) hsc = new HotSpotsCollection();


    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 5" <<endl;


    if (hsd==NULL) hsd = new HSDistanceMatrix();


    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 6" <<endl;


    // �������� ������ � ��������
    char* TracesDir = DEF_TR_DIR ;
    double minX, maxX, minY, maxY;
    hsc->readHotSpotsInfo(TracesDir, minX, maxX, minY, maxY);
    constraintAreaMin.x=minX; constraintAreaMin.y=minY;
    constraintAreaMax.x=maxX; constraintAreaMax.y=maxY;


    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 7" <<endl;


    hsd->makeDistanceMatrix();
    hsd->makeProbabilityMatrix(powA);


    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 8" <<endl;


    if (rc==NULL) rc = new RootCollection();
    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 8.1" <<endl;
    char* RootDir = DEF_RT_DIR ;
    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 8.2" <<endl;
    rc->readRootInfo(RootDir);
    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 8.3" <<endl;
    makeRoot();
    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 8.4" <<endl;
    buildDstMatrix();


    cout << "Node:" << par("fileSuffix").doubleValue() << ", step 9" <<endl;


    // ����� ��������� ������� �� ��������
    currentHSindex = rand() % currentRoot.size();
    currentHSMin.x=(currentRoot[currentHSindex]).Xmin;
    currentHSMin.y=(currentRoot[currentHSindex]).Ymin;
    currentHSMax.x=(currentRoot[currentHSindex]).Xmax;
    currentHSMax.y=(currentRoot[currentHSindex]).Ymax;
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;
    cout << "Root Number:" << RootNumber << endl;

    cout << "Initial HS:" << currentHSindex <<endl;

    // ��������� ������� ����� � ��������� �������
    if( gen == NULL ) loadHSWaypts();
    buildWptMatrix();
    }
}


void SelfSimLATP::setInitialPosition() {
    MobilityBase::setInitialPosition();
    
    currentWpt = rand() % waypts.size();
    lastPosition.x = waypts[currentWpt].x;
    lastPosition.y = waypts[currentWpt].y;
    cout << "Initial position: point #" << currentWpt << " x="<< lastPosition.x <<" y=" << lastPosition.y <<endl;
}

void SelfSimLATP::finish() {
    saveStatistics();
}

void SelfSimLATP::setTargetPosition() {
    if (!movementsFinished) {
        if (nextMoveIsWait) {
            simtime_t waitTime = (simtime_t) pause->get_Levi_rv();
            nextChange = simTime() + waitTime;
        } else {
            collectStatistics(simTime(), lastPosition.x, lastPosition.y);
            generateNextPosition(targetPosition, nextChange);
        }
        nextMoveIsWait = !nextMoveIsWait;
    } else {
        // ������������ � ������������� - ���������� ��������� �����������
        cout << "End of root!" << endl;
        nextChange = simTime() + 100000;
    }
}


void SelfSimLATP::generateNextPosition(Coord& targPos, simtime_t& nextChange)
{
    if( findNextWpt() ) {
        targPos = waypts[currentWpt];
        double distance = sqrt((targPos.x-lastPosition.x)*(targPos.x-lastPosition.x) +
                          (targPos.y-lastPosition.y)*(targPos.y-lastPosition.y));
        double speed = kForSpeed * pow(distance, 1 - roForSpeed);
        simtime_t travelTime = distance / speed;
        nextChange = simTime() + travelTime;
    } else if ( findNextHotSpot() ) {
        cout << "changing location to:" << currentHSindex << " HotSpot left:" << currentRoot.size() <<endl;
        isWptLoaded=false;
        loadHSWaypts();
        isWptMatrixReady=false;
        buildWptMatrix();
        currentWpt = rand() % waypts.size();
        targPos.x = waypts[currentWpt].x;
        targPos.y = waypts[currentWpt].y;
        double distance = sqrt((targPos.x-lastPosition.x)*(targPos.x-lastPosition.x) +
                          (targPos.y-lastPosition.y)*(targPos.y-lastPosition.y));
        double speed = kForSpeed * pow(distance, 1 - roForSpeed);
        simtime_t travelTime = distance / speed;
        nextChange = simTime() + travelTime;
    } else movementsFinished = true;
}



bool SelfSimLATP::findNextHotSpot()
{
    // �������� ����� �������
    if(currentRoot.size()>1) {
       double rn, pr=0, sum=0, h;
       unsigned int i;
       bool flag=false;
       rn=(double)rand()/RAND_MAX;
       for(i=0; i<currentRoot.size(); i++)
           if( (h=getDistance(currentHSindex, i))>0 ) sum+=pow(1/h, powA);
       for(i=0; i<currentRoot.size(); i++) {
           if( (h=getDistance(currentHSindex, i))>0 ) pr+=pow(1/h, powA)/sum;
           if(rn <= pr) {
               cout << "rn=" <<rn <<"  pr="<<pr<<endl;
               currentRoot.erase(currentRoot.begin()+currentHSindex);
               correctDstMatrix(currentHSindex);
               (i < currentHSindex)? currentHSindex=i : currentHSindex=i-1;
               flag=true;
               break; }
       }
       if(!flag) {  // �� �������� �������� �������� ��������� ����� �������
           currentHSindex=0;
           currentRoot.pop_back();
       }
       currentHSMin.x=(currentRoot[currentHSindex]).Xmin;
       currentHSMin.y=(currentRoot[currentHSindex]).Ymin;
       currentHSMax.x=(currentRoot[currentHSindex]).Xmax;
       currentHSMax.y=(currentRoot[currentHSindex]).Ymax;
       currentHSCenter=(currentHSMin+currentHSMax)*0.5;
       return true;
    }
    else return false;
}


void SelfSimLATP::move() {
    LineSegmentsMobilityBase::move();
}

//-------------------------- Root operations --------------------------------------
void SelfSimLATP::makeRoot()
{
    if(!isRootReady) {
       RootNumber = rand() % rc->RootData.size();
       currentRoot.clear();
       for(unsigned int i=0; i< (rc->RootData[RootNumber])->size(); i++) {
           currentRoot.push_back((rc->RootData[RootNumber])->at(i));
       }
    }
    isRootReady=true;
}

void SelfSimLATP::buildDstMatrix()
{
   Coord hscntr[currentRoot.size()];

   if(isRootReady && !isDstMatrixReady) {
       for(unsigned int i=0; i<currentRoot.size(); i++) {
           hscntr[i].x=0.5*(currentRoot[i].Xmin + currentRoot[i].Xmax);
           hscntr[i].y=0.5*(currentRoot[i].Ymin + currentRoot[i].Ymax);
       }
       dstMatrix.clear();
       for(unsigned int i=0; i<currentRoot.size(); i++) {
           vector<double>* dij;
           dij = new vector<double>;
           dij->clear();
           dij->push_back(0);
           for(unsigned int j=i+1; j<currentRoot.size(); j++) {
               double d;
               d=sqrt((hscntr[i].x-hscntr[j].x)*(hscntr[i].x-hscntr[j].x)+(hscntr[i].y-hscntr[j].y)*(hscntr[i].y-hscntr[j].y));
               dij->push_back(d);
           }
           dstMatrix.push_back(dij);
       }
       isDstMatrixReady=true;
   }
}


double SelfSimLATP::getDistance(unsigned int i, unsigned int j)
{
    if(i<=j) return (dstMatrix[i])->at(j-i);
        else return (dstMatrix[j])->at(i-j);
}


void SelfSimLATP::correctDstMatrix(unsigned int delete_Index)
{
    for(unsigned int i=0; i<delete_Index; i++) (dstMatrix[i])->erase( (dstMatrix[i])->begin()+delete_Index-i);
    dstMatrix[delete_Index]->clear();
    dstMatrix.erase(dstMatrix.begin()+delete_Index);
}

// ------------------------------------------------------------------------

void SelfSimLATP::loadHSWaypts()
{
    if(!isWptLoaded) {
       gen = new SelfSimMapGenerator(currentHSMin.x, currentHSMax.x, currentHSMin.y, currentHSMax.y, 9);
       gen->MakeSelfSimSet("variances.txt", (currentRoot[currentHSindex]).waypointNum);
       gen->PutSetOnMap();
       waypts.clear();
       for(unsigned int i=0; i<(gen->mapx).size(); i++) {
           Coord h;
           h.x = (gen->mapx)[i];
           h.y = (gen->mapy)[i];
           waypts.push_back(h);
       }
    (gen->mapx).clear();
    (gen->mapy).clear();
    delete gen;
    isWptLoaded=true;
    }
}


void SelfSimLATP::buildWptMatrix()
{
    if(isWptLoaded && !isWptMatrixReady) {
        wptMatrix.clear();
        for(unsigned int i=0; i<waypts.size(); i++) {
                   vector<double>* wij;
                   wij = new vector<double>;
                   wij->clear();
                   wij->push_back(0);
                   for(unsigned int j=i+1; j<waypts.size(); j++) {
                       double d;
                       d=sqrt((waypts[i].x-waypts[j].x)*(waypts[i].x-waypts[j].x)+(waypts[i].y-waypts[j].y)*(waypts[i].y-waypts[j].y));
                       wij->push_back(d);
                   }
                   wptMatrix.push_back(wij);
               }
        isWptMatrixReady=true;
    }
}

double SelfSimLATP::getWptDist(unsigned int i, unsigned int j)
{
    if(i<=j) return (wptMatrix[i])->at(j-i);
    else return (wptMatrix[j])->at(i-j);
}

void SelfSimLATP::correctWptMatrix(unsigned int delete_Index)
{
    for(unsigned int i=0; i<delete_Index; i++) (wptMatrix[i])->erase( (wptMatrix[i])->begin()+delete_Index-i);
    wptMatrix[delete_Index]->clear();
    wptMatrix.erase(wptMatrix.begin()+delete_Index);
}

bool SelfSimLATP::findNextWpt()
{
    if(waypts.size()>1) {
           double rn, pr=0, sum=0, h;
           rn=(double)rand()/RAND_MAX;
           for(unsigned int i=0; i<waypts.size(); i++)
               if( (h=getWptDist(currentWpt, i))>0 ) sum+=pow(1/h, powA);
           for(unsigned int i=0; i<waypts.size(); i++) {
               if( (h=getWptDist(currentWpt, i))>0 ) pr+=pow(1/h, powA)/sum;
               if(rn <= pr) {
                   waypts.erase(waypts.begin()+currentWpt);
                   correctWptMatrix(currentWpt);
                   (i < currentWpt)? currentWpt=i : currentWpt=i-1;
                   break;
               }
           }
    return true;
    } else { waypts.clear(); return false; }
}


//-------------------------- Statistic collection ---------------------------------
void SelfSimLATP::collectStatistics(simtime_t appearenceTime, double x, double y) {
    times.push_back(appearenceTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
}

void SelfSimLATP::saveStatistics() {
    char outFileName[256];
    char *fileName = createFileName(outFileName, 0,
            par("traceFileName").stringValue(), (int) ((par("fileSuffix"))));

    ofstream* file = new ofstream(fileName);
    for (unsigned int i = 0; i < times.size(); i++) {
        simtime_t time = times[i];
        double x = xCoordinates[i];
        double y = yCoordinates[i];
        (*file) << time << "\t" << x << "\t" << y << endl;
    }

    file->close();
    delete file;
}
