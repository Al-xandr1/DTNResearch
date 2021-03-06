//#include "SimpleLevyMobility.h"
//
//#define TRACE_TYPE ".txt"
//
//Define_Module(RealMobility);
//
//RealMobility::RealMobility() {
//    nextMoveIsWait = false;
//    jump = NULL;
//    pause = NULL;
//    kForSpeed = 1;
//    roForSpeed = 0;
//    movementsFinished = false;
//
//    specification = NULL;
//    hsAlgorithm = NULL;
//}
//
//void RealMobility::initialize(int stage) {
//    LineSegmentsMobilityBase::initialize(stage);
//
//    if (stage == 0) {
//        stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0;
//    }
//
//    if (hasPar("ciJ") && hasPar("aliJ") && hasPar("aciJ")
//        && hasPar("ciP") && hasPar("aliP") && hasPar("aciP")) {
//
//        double ciJ  = par("ciJ").doubleValue();
//        double aliJ = par("aliJ").doubleValue();
//        double aciJ = par("aciJ").doubleValue();
//
//        double ciP  = par("ciP").doubleValue();
//        double aliP = par("aliP").doubleValue();
//        double aciP = par("aciP").doubleValue();
//
//        if (jump == NULL || pause == NULL) {
//            jump = new LeviJump(ciJ, aliJ, aciJ);
//            pause = new LeviPause(ciP, aliP, aciP);
//        }
//    } else {
//        cout << "It is necessary to specify ALL parameters for length and pause Levy distribution";
//        exit(-112);
//    }
//
//    initializeSpecification();
//}
//
//void RealMobility::initializeSpecification() {
//    if (!specification) {
//        if (hasPar("specification")) specification = par("specification").stringValue();
//        else exit(-113);
//
//        if (strcmp(specification, SIMPLE_LEVY) == 0) {
//            hsAlgorithm = NULL;
//
//        } else if (strcmp(specification, LEVY_HOTSPOTS_RANDOM) == 0) {
//            hsAlgorithm = new HotSpotsAlgorithm(this, par("powA").doubleValue(), false, false, false, false);
//
//        } else if (strcmp(specification, LEVY_HOTSPOTS_LATP_CENTER_LOGIC) == 0) {
//            hsAlgorithm = new HotSpotsAlgorithm(this, par("powA").doubleValue(), true, true, false, false);
//
//        } else if (strcmp(specification, LEVY_HOTSPOTS_LATP) == 0) {
//            hsAlgorithm = new HotSpotsAlgorithm(this, par("powA").doubleValue(), true, false, false, false);
//
//        } else if (strcmp(specification, LEVY_HOTSPOTS_LATP_PATH_COUNTS) == 0) {
//            hsAlgorithm = new HotSpotsAlgorithm(this, par("powA").doubleValue(), true, false, true, false);
//
////        } else if (strcmp(specification, LEVY_HOTSPOTS_WAYPOINTS_LATP_PATH_COUNTS) == 0) {
////            hsAlgorithm = new HotSpotsAlgorithm(this, par("powA").doubleValue(), true, false, true, true);
//
//        } else {
//            cout << "Unknown type of specification";
//            exit(-114);
//        }
//    }
//}
//
//void RealMobility::setInitialPosition() {
//    MobilityBase::setInitialPosition();
//
//    if (hsAlgorithm) lastPosition = hsAlgorithm->getInitialPosition();
//}
//
//void RealMobility::finish() {
//    saveStatistics();
//}
//
//void RealMobility::setTargetPosition() {
//    if (!movementsFinished) {
//        if (nextMoveIsWait) {
//            simtime_t waitTime = (simtime_t) pause->get_Levi_rv();
//            nextChange = simTime() + waitTime;
//        } else {
//            collectStatistics(simTime(), lastPosition.x, lastPosition.y);
//
//            generateNextPosition(targetPosition, nextChange);
//        }
//        nextMoveIsWait = !nextMoveIsWait;
//    } else {
//        // ������������ � ������������� - ���������� ��������� �����������
//        nextChange = simTime() + 100000;
//    }
//}
//
//// ���������� ��������� ������� � ����������� �� ����, �������� ������������� ������� ����� ��� ���
//void RealMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
//    // ���������� ������ ���� ��� ������
//    const double angle = uniform(0, 2 * PI);
//    const double distance = jump->get_Levi_rv();
//    const double speed = kForSpeed * pow(distance, 1 - roForSpeed);
//    Coord delta(distance * cos(angle), distance * sin(angle), 0);
//    simtime_t travelTime = distance / speed;
//
//    targetPosition = lastPosition + delta;
//    nextChange = simTime() + travelTime;
//
//    if (hsAlgorithm)
//        // ���� ������ ���������� ������ ��������, �� ����� ����������� �����������
//        movementsFinished = !hsAlgorithm->fixTargetPosition(targetPosition, delta, distance);
//}
//
//void RealMobility::move() {
//    LineSegmentsMobilityBase::move();
//}
//
////-------------------------- Statistic collection ---------------------------------
//void RealMobility::collectStatistics(simtime_t appearenceTime, double x, double y) {
//    times.push_back(appearenceTime);
//    xCoordinates.push_back(x);
//    yCoordinates.push_back(y);
//}
//
//void RealMobility::saveStatistics() {
//    char outFileName[256];
//    char *fileName = createFileName(outFileName, 0, par("traceFileName").stringValue(),
//            (int) ((par("fileSuffix"))), TRACE_TYPE);
//
//    ofstream* file = new ofstream(fileName);
//    for (int i = 0; i < times.size(); i++) {
//        simtime_t time = times[i];
//        double x = xCoordinates[i];
//        double y = yCoordinates[i];
//        (*file) << time << "\t" << x << "\t" << y << endl;
//    }
//
//    file->close();
//    delete file;
//}
