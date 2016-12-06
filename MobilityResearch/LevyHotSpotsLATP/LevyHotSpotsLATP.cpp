#include "LevyHotSpotsLATP.h"

Define_Module(LevyHotSpotsLATP);

LevyHotSpotsLATP::LevyHotSpotsLATP() {
    NodeID = -1;

    isPause = false;
    step = 0;
    jump = NULL;
    pause = NULL;
    kForSpeed = 1;
    roForSpeed = 0;

    currentHSindex = -1;

    usedWPcounts = false;
    currentHSWaypointNum = 0;

    movementsFinished = false;

    angle = -1;
    distance = -1;
    speed = -1;
    travelTime = 0;

    hsc=NULL;
    hsd=NULL;

    powA=2.0;

    waitTime = 0;

    wpFileName = NULL;
    trFileName = NULL;
}

void LevyHotSpotsLATP::setCurrentHSbordersWith(HotSpotData* hsi)
{
    currentHSMin.x = hsi->Xmin;    //std::cout<<currentHSMin.x<<"\t";
    currentHSMin.y = hsi->Ymin;    //std::cout<<currentHSMin.y<<"\t";
    currentHSMax.x = hsi->Xmax;    //std::cout<<currentHSMax.x<<"\t";
    currentHSMax.y = hsi->Ymax;    //std::cout<<currentHSMax.y<<"\n";
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;
    //todo сделать инициализацию ПРАВИЛЬНОГО СРЕДНЕГО количества точек
    //todo взять число точек из соответсвующей локации (по имени) текущего root'а
    currentHSWaypointNum = hsi->waypointNum;
    return;
}


void LevyHotSpotsLATP::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    double ciJ,aliJ,aciJ, ciP,aliP,aciP;

    if (stage == 0) {
        stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0;

        constraintAreaMin.x = par("constraintAreaMinX").doubleValue();
        constraintAreaMax.x = par("constraintAreaMaxX").doubleValue();
        constraintAreaMin.y = par("constraintAreaMinY").doubleValue();
        constraintAreaMax.y = par("constraintAreaMaxY").doubleValue();

        NodeID = (int) par("NodeID");
        usedWPcounts = par("usedWPcounts").boolValue();

        if (hasPar("ciJ") && hasPar("aliJ") && hasPar("aciJ") && hasPar("ciP") && hasPar("aliP") && hasPar("aciP") && hasPar("powA")) {

           ciJ  = par("ciJ").doubleValue();
           aliJ = par("aliJ").doubleValue();
           aciJ = par("aciJ").doubleValue();

           ciP  = par("ciP").doubleValue();
           aliP = par("aliP").doubleValue();
           aciP = par("aciP").doubleValue();

           powA = par("powA").doubleValue();

        } else { cout << "It is necessary to specify ALL parameters for length and pause Levy distribution"; exit(-112);}
    }

    if (jump  == NULL) jump  = new LeviJump(ciJ, aliJ, aciJ);
    if (pause == NULL) pause = new LeviPause(ciP, aliP, aciP);

    if (!hsc) {
        // загрузка данных о докациях
        hsc = HotSpotsCollection::getInstance();
        double minX, maxX, minY, maxY;
        hsc->getTotalSize(minX, maxX, minY, maxY);
        constraintAreaMin.x=minX; constraintAreaMin.y=minY;
        constraintAreaMax.x=maxX; constraintAreaMax.y=maxY;
    }

    if (hsd==NULL) {
        hsd = new HSDistanceMatrix();
        hsd->makeDistanceMatrix();
        hsd->makeProbabilityMatrix(powA);
    }

    // выбор случайной локации
    if (currentHSindex == -1) {
        currentHSindex=rand() % hsc->getHSData()->size();
        setCurrentHSbordersWith( &(hsc->getHSData()->at(currentHSindex)) );
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

int LevyHotSpotsLATP::getNodeID() {
    return NodeID;
}

void LevyHotSpotsLATP::setInitialPosition() {
    MobilityBase::setInitialPosition();
    
    lastPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
    lastPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
    targetPosition = lastPosition;
    ASSERT(isCorrectCoordinates(lastPosition.x, lastPosition.y));
}

bool LevyHotSpotsLATP::isHotSpotEmpty() {
    return currentHSMin.x == currentHSMax.x || currentHSMin.y == currentHSMax.y;
}

void LevyHotSpotsLATP::finish() {
    saveStatistics();
}

void LevyHotSpotsLATP::setTargetPosition() {
    if (movementsFinished) {nextChange = -1; return;};
    ASSERT(isCorrectCoordinates(lastPosition.x, lastPosition.y));
    ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));

    step++;
    if (isPause) {
        waitTime = (simtime_t) pause->get_Levi_rv();
        ASSERT(waitTime > 0);
        nextChange = simTime() + waitTime;
    } else {
        collectStatistics(simTime() - waitTime, simTime(), lastPosition.x, lastPosition.y);
        movementsFinished = !generateNextPosition(targetPosition, nextChange);

        if (movementsFinished) {nextChange = -1; return;};
        ASSERT(isCorrectCoordinates(targetPosition.x, targetPosition.y));
    }
    isPause = !isPause;
}

bool LevyHotSpotsLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    ASSERT(!usedWPcounts || currentHSWaypointNum >=0);
    cout << "currentHSindex = "<< currentHSindex << ", currentHSWaypointNum = " << currentHSWaypointNum << endl;//todo
    if (usedWPcounts && currentHSWaypointNum == 0) {
        //если счётчик равен 0, то пора менять локацию
        if (findNextHotSpotAndTargetPosition()) return true;
        else return false; // не нашли - останавливаемся
    }

    while (true) {
        // генерируем прыжок Леви как обычно
        angle = uniform(0, 2 * PI);
        distance = jump->get_Levi_rv();
        ASSERT(distance > 0);
        speed = kForSpeed * pow(distance, 1 - roForSpeed);
        Coord delta(distance * cos(angle), distance * sin(angle), 0);
        deltaVector = delta;
        travelTime = distance / speed;
        targetPosition = lastPosition + delta;
        ASSERT(targetPosition.x != lastPosition.x);
        nextChange = simTime() + travelTime;

        // если вышли за пределы локации
        if (!isCorrectCoordinates(targetPosition.x, targetPosition.y)) {
            if (isHotSpotEmpty()) { // если локация точечная
                if (usedWPcounts) {
                    // в этой ситуации (счётчик не нуль, а локация вырожденная - в такой обычно должна быть только одна точка)
                    // нужно выбрать снова ТУ ЖЕ позицию
                    targetPosition = lastPosition;
                    break;

                } else if (findNextHotSpotAndTargetPosition()) return true; // выбираем следующую локацию
                       else return false; // не нашли - останавливаемся
            }

            // для ускорения вычислений определяем вспомогательные переменные
            double x, y, Xdir, Ydir, dir;
            bool flag = ( (y=lastPosition.y) < currentHSCenter.y);

            // выбираем самую дальнюю от текущей позиции вершину прямоугольника текущей локации
            // и вычисляем координаты вектора из текущей позиции в эту вершину
            if ( (x=lastPosition.x) < currentHSCenter.x ) {
                if (flag) { Xdir=currentHSMax.x-x; Ydir=currentHSMax.y-y; }
                else      { Xdir=currentHSMax.x-x; Ydir=currentHSMin.y-y; }
            } else {
                if (flag) { Xdir=currentHSMin.x-x; Ydir=currentHSMax.y-y; }
                else      { Xdir=currentHSMin.x-x; Ydir=currentHSMin.y-y; }
            }

            // проверяем, можем ли остаться в прямоугольнике текущей локации, если прыгать к дальнему углу прямоугольника
            if ( distance > (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
                // не можем остаться
                if (usedWPcounts) continue; // генерируем заново прыжок
                else if (findNextHotSpotAndTargetPosition()) return true; // выбираем следующую локацию
                     else return false; // не нашли - останавливаемся
            }

            // можем остаться - прыгаем
            delta.x = Xdir * distance/dir;
            delta.y = Ydir * distance/dir;
            targetPosition = lastPosition + delta;
        }
        break; //получили правильный targetPosition
    }

    // уменьшаем счётчик количества путевых точек
    if (usedWPcounts) currentHSWaypointNum--;
    return true;
}


bool LevyHotSpotsLATP::findNextHotSpotAndTargetPosition() {
    if (findNextHotSpot()) {   // нашли следующую локацию - идём в её случайную точку
        targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
        targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);

        distance = sqrt( (targetPosition.x-lastPosition.x)*(targetPosition.x-lastPosition.x)+(targetPosition.y-lastPosition.y)*(targetPosition.y-lastPosition.y) );
        ASSERT(distance > 0);
        speed = kForSpeed * pow(distance, 1 - roForSpeed);
        travelTime = distance / speed;
        nextChange = simTime() + travelTime;
        return true;
    }

    return false;
}


bool LevyHotSpotsLATP::findNextHotSpot()
{
    int oldHSindex = currentHSindex;
    // выбираем новую локацию
    double rn, pr=0;
    rn=(double)rand()/RAND_MAX;
    for(unsigned int i=0; i<hsc->getHSData()->size(); i++) {
        if(i != currentHSindex ) pr+=(hsd->ProbabilityMatrix)[currentHSindex][i];
        if(rn <= pr) {currentHSindex=i; break; }
    }
    setCurrentHSbordersWith( &(hsc->getHSData()->at(currentHSindex)) );

    ASSERT(oldHSindex != currentHSindex);
    return true;
}


//-------------------------- Statistic collection ---------------------------------
void LevyHotSpotsLATP::collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y) {
    inTimes.push_back(inTime);
    outTimes.push_back(outTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
    hsc->getHSData()->at(currentHSindex).generatedSumTime += (outTime - inTime).dbl();
    hsc->getHSData()->at(currentHSindex).generatedWaypointNum++;

    Waypoint h(x, y, inTime.dbl(), outTime.dbl(), wpFileName);
    hsc->getHSData()->at(currentHSindex).waypoints.push_back(h);
}

void LevyHotSpotsLATP::saveStatistics() {
    char *outDir = NamesAndDirs::getOutDir();
    char *wpsDir = NamesAndDirs::getWpsDir();
    char *trsDir = NamesAndDirs::getTrsDir();
    char *hsDir  = NamesAndDirs::getHsDir();
    char *locs   = NamesAndDirs::getLocFile();

    if (NodeID == 0 ) {//чтобы записывал только один узел
        //--- Create output directories ---
        if (CreateDirectory(outDir, NULL)) cout << "create output directory: " << outDir << endl;
        else cout << "error create output directory: " << outDir << endl;

        if (CreateDirectory(wpsDir, NULL)) cout << "create output directory: " << wpsDir << endl;
        else cout << "error create output directory: " << wpsDir << endl;

        if (CreateDirectory(trsDir, NULL)) cout << "create output directory: " << trsDir << endl;
        else cout << "error create output directory: " << trsDir << endl;

        if (CreateDirectory(hsDir, NULL)) cout << "create output directory: " << hsDir << endl;
        else cout << "error create output directory: " << hsDir << endl;

        // --- Write HotSpots ---
        for (unsigned int i = 0; i < hsc->getHSData()->size(); i++) {
            char* fullNameHS = buildFullName(hsDir, hsc->getHSData()->at(i).hotSpotName);
            ofstream* hsFile = new ofstream(fullNameHS);
            (*hsFile) << hsc->getHSData()->at(i).Xmin << "\t" << hsc->getHSData()->at(i).Xmax << endl;
            (*hsFile) << hsc->getHSData()->at(i).Ymin << "\t" << hsc->getHSData()->at(i).Ymax << endl;
            (*hsFile) << hsc->getHSData()->at(i).generatedSumTime << "\t"<< hsc->getHSData()->at(i).generatedWaypointNum << endl;

            for(unsigned int j = 0; j < hsc->getHSData()->at(i).waypoints.size(); j++)
                (*hsFile) << hsc->getHSData()->at(i).waypoints[j].X  << "\t" << hsc->getHSData()->at(i).waypoints[j].Y  << "\t"
                          << hsc->getHSData()->at(i).waypoints[j].Tb << "\t" << hsc->getHSData()->at(i).waypoints[j].Te << "\t"
                          << hsc->getHSData()->at(i).waypoints[j].traceName << endl;

            hsFile->close();
            delete hsFile;
        }

        // --- Write Locations ---
        ofstream lcfile(locs);
        for(unsigned int i = 0; i < hsc->getHSData()->size(); i++) {
            lcfile << hsc->getHSData()->at(i).hotSpotName << "\t"<< hsc->getHSData()->at(i).generatedSumTime << "\t" << "\t";
            lcfile << hsc->getHSData()->at(i).generatedWaypointNum << "\t" << "\t";
            lcfile << hsc->getHSData()->at(i).Xmin << "\t"<< hsc->getHSData()->at(i).Xmax << "\t";
            lcfile << hsc->getHSData()->at(i).Ymin << "\t"<< hsc->getHSData()->at(i).Ymax << endl;
        }
        lcfile.close();
    }

    //--- Write points ---
    ofstream wpFile(buildFullName(wpsDir, wpFileName));
    ofstream trFile(buildFullName(trsDir, trFileName));
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

bool LevyHotSpotsLATP::isCorrectCoordinates(double x, double y) {
    if (currentHSMin.x <= x && x <= currentHSMax.x && currentHSMin.y <= y && y <= currentHSMax.y) return true;
    //log();
    return false;
}

void LevyHotSpotsLATP::log() {  // Отладочная функция
    cout << "----------------------------- LOG --------------------------------" << endl;
    cout << "step = " << step << ", isPause = " << isPause << endl;
    cout << "simTime() = " << simTime() << endl;
    cout << "lastPosition = " << lastPosition << endl;

    cout << "currentHSindex = " << currentHSindex << endl;
    cout << "\t currentHSMin.x = " << currentHSMin.x << ", currentHSMax.x = " << currentHSMax.x << endl;
    cout << "\t currentHSMin.y = " << currentHSMin.y << ", currentHSMax.y = " << currentHSMax.y << endl;
    cout << "\t currentHSCenter.x = " << currentHSCenter.x << ", currentHSCenter.y = " << currentHSCenter.y << endl;
    cout << "\t isHotSpotEmpty = " << isHotSpotEmpty() << endl;
    hsc->getHSData()->at(currentHSindex).print();

    if (isPause) {
        cout << "waitTime = " << waitTime << endl;
    } else {
        cout << "distance = " << distance << ", angle = " << angle << ", speed = " << speed << endl;
        cout << "deltaVector = " << deltaVector << ", travelTime = " << travelTime << endl;
    }

    cout << "targetPosition = " << targetPosition << endl;
    cout << "nextChange = " << nextChange << endl;

    cout << "movementsFinished = " << movementsFinished << endl;
    cout << "-------------------------------------------------------------" << endl << endl;
}
