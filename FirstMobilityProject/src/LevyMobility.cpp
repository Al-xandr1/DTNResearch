#include "LevyMobility.h"
#include "HotSpot.h"
#include "HotSpot.h"

#define DEF_HS_DIR "./hotspotfiles"         //���������� �� ��������� ��� "������� �����"

Define_Module(LevyMobility);

LevyMobility::LevyMobility() {
    nextMoveIsWait = false;
    jump = NULL;
    pause = NULL;
    kForSpeed = 1;
    roForSpeed = 0;
    useHotSpots = false;
    hotSpots = NULL;
    currentHotSpot = NULL;
}

void LevyMobility::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == 0) {
        stationary = (par("speed").getType() == 'L'
                || par("speed").getType() == 'D') && (double) par("speed") == 0;
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

    initializeHotSpots();
}

// �������������� ������� ����� ���� �� ������������� ��������
void LevyMobility::initializeHotSpots() {
    useHotSpots = par("useHotSpots").boolValue();
    if (useHotSpots && hotSpots == NULL)
    {
        HotSpotReader hsReader;
        hotSpots = hsReader.readAllHotSpots(DEF_HS_DIR);
        checkHotSpotsBound();
    }
}

// ���������: ��� ��� ������� ����� �� ������� �� ����� �������
void LevyMobility::checkHotSpotsBound() {
    if (hotSpots) {
        for (int i = 0; i < hotSpots->size(); i++) {
            HotSpot hotSpot = (*hotSpots)[i];
            if (hotSpot.Xmin < constraintAreaMin.x || hotSpot.Xmax > constraintAreaMax.x
                    || hotSpot.Ymin < constraintAreaMin.y || hotSpot.Ymax > constraintAreaMax.y) {
                cout << "HotSpots has wrong bounds!"; EV << "HotSpots has wrong bounds!";
                exit(123);
            }
        }
    }
}

void LevyMobility::setInitialPosition() {
    MobilityBase::setInitialPosition();

    if (useHotSpots) {
        currentHotSpot = getRandomHotSpot(NULL);
        lastPosition = getRandomPositionInsideHS(currentHotSpot);
        if (!currentHotSpot->isPointBelong(lastPosition)){
            exit(-343);
        }
    }
}

// �������� ��������� ������� ����� �� ������ hotSpots, �������� �� ��������� excludedHotSpot
HotSpot* LevyMobility::getRandomHotSpot(HotSpot* excludedHotSpot) {
    int index = -1;
    HotSpot* hotSpot = excludedHotSpot;
    do {
        index = rint(uniform(0, hotSpots->size() - 1));
        hotSpot = &((*hotSpots)[index]);
    } while (hotSpot == excludedHotSpot);

    cout << "getRandomHotSpot: index = " << index << ", size = " << hotSpots->size() << endl;
    return hotSpot;
}

// �������� ��������� ��������� ������ �������� ������� �����
Coord LevyMobility::getRandomPositionInsideHS(HotSpot* hotSpot) {
    return Coord(uniform(currentHotSpot->Xmin, currentHotSpot->Xmax),
                 uniform(currentHotSpot->Ymin, currentHotSpot->Ymax),
                 0);
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

        generateNextPosition(targetPosition, nextChange);
    }
    nextMoveIsWait = !nextMoveIsWait;
}

// ���������� ��������� ������� � ����������� �� ����, �������� ������������� ������� ����� ��� ���
void LevyMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    // ���������� ��� ������
    const double angle = uniform(0, 2 * PI);
    const double distance = jump->get_Levi_rv();
    const double speed = kForSpeed * pow(distance, 1 - roForSpeed);
    Coord delta(distance * cos(angle), distance * sin(angle), 0);
    simtime_t travelTime = distance / speed;

    targetPosition = lastPosition + delta;
    nextChange = simTime() + travelTime;

    bool isNewHS = false;//todo remove
    bool isEqual = false;//todo remove

    if (useHotSpots) {
        Coord newTargetPosition = targetPosition;
        if (!currentHotSpot->isPointBelong(lastPosition)){
            exit(-341);
        }

        if (!currentHotSpot->isPointBelong(newTargetPosition)) {
            // ���� ����� ����� �� ����������� ������� ������� �����
            Coord farthestVertix = currentHotSpot->getFarthestVertix(lastPosition);
            // �������� ����� �������� ������
            double length = lastPosition.distance(newTargetPosition);
            // �������� ���������� �� ������� ����� �� ����� ������� ������� ��������������
            double maxLength = lastPosition.distance(farthestVertix);

            if (length < maxLength) {
                // ����� ��������� ����� ������ ������������
                Coord distanceVector = farthestVertix - lastPosition;
                double locLength = distanceVector.length();
                Coord directionVector = distanceVector / locLength;
                newTargetPosition = directionVector * length;

            } else if (length == maxLength) {
                // ����� ����� ��������� � ��������
                newTargetPosition = farthestVertix;
                isEqual = true;

            } else {
                // TODO � ���� ������ ��� ����� ���� ����� ������� ��� �� ����������� ����� (������) ��� ������� � ����, ������ ��������� ���
                //����� ���� �������� ��������� �������
                currentHotSpot = getRandomHotSpot(currentHotSpot);
                newTargetPosition = getRandomPositionInsideHS(currentHotSpot);
                isNewHS = true;
            }
        }

        if (!currentHotSpot->isPointBelong(newTargetPosition)){
            exit(-346);
        }
        targetPosition = newTargetPosition;
    }
}

void LevyMobility::move() {
    LineSegmentsMobilityBase::move();
//    Coord position, speed;
//    double angle;
//    reflectIfOutside(position, speed, angle);
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
