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

        speed = kForSpeed * pow(distance, 1 - roForSpeed);
        travelTime = checkValue(distance / speed, (MAXTIME - simTime()).dbl());
        return true;
    }

    return false;
}

void Movement::setWaitTime(const simtime_t waitTime) {
    ASSERT(waitTime > 0 && waitTime <= (MAXTIME - simTime()));
    this->waitTime = waitTime;
}

void Movement::setDistance(const double dist) {
    ASSERT(dist > 0);
    distance = dist;
    speed = kForSpeed * pow(distance, 1 - roForSpeed);

    const simtime_t maxTravelTime = MAXTIME - simTime();
    travelTime = checkValue(distance / speed, maxTravelTime.dbl());
    ASSERT(travelTime > 0 && travelTime <= maxTravelTime);
    //todo сбросить остальные параметры?
}

const Coord Movement::getDeltaVector() {
    return Coord(distance * cos(angle), distance * sin(angle), 0);
}

void Movement::log() {
    cout << "Movement->kForSpeed = " << kForSpeed << endl;
    cout << "Movement->roForSpeed = " << roForSpeed << endl;
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
