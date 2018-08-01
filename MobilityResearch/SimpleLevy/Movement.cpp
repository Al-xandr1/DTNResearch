#include <Movement.h>

bool Movement::genPause(const char* where) {
    if (levyPause) {
        const double maxPermittedPause = (MAXTIME - simTime()).dbl();
        const double pauseTime = levyPause->get_Levi_rv(maxPermittedPause);
        setWaitTime(checkValue(pauseTime, maxPermittedPause, where));
        return true;
    }

    return false;
}

bool Movement::genFlight(const char* where) {
    if (leviJump) {
        // генерируем прыжок Леви как обычно
        angle = uniform(0, 2 * PI);

        do {
            const double dist = leviJump->get_Levi_rv(maxPermittedDistance);
            distance = checkValue(dist, maxPermittedDistance, where);
        } while (getDeltaVector() == Coord::ZERO);

        computeSpeed();
        return true;
    }

    return false;
}

void Movement::setWaitTime(const simtime_t waitTime) {
    ASSERT(waitTime > 0 && waitTime <= (MAXTIME - simTime()));
    this->waitTime = waitTime;
}

void Movement::setDistance(const double dist, const char* where) {
    distance = checkValue(dist, maxPermittedDistance, where);
    computeSpeed();
    //todo сбросить остальные параметры?
}

const Coord Movement::getDeltaVector() {
    return Coord(distance * cos(angle), distance * sin(angle), 0);
}

void Movement::computeSpeed() {
    if (distance < distanceThreshold) {
        speed = kForSpeed_1 * pow(distance, 1 - roForSpeed_1);
    } else {
        speed = kForSpeed_2 * pow(distance, 1 - roForSpeed_2);
    }
    travelTime = checkValue(distance / speed, (MAXTIME - simTime()).dbl());
}

void Movement::log() {
    cout << "Movement->kForSpeed_1 = " << kForSpeed_1 << endl;
    cout << "Movement->roForSpeed_1 = " << roForSpeed_1 << endl;
    cout << "Movement->maxPermittedDistance = " << maxPermittedDistance << endl;
    cout << "Movement->waitTime = " << waitTime << endl;
    cout << "Movement->distance = " << distance << endl;
    cout << "Movement->angle = " << angle << endl;
    cout << "Movement->speed = " << speed << endl;
    cout << "Movement->travelTime = " << travelTime << endl;
    cout << "Movement->getDeltaVector() = " << getDeltaVector() << endl;
    cout << "Movement->getDeltaVector().length() = " << getDeltaVector().length() << ", EPSILON = " << EPSILON << endl;
    cout << "Movement->(getDeltaVector() != Coord::ZERO) = " << (getDeltaVector() != Coord::ZERO) << endl;
}
