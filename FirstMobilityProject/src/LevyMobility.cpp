#include "LevyMobility.h"

Define_Module(LevyMobility);

LevyMobility::LevyMobility() {
    nextMoveIsWait = false;
    jump = NULL;
    pause = NULL;
    kForSpeed = 1;
    roForSpeed = 0;
    movementsFinished = false;

    specification = NULL;
    hsAlgorithm = NULL;
}

void LevyMobility::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == 0) {
        stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0;
    }

    if (hasPar("ciJ") && hasPar("aliJ") && hasPar("aciJ")
        && hasPar("ciP") && hasPar("aliP") && hasPar("aciP")) {

        double ciJ  = par("ciJ").doubleValue();
        double aliJ = par("aliJ").doubleValue();
        double aciJ = par("aciJ").doubleValue();

        double ciP  = par("ciP").doubleValue();
        double aliP = par("aliP").doubleValue();
        double aciP = par("aciP").doubleValue();

        if (jump == NULL || pause == NULL) {
            jump = new LeviJump(ciJ, aliJ, aciJ);
            pause = new LeviPause(ciP, aliP, aciP);
        }
    } else {
        cout << "It is necessary to specify ALL parameters for length and pause Levy distribution";
        exit(-112);
    }

    initializeSpecification();
}

void LevyMobility::initializeSpecification() {
    if (!specification) {
        if (hasPar("specification")) specification = par("specification").stringValue();
        else exit(-113);

        if (strcmp(specification, SIMPLE_LEVY) == 0) {
            hsAlgorithm = NULL;
        } else if (strcmp(specification, LEVY_HOTSPOTS_RANDOM) == 0) {
            hsAlgorithm = new HotSpotsAlgorithm(this, par("powA").doubleValue(), false, false, false);
        } else if (strcmp(specification, LEVY_HOTSPOTS_LATP_CENTER_LOGIC) == 0) {
            hsAlgorithm = new HotSpotsAlgorithm(this, par("powA").doubleValue(), true, true, false);
        } else if (strcmp(specification, LEVY_HOTSPOTS_LATP) == 0) {
            hsAlgorithm = new HotSpotsAlgorithm(this, par("powA").doubleValue(), true, false, false);
        } else if (strcmp(specification, LEVY_HOTSPOTS_LATP_PATH_COUNTS) == 0) {
            hsAlgorithm = new HotSpotsAlgorithm(this, par("powA").doubleValue(), true, false, true);
        } else {
            cout << "Unknown type of specification";
            exit(-114);
        }
    }
}

void LevyMobility::setInitialPosition() {
    MobilityBase::setInitialPosition();

    if (hsAlgorithm) lastPosition = hsAlgorithm->getInitialPosition();
}

void LevyMobility::finish() {
    saveStatistics();
}

void LevyMobility::setTargetPosition() {
    if (!movementsFinished) {               //todo остановку делать подругому
        if (nextMoveIsWait) {
            simtime_t waitTime = (simtime_t) pause->get_Levi_rv();
            nextChange = simTime() + waitTime;
        } else {
            collectStatistics(simTime(), lastPosition.x, lastPosition.y);

            generateNextPosition(targetPosition, nextChange);
        }
        nextMoveIsWait = !nextMoveIsWait;
    }
}

// Генерирует следующую позицию в зависимости от того, включено использование горячих точек или нет
void LevyMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    // генерируем прыжок Леви как обычно
    const double angle = uniform(0, 2 * PI);
    const double distance = jump->get_Levi_rv();
    const double speed = kForSpeed * pow(distance, 1 - roForSpeed);
    Coord delta(distance * cos(angle), distance * sin(angle), 0);
    simtime_t travelTime = distance / speed;

    targetPosition = lastPosition + delta;
    nextChange = simTime() + travelTime;

    if (hsAlgorithm)
        // Если правка координаты прошла неудачно, то нужно заканчивать перемещения
        movementsFinished = !hsAlgorithm->fixTargetPosition(targetPosition, delta, distance);
}

void LevyMobility::move() {
    LineSegmentsMobilityBase::move();
}

//-------------------------- Statistic collection ---------------------------------
void LevyMobility::collectStatistics(simtime_t appearenceTime, double x, double y) {
    times.push_back(appearenceTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
}

void LevyMobility::saveStatistics() {
    char outFileName[256];
    char *fileName = createFileName(outFileName, 0,
            par("traceFileName").stringValue(), (int) ((par("fileSuffix"))));

    ofstream* file = new ofstream(fileName);
    for (int i = 0; i < times.size(); i++) {
        simtime_t time = times[i];
        double x = xCoordinates[i];
        double y = yCoordinates[i];
        (*file) << time << "\t" << x << "\t" << y << endl;
    }

    file->close();
    delete file;
}
