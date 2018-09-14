#include <Movement.h>

bool Movement::genPause(string where) {
    if (levyPause) {
        const double maxPermittedPause = (MAXTIME - simTime()).dbl();
        const double pauseTime = levyPause->get_Levi_rv(maxPermittedPause);
        setWaitTime(checkValue(pauseTime, maxPermittedPause, string("Movement::genPause:") + where));
        return true;
    }

    return false;
}

bool Movement::genFlight(string where) {
    if (leviJump) {
        // генерируем прыжок Леви как обычно
        angle = uniform(0, 2 * PI);

        do {
            const double dist = leviJump->get_Levi_rv(maxPermittedDistance);
            distance = checkValue(dist, maxPermittedDistance, string("Movement::genFlight:") + where);
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

void Movement::setDistance(const double dist, string where) {
    distance = checkValue(dist, maxPermittedDistance, string("Movement::setDistance:") + where);
    computeSpeed();
}

const Coord Movement::getDeltaVector() {
    return Coord(distance * cos(angle), distance * sin(angle), 0);
}

void Movement::computeSpeed() {
    if (distance < distanceThreshold) {
        travelTime = kForSpeed_1 * pow(distance, 1 - roForSpeed_1);

//        travelTime = constV;

        // THIRD_FIXED
//        travelTime = speed + deltaV;
    } else {
        // FIRST
        travelTime = kForSpeed_2 * pow(distance, 1 - roForSpeed_2);

        // SECOND
//        travelTime = kForSpeed_2 * pow(distance, 1 - roForSpeed_2) +
//                (kForSpeed_1 * pow(distanceThreshold, 1 - roForSpeed_1) - kForSpeed_2 * pow(distanceThreshold, 1 - roForSpeed_2));

        // THIRD
//        travelTime = -1 * kForSpeed_2 * pow(distance, 1 - roForSpeed_2) +
//                     2 * kForSpeed_2 * pow(distanceThreshold, 1 - roForSpeed_2);

//        travelTime = constV;
        // THIRD_FIXED
//        travelTime = speed - deltaV;

        // FORTH
//        travelTime = -1 * kForSpeed_2 * pow(distance, 1 - roForSpeed_2) +
//                     (kForSpeed_1 * pow(distanceThreshold, 1 - roForSpeed_1) + kForSpeed_2 * pow(distanceThreshold, 1 - roForSpeed_2));
    }
//    travelTime = min(constV, speed);

    speed = checkValue(distance / travelTime, maxPermittedDistance / 0.0000000001, string("Movement::computeSpeed"));
}

void Movement::log() {
    cout << "Movement::log-> ====================================================" << endl;
    cout << "Movement->kForSpeed_1 = " << kForSpeed_1 << endl;
    cout << "Movement->roForSpeed_1 = " << roForSpeed_1 << endl;
    cout << "Movement->distanceThreshold = " << distanceThreshold << endl;
    cout << "Movement->kForSpeed_2 = " << kForSpeed_2 << endl;
    cout << "Movement->roForSpeed_2 = " << roForSpeed_2 << endl;
    cout << "Movement->maxPermittedDistance = " << maxPermittedDistance << endl;
    cout << "Movement->waitTime = " << waitTime << endl;
    cout << "Movement->distance = " << distance << endl;
    cout << "Movement->angle = " << angle << endl;
    cout << "Movement->speed = " << speed << endl;
    cout << "Movement->travelTime = " << travelTime << endl;
    cout << "Movement->getDeltaVector() = " << getDeltaVector() << endl;
    cout << "Movement->getDeltaVector().length() = " << getDeltaVector().length() << ", EPSILON = " << EPSILON << endl;
    cout << "Movement->(getDeltaVector() != Coord::ZERO) = " << (getDeltaVector() != Coord::ZERO) << endl;
    cout << endl << endl;
}
