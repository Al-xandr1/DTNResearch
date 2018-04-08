#include "SelfSimLATP.h"

Define_Module(SelfSimLATP);

SelfSimLATP::SelfSimLATP() {
    NodeID = -1;

    // начинаем маршрут с паузы, чтобы мы "нормально прошли" первую точку (например постояли в ней)
    // а не так, чтобы при инициализации маршрута мы её поставили и при первой генерации сразу выбрали новую
    isPause = true;

    // первый шаг нулевой. Далее на нём проверяем, что мы прошли инициализацию,
    // и реально начали ходить (начиная с первого шага)
    step = 0;

    kForSpeed = 1;
    roForSpeed = 0;

    movementsFinished = false;

    hsc = NULL;
    hsd = NULL;

    pause = NULL;

    powAforHS = 2.0;
    powAforWP = 2.0;

    rc = NULL;
    RootNumber = 0;
    currentRoot = NULL;
    isRootReady = false;
    isDstMatrixReady = false;

    currentHSindex = -1;
    waypts = NULL;
    isWptLoaded = false;
    gen = NULL;
    isWptMatrixReady = false;

    waitTime = 0;

    wpFileName = NULL;
    trFileName = NULL;
}

void SelfSimLATP::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    double ciP, aliP, deltaXP, joinP;

    if (stage == 0) {
        stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0;

        constraintAreaMin.x = par("constraintAreaMinX").doubleValue();
        constraintAreaMax.x = par("constraintAreaMaxX").doubleValue();
        constraintAreaMin.y = par("constraintAreaMinY").doubleValue();
        constraintAreaMax.y = par("constraintAreaMaxY").doubleValue();

        NodeID = (int) par("NodeID");

        if (hasPar("powAforHS") && hasPar("powAforWP") && hasPar("ciP") && hasPar("aliP") && hasPar("deltaXP") &&
            hasPar("joinP")) {

            ciP = par("ciP").doubleValue();
            aliP = par("aliP").doubleValue();
            deltaXP = par("deltaXP").doubleValue();
            joinP = par("joinP").doubleValue();

            powAforHS = par("powAforHS").doubleValue();
            powAforWP = par("powAforWP").doubleValue();

        } else { cout << "It is necessary to specify ALL parameters"; exit(-112);}

        if (hasPar("kForSpeed") && hasPar("roForSpeed")) {
            kForSpeed = par("kForSpeed").doubleValue();
            roForSpeed = par("roForSpeed").doubleValue();
        } else { cout << "It is necessary to specify ALL parameters for speed function"; exit(-212);}
    }

    if (pause == NULL) pause = new LeviPause(ciP, aliP, deltaXP, joinP);

    if (!hsc) {
        // загрузка данных о докациях
        hsc = HotSpotsCollection::getInstance();
        double minX, maxX, minY, maxY;
        hsc->getTotalSize(minX, maxX, minY, maxY);
        constraintAreaMin.x = minX;
        constraintAreaMin.y = minY;
        constraintAreaMax.x = maxX;
        constraintAreaMax.y = maxY;
    }

    if (!hsd) hsd = HSDistanceMatrix::getInstance(powAforHS);

    // загрузка данных об эталонных маршрутах
    if (!rc) {
        rc = RootsCollection::getInstance();
        makeNewRoot();
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

void SelfSimLATP::setInitialPosition() {
    MobilityBase::setInitialPosition();

    setCurrentWpt(rand() % waypts->size());
    lastPosition.x = waypts->at(getCurrentWpt())->x;
    lastPosition.y = waypts->at(getCurrentWpt())->y;
    targetPosition = lastPosition;
    cout << "Initial position: point #" << getCurrentWpt() << " x=" << lastPosition.x << " y=" << lastPosition.y << endl;
}

void SelfSimLATP::setCurrentWpt(unsigned int i) {
    ASSERT(0 <= i && i < waypts->size());
    currentWpt = i;
}

unsigned int SelfSimLATP::getCurrentWpt() {
    ASSERT(0 <= currentWpt && currentWpt < waypts->size());
    return currentWpt;
}

void SelfSimLATP::setTargetPosition() {
    if (movementsFinished) {
        log("End of root!");
        nextChange = -1;
        return;
    };

    // так как данный метод вызывается на этапе инициализации, то этот вызов мы и пропускаем
    if (step++ == 0) return;

    if (isPause) {
        waitTime = (simtime_t) pause->get_Levi_rv();
        ASSERT(waitTime > 0);
        nextChange = simTime() + waitTime;
    } else {
        ASSERT(simTime() >= waitTime);
        collectStatistics(simTime() - waitTime, simTime(), lastPosition.x, lastPosition.y);
        movementsFinished = !generateNextPosition(targetPosition, nextChange);

        if (movementsFinished) {
            log("End of root!");
            nextChange = -1;
            return;
        };
    }
    isPause = !isPause;
}

bool SelfSimLATP::generateNextPosition(Coord &targetPosition, simtime_t &nextChange) {
    if (findNextWpt()) {
        targetPosition = *(waypts->at(getCurrentWpt()));
        double distance = sqrt((targetPosition.x - lastPosition.x) * (targetPosition.x - lastPosition.x) +
                               (targetPosition.y - lastPosition.y) * (targetPosition.y - lastPosition.y));

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
        isWptLoaded = false;
        loadHSWaypts();
        isWptMatrixReady = false;
        buildWptMatrix();
        setCurrentWpt(rand() % waypts->size());
        targetPosition.x = waypts->at(getCurrentWpt())->x;
        targetPosition.y = waypts->at(getCurrentWpt())->y;
        double distance = sqrt((targetPosition.x - lastPosition.x) * (targetPosition.x - lastPosition.x) +
                               (targetPosition.y - lastPosition.y) * (targetPosition.y - lastPosition.y));

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

bool SelfSimLATP::findNextHotSpot() {
    // выбираем новую локацию
    if (currentRoot->size() > 1) {
        double rn, pr = 0, sum = 0, h;
        unsigned int i;
        bool flag = false;
        do { rn = (double) rand() / RAND_MAX; } while (rn == 0);
        for (i = 0; i < currentRoot->size(); i++)
            if ((h = getDistance(currentHSindex, i)) > 0) sum += pow(1 / h, powAforHS);
        for (i = 0; i < currentRoot->size(); i++) {
            if ((h = getDistance(currentHSindex, i)) > 0) pr += pow(1 / h, powAforHS);
            if (rn <= pr / sum) {
                //cout << "DEBUG!!! rn=" << rn << ",  pr=" << pr << ", sum=" << sum << endl;
                myDelete(currentRoot->at(currentHSindex));
                currentRoot->erase(currentRoot->begin() + currentHSindex);
                correctMatrix(dstMatrix, currentHSindex);
                ASSERT(currentHSindex >= 0);
                (i < currentHSindex) ? setCurrentHSindex(i) : setCurrentHSindex(i - 1);
                flag = true;
                break;
            }
        }
        if (!flag) {  // на маршруте осталось несколок посещений одной локации
            setCurrentHSindex(0);
            myDelete(currentRoot->back());
            currentRoot->pop_back();
        }
        return true;
    } else return false;
}


void SelfSimLATP::setCurrentHSindex(int hsIndex) {
    ASSERT(hsIndex >= 0 && hsIndex < currentRoot->size());
    currentHSindex = hsIndex;
    currentHSMin.x = currentRoot->at(currentHSindex)->Xmin;
    currentHSMin.y = currentRoot->at(currentHSindex)->Ymin;
    currentHSMax.x = currentRoot->at(currentHSindex)->Xmax;
    currentHSMax.y = currentRoot->at(currentHSindex)->Ymax;
    currentHSCenter = (currentHSMin + currentHSMax) * 0.5;
}


//-------------------------- Root operations --------------------------------------
void SelfSimLATP::makeNewRoot() {
    myDelete(currentRoot);
    isRootReady = false;
    makeRoot();
    isDstMatrixReady = false;
    buildDstMatrix();

    // выбор случайной локации из маршрута
    setCurrentHSindex(rand() % currentRoot->size());

    // генерация путевых точек в выбранной локации
    isWptLoaded = false;
    loadHSWaypts();
    isWptMatrixReady = false;
    buildWptMatrix();

    // выбираем случайную точку в новой локации
    setCurrentWpt(rand() % waypts->size());

    // начинаем маршрут с паузы, чтобы мы "нормально прошли" первую точку (например постояли в ней)
    // а не так, чтобы при инициализации маршрута мы её поставили и при первой генерации сразу выбрали новую
    isPause = true;
}

void SelfSimLATP::makeRoot() {
    ASSERT(!currentRoot);

    if (!isRootReady) {
        RootNumber = NodeID % rc->getRootsData()->size();
        ASSERT(RootNumber >= 0 && RootNumber < rc->getRootsData()->size());
        currentRoot = new vector<HotSpotData *>();
        for (unsigned int i = 0; i < rc->getRootsData()->at(RootNumber)->size(); i++) {
            currentRoot->push_back(new HotSpotData(rc->getRootsData()->at(RootNumber)->at(i)));
        }
        log("Root made");
    }
    isRootReady = true;
}

void SelfSimLATP::buildDstMatrix() {
    ASSERT(currentRoot);

    Coord hscntr[currentRoot->size()];

    if (isRootReady && !isDstMatrixReady) {
        for (unsigned int i = 0; i < currentRoot->size(); i++) {
            hscntr[i].x = 0.5 * (currentRoot->at(i)->Xmin + currentRoot->at(i)->Xmax);
            hscntr[i].y = 0.5 * (currentRoot->at(i)->Ymin + currentRoot->at(i)->Ymax);
        }

        for (unsigned int i = 0; i < dstMatrix.size(); i++) myDelete(dstMatrix[i]);
        dstMatrix.clear();

        for (unsigned int i = 0; i < currentRoot->size(); i++) {
            vector<double> *dij = new vector<double>;
            dij->push_back(0);
            for (unsigned int j = i + 1; j < currentRoot->size(); j++) {
                dij->push_back(sqrt((hscntr[i].x - hscntr[j].x) * (hscntr[i].x - hscntr[j].x) +
                                    (hscntr[i].y - hscntr[j].y) * (hscntr[i].y - hscntr[j].y)));
            }
            dstMatrix.push_back(dij);
        }
        isDstMatrixReady = true;
    }
}


double SelfSimLATP::getDistance(unsigned int i, unsigned int j) {
    if (i <= j) return (dstMatrix[i])->at(j - i);
    else return (dstMatrix[j])->at(i - j);
}

// ------------------------------------------------------------------------


void SelfSimLATP::loadHSWaypts() {
    ASSERT(currentRoot);

    if (!isWptLoaded) {
        gen = new SelfSimMapGenerator(currentHSMin.x, currentHSMax.x, currentHSMin.y, currentHSMax.y, 9);
        gen->MakeSelfSimSet(buildFullName(DEF_TR_DIR, VAR_FILE), currentRoot->at(currentHSindex)->waypointNum);
        gen->PutSetOnMap();
        if (waypts) deleteVector(waypts, false);
        else waypts = new vector<Coord *>();
        ASSERT(waypts && waypts->size() == 0);
        for (unsigned int i = 0; i < (gen->mapx).size(); i++) {
            waypts->push_back(new Coord((gen->mapx)[i], (gen->mapy)[i]));
        }
        (gen->mapx).clear();
        (gen->mapy).clear();
        myDelete(gen);
        isWptLoaded = true;

        //checking of duplicated waypoints
        for (unsigned int i = 0; i < waypts->size() - 1; i++) {
            Coord *one = waypts->at(i);
            for (unsigned int j = i + 1; j < waypts->size(); j++) {
                Coord *another = waypts->at(j);
                if (one->x == another->x && one->y == another->y) {
                    cout << endl;
                    printf("currentHSIndex = %d", currentHSindex);
                    cout << endl;
                    currentRoot->at(currentHSindex)->print();
                    printf("currentHSMin = (%0.30f, %0.30f), currentHSMax = (%0.30f, %0.30f)",
                           currentHSMin.x, currentHSMin.y, currentHSMax.x, currentHSMax.y);
                    cout << endl;
                    printf("one = (%0.30f, %0.30f) i = %d, another = (%0.30f, %0.30f) j = %d, waypts.size() = %d",
                           one->x, one->y, i, another->x, another->y, j, waypts->size());
                    cout << endl;

                    exit(-141);
                }
            }
        }
    }
}


void SelfSimLATP::buildWptMatrix() {
    if (isWptLoaded && !isWptMatrixReady) {
        for (unsigned int i = 0; i < wptMatrix.size(); i++) myDelete(wptMatrix[i]);
        wptMatrix.clear();

        for (unsigned int i = 0; i < waypts->size(); i++) {
            vector<double> *wij = new vector<double>;
            wij->push_back(0);
            for (unsigned int j = i + 1; j < waypts->size(); j++) {
                wij->push_back(sqrt((waypts->at(i)->x - waypts->at(j)->x) * (waypts->at(i)->x - waypts->at(j)->x) +
                                    (waypts->at(i)->y - waypts->at(j)->y) * (waypts->at(i)->y - waypts->at(j)->y)));
            }
            wptMatrix.push_back(wij);
        }
        isWptMatrixReady = true;
    }
}

double SelfSimLATP::getWptDist(unsigned int i, unsigned int j) {
    if (i <= j) {
        ASSERT(i >= 0 && i < wptMatrix.size());
        ASSERT((j-i) >= 0 && (j-i) < wptMatrix[i]->size());
        return (wptMatrix[i])->at(j - i);
    } else {
        ASSERT(j >= 0 && j < wptMatrix.size());
        ASSERT((i-j) >= 0 && (i-j) < wptMatrix[j]->size());
        return (wptMatrix[j])->at(i - j);
    }
}

void SelfSimLATP::correctMatrix(vector<vector<double> *> &matrix, unsigned int delete_Index) {
    ASSERT(delete_Index >= 0 && delete_Index < matrix.size());
    for (unsigned int i = 0; i < delete_Index; i++) (matrix[i])->erase((matrix[i])->begin() + delete_Index - i);
    vector<double> *removed = matrix[delete_Index];
    removed->clear();
    matrix.erase(matrix.begin() + delete_Index);
    delete removed;
}

bool SelfSimLATP::findNextWpt() {
    if (waypts->size() > 1) {
        double rn, pr = 0, sum = 0, h;
        do { rn = (double) rand() / RAND_MAX; } while (rn == 0);
        for (unsigned int i = 0; i < waypts->size(); i++)
            if ((h = getWptDist(getCurrentWpt(), i)) > 0) sum += pow(1 / h, powAforWP);

        if (sum == 0) {// remains only duplicates of waypoints
            myDelete(waypts->at(getCurrentWpt()));
            waypts->erase(waypts->begin() + getCurrentWpt());
            correctMatrix(wptMatrix, getCurrentWpt());
            if (getCurrentWpt() > 0) setCurrentWpt(getCurrentWpt() - 1);

        } else {
            bool found = false;
            int additions = 0;
            for (unsigned int i = 0; i < waypts->size(); i++) {
                if ((h = getWptDist(getCurrentWpt(), i)) > 0) {
                    pr += pow(1 / h, powAforWP);
                    additions++;
                }
                if (rn <= pr / sum) {
                    correctMatrix(wptMatrix, getCurrentWpt());
                    myDelete(waypts->at(getCurrentWpt()));
                    waypts->erase(waypts->begin() + getCurrentWpt());
                    (i < currentWpt) ? setCurrentWpt(i) : setCurrentWpt(i - 1);
                    found = true;
                    break;
                }
            }
            if (!found) {
                printf("rn = %0.30f, pr = %0.30f, sum = %0.30f, pr/sum = %0.30f,  additions = %d, waypts.size() = %d, currentWpt = %d",
                       rn, pr, sum, pr / sum, additions, waypts->size(), getCurrentWpt());
                cout << endl;
                for (unsigned int i = 0; i < waypts->size(); i++) {
                    printf("index = %d, h = %0.30f, coord = (%0.30f, %0.30f)",
                           i, getWptDist(getCurrentWpt(), i), waypts->at(i)->x, waypts->at(i)->y);
                    cout << endl;
                }
                exit(-132);
            }
        }

        return true;
    } else {
        ASSERT(waypts);
        deleteVector(waypts, false);
        ASSERT(waypts && waypts->size() == 0);

        return false;
    }
}

void SelfSimLATP::log(string log) {
    cout << "NodeId = " << NodeID << ": "  << log << endl;
}


//-------------------------- Statistic collection ---------------------------------
void SelfSimLATP::collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y) {
    inTimes.push_back(inTime);
    outTimes.push_back(outTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
}


void SelfSimLATP::saveStatistics() {
    log("Start saving statistics...");
    const char *outDir = NamesAndDirs::getOutDir();
    const char *wpsDir = NamesAndDirs::getOutWpsDir();
    const char *trsDir = NamesAndDirs::getOutTrsDir();

    if (NodeID == 0) {//чтобы записывал только один узел
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
        const char *wpName = buildFullName(wpsDir, wpFileName);
        const char *trName = buildFullName(trsDir, trFileName);
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
    log("Statistics saved");
}
