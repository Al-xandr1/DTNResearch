#include "RegularSimpleLevyMobility.h"

Define_Module(RegularSimpleLevyMobility);

void RegularSimpleLevyMobility::initialize(int stage) {
    SimpleLevyMobility::initialize(stage);
}

void RegularSimpleLevyMobility::handleMessage(cMessage *message) {
    if (message->isSelfMessage())
        MobilityBase::handleMessage(message);
    else
        switch (message->getKind()) {
            // ������������ ��� "�����" ��� �����������, ����� ����� ������ ������
            case MOBILITY_START:{
                nextChange = simTime();
                MovingMobilityBase::scheduleUpdate();
                emitMobilityStateChangedSignal();
                myDelete(message);
                break;
            }
            default:
                ASSERT(false); //unreacheble statement
        }
}

void RegularSimpleLevyMobility::setTargetPosition() {
    SimpleLevyMobility::setTargetPosition();

    if (movementsFinished) {
        // ������� ������ � ��������� � ������������� - ����� �������������, �� �� ���������
        movementsFinished = false;
        nextChange = MAXTIME;
    }

}

void RegularSimpleLevyMobility::makeNewRoot() {
//    RealMobility::makeNewRoot();
//
//    unsigned int day = RoutingDaemon::instance->getCurrentDay();
//    ASSERT(day >= 1);
//
//    timeOffset = (day-1) * RoutingDaemon::instance->getDayDuration();
//    cout << "Root made! day = " << day << ", NodeID = " << NodeID << ", simetime = " << simTime() << endl;
}
