#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <windows.h>

#include "LevyHotSpotsLATP.h"

#define TRACE_TYPE ".txt"
#define WAYPOINTS_TYPE ".wpt"

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

    if (hsc==NULL) {
        hsc = new HotSpotsCollection();
        // загрузка данных о докациях
        char* TracesDir = DEF_TR_DIR ;
        double minX, maxX, minY, maxY;
        hsc->readHotSpotsInfo(TracesDir, minX, maxX, minY, maxY);
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
        currentHSindex=rand() % (hsc->HSData).size();
        currentHSMin.x=((hsc->HSData)[currentHSindex]).Xmin;
        currentHSMin.y=((hsc->HSData)[currentHSindex]).Ymin;
        currentHSMax.x=((hsc->HSData)[currentHSindex]).Xmax;
        currentHSMax.y=((hsc->HSData)[currentHSindex]).Ymax;
        currentHSCenter=(currentHSMin+currentHSMax)*0.5;
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

int LevyHotSpotsLATP::getNodeID()
{
    return NodeID;
}

void LevyHotSpotsLATP::setInitialPosition() {
    MobilityBase::setInitialPosition();
    
    lastPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
    lastPosition.y = uniform(currentHSMin.y, currentHSMax.y); 
    //log();
    if (!isCorrectCoordinates(lastPosition.x, lastPosition.y)) exit(-555);
}

bool LevyHotSpotsLATP::isHotSpotEmpty() {
    return currentHSMin.x == currentHSMax.x || currentHSMin.y == currentHSMax.y;
}

void LevyHotSpotsLATP::finish() {
    saveStatistics();
}

void LevyHotSpotsLATP::setTargetPosition() {
    if (!movementsFinished) {
        step++;
        if (isPause) {
            waitTime = (simtime_t) pause->get_Levi_rv();
            nextChange = simTime() + waitTime;
            //log();
            //if (!isCorrectCoordinates(lastPosition.x, lastPosition.y)) exit(-666);
        } else {
            //if (!isCorrectCoordinates(lastPosition.x, lastPosition.y)) exit(-777);
            collectStatistics(simTime() - waitTime, simTime(), lastPosition.x, lastPosition.y);
            generateNextPosition(targetPosition, nextChange);
            //log();
            //if (!isCorrectCoordinates(targetPosition.x, targetPosition.y)) exit(-888);
        }
        isPause = !isPause;
    } else {
        // остановка перемещений по документации
        nextChange = -1;
        //log();
    }
}

void LevyHotSpotsLATP::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    
    // генерируем прыжок Леви как обычно
    angle = uniform(0, 2 * PI);
    distance = jump->get_Levi_rv();
    speed = kForSpeed * pow(distance, 1 - roForSpeed);
    Coord delta(distance * cos(angle), distance * sin(angle), 0);
    deltaVector = delta;
    travelTime = distance / speed;

    targetPosition = lastPosition + delta;
    nextChange = simTime() + travelTime;

    // если вышли за пределы локации
    if (currentHSMin.x >= targetPosition.x || targetPosition.x >= currentHSMax.x || currentHSMin.y >= targetPosition.y || targetPosition.y >= currentHSMax.y) {
        if (isHotSpotEmpty()) { // если локация точечная
//            cout << "HotSpot is empty! select next" << endl;
            if ( findNextHotSpot() ) {   // нашли следующую локацию - идём в её случайную точку
                targetPosition.x = uniform(currentHSMin.x, currentHSMax.x);
                targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);

                distance = sqrt( (targetPosition.x-lastPosition.x)*(targetPosition.x-lastPosition.x)+(targetPosition.y-lastPosition.y)*(targetPosition.y-lastPosition.y) );
                speed = kForSpeed * pow(distance, 1 - roForSpeed);
                travelTime = distance / speed;
                nextChange = simTime() + travelTime;

            } else movementsFinished = true;  // не нашли - останавливаемся
            return;
        }

        // для ускорения вычислений определяем вспомогательные переменные
        double x, y, Xdir, Ydir, dir;
        bool flag = ( (y=getLastPosition().y) < currentHSCenter.y);

        // выбираем самую дальнюю от текущей позиции вершину прямоугольника текущей локации 
        // и вычисляем координаты вектора из текущей позиции в эту вершину
        if ( (x=getLastPosition().x) < currentHSCenter.x ) {
            if (flag) { Xdir=currentHSMax.x-x; Ydir=currentHSMax.y-y; }
            else      { Xdir=currentHSMax.x-x; Ydir=currentHSMin.y-y; }
        } else {
            if (flag) { Xdir=currentHSMin.x-x; Ydir=currentHSMax.y-y; }
            else      { Xdir=currentHSMin.x-x; Ydir=currentHSMin.y-y; }
        }

        // проверяем, можем ли остаться в прямоугольнике текущей локации, если прыгать к дальнему углу прямоугольника
        if ( distance <= (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
            // можем - прыгаем
            delta.x = Xdir * distance/dir;
            delta.y = Ydir * distance/dir;
            targetPosition = getLastPosition() + delta;
        } else { // не можем - надо переходить в другую локацию
            if ( findNextHotSpot() ) {   // нашли следующую локацию - идём в её случайную точку
               targetPosition.x = uniform(currentHSMin.x, currentHSMax.x); 
               targetPosition.y = uniform(currentHSMin.y, currentHSMax.y);

               distance = sqrt( (targetPosition.x-lastPosition.x)*(targetPosition.x-lastPosition.x)+(targetPosition.y-lastPosition.y)*(targetPosition.y-lastPosition.y) );
               speed = kForSpeed * pow(distance, 1 - roForSpeed);
               travelTime = distance / speed;
               nextChange = simTime() + travelTime;

            } else movementsFinished = true;  // не нашли - останавливаемся
        }
    }
}

bool LevyHotSpotsLATP::findNextHotSpot()
{
    // выбираем новую локацию
    double rn, pr=0;
    rn=(double)rand()/RAND_MAX;
    for(unsigned int i=0; i<(hsc->HSData).size(); i++) {
        if(i != currentHSindex ) pr+=(hsd->ProbabilityMatrix)[currentHSindex][i];
        if(rn <= pr) {currentHSindex=i; break; }
    }
    currentHSMin.x=((hsc->HSData)[currentHSindex]).Xmin;
    currentHSMin.y=((hsc->HSData)[currentHSindex]).Ymin;
    currentHSMax.x=((hsc->HSData)[currentHSindex]).Xmax;
    currentHSMax.y=((hsc->HSData)[currentHSindex]).Ymax;
    currentHSCenter=(currentHSMin+currentHSMax)*0.5;

    //    cout << "findNextHotSpot: changing location to" << currentHSindex << endl;
    return true;
}


void LevyHotSpotsLATP::move() {
    LineSegmentsMobilityBase::move();
}

//-------------------------- Statistic collection ---------------------------------
void LevyHotSpotsLATP::collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y) {
    inTimes.push_back(inTime);
    outTimes.push_back(outTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
    ((hsc->HSData)[currentHSindex]).generatedSumTime += (outTime - inTime).dbl();
    ((hsc->HSData)[currentHSindex]).generatedWaypointNum++;

    Waypoint h(x, y, inTime.dbl(), outTime.dbl(), wpFileName);
    ((hsc->HSData)[currentHSindex]).waypoints.push_back(h);
}

void LevyHotSpotsLATP::saveStatistics() {
    char *outDir = "outTrace";
    char *wpsDir = buildFullName(outDir, "waypointfiles");
    char *trsDir = buildFullName(outDir, "tracefiles");
    char *hotSpotFilesDir = buildFullName(outDir, "hotspotfiles");
    char *locations = buildFullName(outDir, "locations.loc");


    if (NodeID == 0 ) {//чтобы записывал только один узел
        //--- Create output directories ---
        if (CreateDirectory(outDir, NULL)) cout << "create output directory: " << outDir << endl;
        else cout << "error create output directory: " << outDir << endl;

        if (CreateDirectory(wpsDir, NULL)) cout << "create output directory: " << wpsDir << endl;
        else cout << "error create output directory: " << wpsDir << endl;

        if (CreateDirectory(trsDir, NULL)) cout << "create output directory: " << trsDir << endl;
        else cout << "error create output directory: " << trsDir << endl;

        if (CreateDirectory(hotSpotFilesDir, NULL)) cout << "create output directory: " << hotSpotFilesDir << endl;
        else cout << "error create output directory: " << hotSpotFilesDir << endl;

        // --- Write HotSpots ---
        for (unsigned int i = 0; i < (hsc->HSData).size(); i++) {
            char* fullNameHS = buildFullName(hotSpotFilesDir, (hsc->HSData)[i].hotSpotName);
            ofstream* hsFile = new ofstream(fullNameHS);
            (*hsFile) << ((hsc->HSData)[i]).Xmin << "\t" << ((hsc->HSData)[i]).Xmax << endl;
            (*hsFile) << ((hsc->HSData)[i]).Ymin << "\t" << ((hsc->HSData)[i]).Ymax << endl;
            (*hsFile) << ((hsc->HSData)[i]).generatedSumTime << "\t"<< ((hsc->HSData)[i]).generatedWaypointNum << endl;

            for(unsigned int j = 0; j < ((hsc->HSData)[i]).waypoints.size(); j++)
                (*hsFile) << (((hsc->HSData)[i]).waypoints[j]).X  << "\t" << (((hsc->HSData)[i]).waypoints[j]).Y  << "\t"
                          << (((hsc->HSData)[i]).waypoints[j]).Tb << "\t" << (((hsc->HSData)[i]).waypoints[j]).Te << "\t"
                          << (((hsc->HSData)[i]).waypoints[j]).traceName << endl;

            hsFile->close();
            delete hsFile;
        }

        // --- Write Locations ---
        ofstream lcfile(locations);
        for(unsigned int i = 0; i < (hsc->HSData).size(); i++) {
            lcfile << ((hsc->HSData)[i]).hotSpotName << "\t"<< ((hsc->HSData)[i]).generatedSumTime << "\t" << "\t";
            lcfile << ((hsc->HSData)[i]).generatedWaypointNum << "\t" << "\t";
            lcfile << ((hsc->HSData)[i]).Xmin << "\t"<< ((hsc->HSData)[i]).Xmax << "\t";
            lcfile << ((hsc->HSData)[i]).Ymin << "\t"<< ((hsc->HSData)[i]).Ymax << endl;
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
    cout << "------------- ERROR! -------------" << endl;
    log();
    return false;
}

void LevyHotSpotsLATP::log() {  // Отладочная функция
    cout << "-------------------------------------------------------------" << endl;
    cout << "step = " << step << ", isPause = " << isPause << endl;
    cout << "simTime() = " << simTime() << endl;
    cout << "lastPosition = " << lastPosition << endl;

    cout << "currentHSindex = " << currentHSindex << endl;
    cout << "\t currentHSMin.x = " << currentHSMin.x << ", currentHSMax.x = " << currentHSMax.x << endl;
    cout << "\t currentHSMin.y = " << currentHSMin.y << ", currentHSMax.y = " << currentHSMax.y << endl;
    cout << "\t currentHSCenter.x = " << currentHSCenter.x << ", currentHSCenter.y = " << currentHSCenter.y << endl;
    ((hsc->HSData)[currentHSindex]).print();

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
