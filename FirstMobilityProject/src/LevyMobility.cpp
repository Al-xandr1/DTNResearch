#include "LevyMobility.h"

Define_Module(LevyMobility);

LevyMobility::LevyMobility() {
    nextMoveIsWait = false;

    double ciJ = 100;
    double aliJ = 1;
    double aciJ = 0.001;

    double ciP = 1;
    double aliP = 1;
    double aciP = 0.001;

    jump = new LeviJump(ciJ, aliJ, aciJ);
    pause = new LeviPause(ciP, aliP, aciP);
    kForSpeed = 1;
    roForSpeed = 0;


}

void LevyMobility::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == 0) {
        stationary = (par("speed").getType() == 'L'
                || par("speed").getType() == 'D') && (double) par("speed") == 0;
    }
}

void LevyMobility::finish() {
    saveStatistics();
}

void LevyMobility::setTargetPosition() {
    if (nextMoveIsWait) {
        simtime_t waitTime = (simtime_t) pause->get_Levi_rv();
        nextChange = simTime() + waitTime;
    } else {
        collectStatistics(simTime(), lastPosition.x, lastPosition.y);

        double angle = uniform(0, 2 * PI);
        double distance = jump->get_Levi_rv();
        double speed = kForSpeed * pow(distance, 1 - roForSpeed);
        Coord delta(distance * cos(angle), distance * sin(angle), 0);
        targetPosition = lastPosition + delta;
        simtime_t travelTime = distance / speed;
        nextChange = simTime() + travelTime;
    }
    nextMoveIsWait = !nextMoveIsWait;
}

void LevyMobility::move() {
    LineSegmentsMobilityBase::move();
    Coord position, speed;
    double angle;
    reflectIfOutside(position, speed, angle);
}

//-------------------------- Statistic collection ---------------------------------
void LevyMobility::collectStatistics(simtime_t appearenceTime, double x, double y) {
    times.push_back(appearenceTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
}

void LevyMobility::saveStatistics() {
    char outFileName[256];
    char *fileName = DevelopmentHelper::createFileName(outFileName, 0,
            par("traceFileName").stringValue(), (int) ((par("fileSuffix"))));

    ofstream* file = new ofstream(fileName);
    (*file) << "appearenceTime               x               y" << endl << endl;
    for (int i = 0; i < times.size(); i++) {
        simtime_t time = times[i];
        double x = xCoordinates[i];
        double y = yCoordinates[i];
        (*file) << time << "  " << x << "  " << y << endl << endl;
    }

    file->close();
}
