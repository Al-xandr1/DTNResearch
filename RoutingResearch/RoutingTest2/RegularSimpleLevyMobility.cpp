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

void RegularSimpleLevyMobility::setTargetPosition() {
    SimpleLevyMobility::setTargetPosition();

    if (movementsFinished) {
        // ������� ������ � ��������� � ������������� - ����� �������������, �� �� ���������
        endRoute();
        movementsFinished = false;
        nextChange = MAXTIME;
    }
}

void RegularSimpleLevyMobility::nodeTurnedOff() {
    emitMobilityStateChangedSignal();
}

void RegularSimpleLevyMobility::endRoute() {
    cMessage* msg = new cMessage("END_ROUTE", END_ROUTE);
    take(msg);
    sendDirect(msg, getParentModule()->gate("in"));
}

void RegularSimpleLevyMobility::makeNewRoot() {
    // this method should be empty
}
