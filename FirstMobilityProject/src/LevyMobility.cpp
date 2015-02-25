#include "LevyMobility.h"
#include "HotSpot.h"
#include "HotSpot.h"

#define DEF_HS_DIR "./hotspotfiles"         //���������� �� ��������� ��� "������� �����"

Define_Module(LevyMobility);

LevyMobility::LevyMobility() {
    nextMoveIsWait = false;

    double ciJ = 10;        // 10
    double aliJ = 1.0;      // 0.5, 1.0, 1.5
    double aciJ = 0.001;

    double ciP = 1;         // 1
    double aliP = 0.5;      // 0.5
    double aciP = 0.001;

    jump = new LeviJump(ciJ, aliJ, aciJ);
    pause = new LeviPause(ciP, aliP, aciP);
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

    initializeHotSpots();
}

// �������������� ������� ����� ���� �� ������������� ��������
void LevyMobility::initializeHotSpots() {
    useHotSpots = par("useHotSpots").boolValue();
    if (useHotSpots)
    {
        if (hotSpots) {delete hotSpots; hotSpots = NULL;}
        HotSpotReader hsReader;
        hotSpots = hsReader.readAllHotSpots(DEF_HS_DIR);
        checkHotSpotsBound();
    }
}

void LevyMobility::setInitialPosition() {
    MobilityBase::setInitialPosition();

    if (useHotSpots) {
        currentHotSpot = getRandomHotSpot(NULL);
        lastPosition = getRandomPositionInsideHS(currentHotSpot);
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

#define DELTA_ROTATE 0.1 //����� �������, �� ������� ����� ������������ ����� (� ������)

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

    if (useHotSpots) {
        // ����������� �� ����� ����� ������� ������� �����
        if (!currentHotSpot->isPointBelong(targetPosition.x, targetPosition.y)) {
            // �������� ������������ ���������� �� ������� ����� �� ���� ������ ��������������
            double l1 = getLength(lastPosition.x, lastPosition.y, currentHotSpot->Xmin, currentHotSpot->Ymin);
            double l2 = getLength(lastPosition.x, lastPosition.y, currentHotSpot->Xmin, currentHotSpot->Ymax);
            double l3 = getLength(lastPosition.x, lastPosition.y, currentHotSpot->Xmax, currentHotSpot->Ymax);
            double l4 = getLength(lastPosition.x, lastPosition.y, currentHotSpot->Xmax, currentHotSpot->Ymin);

            double maxLength = max(l1, max(l2, max(l3, l4)));

            // �������� ����� �������� ������
            double length = getLength(lastPosition.x, lastPosition.y, targetPosition.x, targetPosition.y);

            if (length < maxLength) {// ����� ��������� ����� � ������� � �������������
                double deltaAngle = DELTA_ROTATE / distance; //���������� ������ �������� �� ���� �� ������� ����� ����

                double shiftedAngle = angle;
                while (!currentHotSpot->isPointBelong(targetPosition.x, targetPosition.y)) {
                    shiftedAngle += deltaAngle;
                    Coord rotateDelta(distance * cos(shiftedAngle), distance * sin(shiftedAngle), 0);
                    targetPosition = lastPosition + rotateDelta;
                }

            } else if (length == maxLength) {// ���������� ����� �� �� �������, �� ������� �������� ������ ������� ������������ ����� (�� �� ����� ��������)
                if (l1 == maxLength) {
                    Coord p(currentHotSpot->Xmin, currentHotSpot->Ymin, 0);
                    targetPosition = p;
                } else if (l2 == maxLength) {
                    Coord p(currentHotSpot->Xmin, currentHotSpot->Ymax, 0);
                    targetPosition = p;
                } else if (l3 == maxLength) {
                    Coord p(currentHotSpot->Xmax, currentHotSpot->Ymax, 0);
                    targetPosition = p;
                } else if (l4 == maxLength) {
                    Coord p(currentHotSpot->Xmax, currentHotSpot->Ymin, 0);
                    targetPosition = p;
                }

            } else {// TODO � ���� ������ ��� ����� ���� ����� ������� ��� �� ����������� ����� (������) ��� ������� � ����, ������ ��������� ���
                //����� ���� �������� ��������� �������
                currentHotSpot = getRandomHotSpot(currentHotSpot);
                targetPosition = getRandomPositionInsideHS(currentHotSpot);
            }
        }
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
