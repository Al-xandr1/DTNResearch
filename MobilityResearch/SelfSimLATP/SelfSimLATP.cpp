#include "SelfSimLATP.h"

Define_Module(SelfSimLATP);

SelfSimLATP::SelfSimLATP() {

    NodeID = -1;

    isPause = false;
    step = 0;
    kForSpeed = 1;
    roForSpeed = 0;

    movementsFinished = false;

    hsc=NULL;
    hsd=NULL;

    pause = NULL;

    powAforHS=2.0;
    powAforWP=2.0;

    rc=NULL;
    RootNumber=0;
    isRootReady=false;
    isDstMatrixReady=false;

    currentHSindex = -1;
    isWptLoaded=false;
    gen=NULL;
    isWptMatrixReady=false;

    waitTime = 0;

    wpFileName = NULL;
    trFileName = NULL;
}

void SelfSimLATP::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    double ciP,aliP,deltaXP,joinP;

    if (stage == 0) {
        stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0;

        constraintAreaMin.x = par("constraintAreaMinX").doubleValue();
        constraintAreaMax.x = par("constraintAreaMaxX").doubleValue();
        constraintAreaMin.y = par("constraintAreaMinY").doubleValue();
        constraintAreaMax.y = par("constraintAreaMaxY").doubleValue();

        NodeID = (int) par("NodeID");

        if (hasPar("powAforHS") && hasPar("powAforWP") && hasPar("ciP") && hasPar("aliP") && hasPar("deltaXP") && hasPar("joinP")) {

            ciP  = par("ciP").doubleValue();
            aliP = par("aliP").doubleValue();
            deltaXP = par("deltaXP").doubleValue();
            joinP = par("joinP").doubleValue();

            powAforHS = par("powAforHS").doubleValue();
            powAforWP = par("powAforWP").doubleValue();

        } else { cout << "It is necessary to specify ALL parameters"; exit(-112);}
    }

    if (pause == NULL) pause = new LeviPause(ciP, aliP, deltaXP, joinP);

    if (!hsc) {
        // загрузка данных о докациях
        hsc = HotSpotsCollection::getInstance();
        double minX, maxX, minY, maxY;
        hsc->getTotalSize(minX, maxX, minY, maxY);
        constraintAreaMin.x=minX; constraintAreaMin.y=minY;
        constraintAreaMax.x=maxX; constraintAreaMax.y=maxY;
    }

    if (!hsd) hsd = HSDistanceMatrix::getInstance(powAforHS);

    // загрузка данных об эталонных маршрутах
    if (!rc) {
        rc = RootsCollection::getInstance();
        makeRoot();
        buildDstMatrix();
    }

    // выбор случайной локации из маршрута
    if (currentHSindex == -1) {
        currentHSindex = rand() % currentRoot.size();
        currentHSMin.x=(currentRoot[currentHSindex]).Xmin;
        currentHSMin.y=(currentRoot[currentHSindex]).Ymin;
        currentHSMax.x=(currentRoot[currentHSindex]).Xmax;
        currentHSMax.y=(currentRoot[currentHSindex]).Ymax;
        currentHSCenter=(currentHSMin+currentHSMax)*0.5;
    }

    // генерация путевых точек в выбранной локации
    if (gen == NULL) {
        loadHSWaypts();
        buildWptMatrix();
    }

    if (wpFileName == NULL && trFileName == NULL) {
        wpFileName = new char[256];
        trFileName = new char[256];
        wpFileName = createFileName(wpFileName, 0, par("traceFileName").stringValue(),
                (int) ((par("NodeID"))), WAYPOINTS_TYPE);
        trFileName = createFileName(trFileName, 0, par("traceFileName").stringValue(),
                (int) ((par("NodeID"))), TRACE_TYPE);
    }
}

void SelfSimLATP::handleMessage(cMessage * message)
{
    if (message->isSelfMessage())
        MobilityBase::handleMessage(message);
    else
        switch (message->getKind()) {
            //todo Заглушка. Делать окончание и начало дня как для RegularRootLATP::handleMessage
        }
}

void SelfSimLATP::setInitialPosition() {
    MobilityBase::setInitialPosition();

    currentWpt = rand() % waypts.size();
    lastPosition.x = waypts[currentWpt].x;
    lastPosition.y = waypts[currentWpt].y;
    targetPosition = lastPosition;
    cout << "Initial position: point #" << currentWpt << " x="<< lastPosition.x <<" y=" << lastPosition.y <<endl;
}

void SelfSimLATP::setTargetPosition() {
    if (movementsFinished) {cout<<"End of root!"<<endl; nextChange = -1; return;};

    step++;
    if (isPause) {
        waitTime = (simtime_t) pause->get_Levi_rv();
        ASSERT(waitTime > 0);
        nextChange = simTime() + waitTime;
    } else {
        collectStatistics(simTime() - waitTime, simTime(), lastPosition.x, lastPosition.y);
        movementsFinished = !generateNextPosition(targetPosition, nextChange);

        if (movementsFinished) {cout<<"End of root!"<<endl; nextChange = -1; return;};
    }
    isPause = !isPause;
}

bool SelfSimLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange)
{
    if (findNextWpt()) {
        targetPosition = waypts[currentWpt];
        double distance = sqrt((targetPosition.x-lastPosition.x)*(targetPosition.x-lastPosition.x) +
                               (targetPosition.y-lastPosition.y)*(targetPosition.y-lastPosition.y));

        simtime_t travelTime;
        if (distance != 0) {
            double speed = kForSpeed * pow(distance, 1 - roForSpeed);
            travelTime = distance / speed;
        } else {
            //pause is generated again
            travelTime = (simtime_t) pause->get_Levi_rv();
        }

        nextChange = simTime() + travelTime;

    } else if (findNextHotSpot()) {
        isWptLoaded=false;
        loadHSWaypts();
        isWptMatrixReady=false;
        buildWptMatrix();
        currentWpt = rand() % waypts.size();
        targetPosition.x = waypts[currentWpt].x;
        targetPosition.y = waypts[currentWpt].y;
        double distance = sqrt((targetPosition.x-lastPosition.x)*(targetPosition.x-lastPosition.x) +
                               (targetPosition.y-lastPosition.y)*(targetPosition.y-lastPosition.y));

        simtime_t travelTime;
        if (distance != 0) {
            double speed = kForSpeed * pow(distance, 1 - roForSpeed);
            travelTime = distance / speed;
        } else {
            //pause is generated again
            travelTime = (simtime_t) pause->get_Levi_rv();
        }

        nextChange = simTime() + travelTime;
    } else return false;

    return true;
}

bool SelfSimLATP::findNextHotSpot()
{
    // выбираем новую локацию
    if(currentRoot.size()>1) {
       double rn, pr=0, sum=0, h;
       unsigned int i;
       bool flag=false;
       rn=(double)rand()/RAND_MAX;
       for(i=0; i<currentRoot.size(); i++)
           if( (h=getDistance(currentHSindex, i))>0 ) sum+=pow(1/h, powAforHS);
       for(i=0; i<currentRoot.size(); i++) {
           if( (h=getDistance(currentHSindex, i))>0 ) pr+=pow(1/h, powAforHS);
           if(rn <= pr/sum) {
               //cout << "rn=" <<rn <<"  pr="<<pr<<endl;
               currentRoot.erase(currentRoot.begin()+currentHSindex);
               correctMatrix(dstMatrix, currentHSindex);
               (i < currentHSindex)? currentHSindex=i : currentHSindex=i-1;
               flag=true;
               break; }
       }
       if(!flag) {  // на маршруте осталось несколок посещений одной локации
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


//-------------------------- Root operations --------------------------------------
void SelfSimLATP::makeRoot()
{
    if(!isRootReady) {
       RootNumber = rand() % rc->getRootsData()->size();
       currentRoot.clear();
       for(unsigned int i=0; i< rc->getRootsData()->at(RootNumber)->size(); i++) {
           currentRoot.push_back(rc->getRootsData()->at(RootNumber)->at(i));
       }
       cout << "root made" << endl;
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

// ------------------------------------------------------------------------


void SelfSimLATP::loadHSWaypts()
{
    if(!isWptLoaded) {
       gen = new SelfSimMapGenerator(currentHSMin.x, currentHSMax.x, currentHSMin.y, currentHSMax.y, 9);
       gen->MakeSelfSimSet(buildFullName(DEF_TR_DIR, VAR_FILE), (currentRoot[currentHSindex]).waypointNum);
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

       //checking of duplicated waypoints
//       for (unsigned int i = 0; i < waypts.size() - 1; i++) {
//           Coord one = waypts[i];
//           for (unsigned int j = i + 1;  j < waypts.size(); j++) {
//               Coord another = waypts[j];
//               if (one.x == another.x && one.y == another.y) {
//                   cout << endl;
//                   printf("currentHSIndex = %d", currentHSindex);
//                   cout << endl;
//                   currentRoot[currentHSindex].printHotSpotRootInfo();
//                   printf("currentHSMin = (%0.30f, %0.30f), currentHSMax = (%0.30f, %0.30f)",
//                           currentHSMin.x, currentHSMin.y, currentHSMax.x, currentHSMax.y);
//                   cout << endl;
//                   printf("one = (%0.30f, %0.30f) i = %d, another = (%0.30f, %0.30f) j = %d, waypts.size() = %d",
//                           one.x, one.y, i, another.x, another.y, j, waypts.size());
//                   cout << endl;
//
//                   exit(-2);
//               }
//           }
//       }
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

void SelfSimLATP::correctMatrix(vector<vector<double>*> &matrix, unsigned int delete_Index)
{
    for(unsigned int i=0; i<delete_Index; i++) (matrix[i])->erase( (matrix[i])->begin()+delete_Index-i);
    vector<double>* removed = matrix[delete_Index];
    removed->clear();
    matrix.erase(matrix.begin()+delete_Index);
    delete removed;
}

bool SelfSimLATP::findNextWpt()
{
    if(waypts.size()>1) {
           double rn, pr=0, sum=0, h;
           do {rn=(double)rand()/RAND_MAX;} while(rn == 0);
           for(unsigned int i=0; i<waypts.size(); i++)
               if( (h=getWptDist(currentWpt, i))>0 ) sum+=pow(1/h, powAforWP);

           if (sum == 0) {// remains only duplicates of waypoints
               waypts.erase(waypts.begin()+currentWpt);
               correctMatrix(wptMatrix, currentWpt);
               if (currentWpt > 0) currentWpt--;

           } else {
//               bool found = false;
//               int additions = 0;
               for(unsigned int i=0; i<waypts.size(); i++) {
                   if( (h=getWptDist(currentWpt, i))>0 ) {
                       pr+=pow(1/h, powAforWP);
//                       additions++;
                   }
                   if(rn <= pr/sum) {
                       waypts.erase(waypts.begin()+currentWpt);
                       correctMatrix(wptMatrix, currentWpt);
                       (i < currentWpt)? currentWpt=i : currentWpt=i-1;
//                       found = true;
                       break;
                   }
               }
//               if (!found) {
//                   printf("rn = %0.30f, pr = %0.30f, sum = %0.30f, pr/sum = %0.30f,  additions = %d, waypts.size() = %d, currentWpt = %d",
//                           rn, pr, sum, pr/sum, additions, waypts.size(), currentWpt); cout << endl;
//                   for(unsigned int i=0; i<waypts.size(); i++) {
//                       printf("index = %d, h = %0.30f, coord = (%0.30f, %0.30f)", i, getWptDist(currentWpt, i), waypts[i].x, waypts[i].y); cout << endl;
//                   }
//                   exit(-1);
//               }
           }

           return true;
    } else { waypts.clear(); return false; }
}


//-------------------------- Statistic collection ---------------------------------
void SelfSimLATP::collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y) {
    inTimes.push_back(inTime);
    outTimes.push_back(outTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
}


void SelfSimLATP::saveStatistics() {
    char *outDir = NamesAndDirs::getOutDir();
    char *wpsDir = NamesAndDirs::getWpsDir();
    char *trsDir = NamesAndDirs::getTrsDir();

    if (NodeID == 0 ) {//чтобы записывал только один узел
        //--- Create output directories ---
        if (CreateDirectory(outDir, NULL)) cout << "create output directory: " << outDir << endl;
        else cout << "error create output directory: " << outDir << endl;

        if (CreateDirectory(wpsDir, NULL)) cout << "create output directory: " << wpsDir << endl;
        else cout << "error create output directory: " << wpsDir << endl;

        if (CreateDirectory(trsDir, NULL)) cout << "create output directory: " << trsDir << endl;
        else cout << "error create output directory: " << trsDir << endl;
    }

    //--- Write points ---
    if (outTimes.size() > 0) {
        char *wpName = buildFullName(wpsDir, wpFileName);
        char *trName = buildFullName(trsDir, trFileName);
        cout << "wpName = " << wpName << endl;
        cout << "trName = " << trName << endl << endl;

        ofstream wpFile(wpName);
        ofstream trFile(trName);
        for (unsigned int i = 0; i < outTimes.size(); i++) {
            simtime_t inTime = inTimes[i];
            simtime_t outTime = outTimes[i];
            double x = xCoordinates[i];
            double y = yCoordinates[i];

            wpFile << x << "\t" << y << "\t" << inTime << "\t" << outTime << endl;
            trFile << inTime << "\t" << x << "\t" << y << endl;
        }

        wpFile.close();
        trFile.close();
    }
}
