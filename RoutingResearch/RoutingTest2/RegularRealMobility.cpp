#include "RegularRealMobility.h"

Define_Module(RegularRealMobility);

void RegularRealMobility::initialize(int stage) {
    RealMobility::initialize(stage);
}

void RegularRealMobility::handleMessage(cMessage *message) {
    if (message->isSelfMessage())
        MobilityBase::handleMessage(message);
    else
        switch (message->getKind()) {
            // ������������ ��� "�����" ��� �����������, ����� ����� ������ ������
            case MOBILITY_START:{
                ASSERT(RoutingDaemon::instance->getCurrentDay() >= 1);
                // ��� ������� ��� ������� �������� ��� ������������� �����������
                if (RoutingDaemon::instance->getCurrentDay() > 1) {
                    makeNewRoot();
                }

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

void RegularRealMobility::setTargetPosition() {
    RealMobility::setTargetPosition();

    if (movementsFinished) {
        // ������� ������ � ��������� � ������������� - ����� �������������, �� �� ���������
        movementsFinished = false;
        nextChange = MAXTIME;
    }
}

void RegularRealMobility::nodeTurnedOff() {
    emitMobilityStateChangedSignal();
}

void RegularRealMobility::makeNewRoot() {
    RealMobility::makeNewRoot();

    unsigned int day = RoutingDaemon::instance->getCurrentDay();
    ASSERT(day >= 1);

    timeOffset = (day-1) * RoutingDaemon::instance->getDayDuration();
    cout << "Root made! day = " << day << ", NodeID = " << NodeID << ", simTime = " << simTime() << ", timeOffset = " << timeOffset << endl;
}
