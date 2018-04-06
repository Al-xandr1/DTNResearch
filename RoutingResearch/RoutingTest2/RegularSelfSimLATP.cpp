#include <RegularSelfSimLATP.h>

Define_Module(RegularSelfSimLATP);

void RegularSelfSimLATP::initialize(int stage) {
    SelfSimLATP::initialize(stage);
}

void RegularSelfSimLATP::handleMessage(cMessage *message) {
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

void RegularSelfSimLATP::setTargetPosition() {
    SelfSimLATP::setTargetPosition();

    if (movementsFinished) {
        // ������� ������ � ��������� � ������������� - ����� �������������, �� �� ���������
        movementsFinished = false;
        nextChange = MAXTIME;
    }
}

 void RegularSelfSimLATP::makeRoot() {
     ASSERT(!currentRoot);

     if (!isRootReady) {
         unsigned int day = RoutingDaemon::instance->getCurrentDay();
         ASSERT(day >= 1);

         vector<RootsCollection *> *dailyRoot = rc->getDailyRoot();
         ASSERT(dailyRoot);

         // ��� ����� ��� ����� ���� ������ ��� ������� ��� ����.
         // �������, ��������, ��������� ������� ��������� ����������
         if (day > dailyRoot->size()) day = dailyRoot->size();
         RootsCollection *dailyRC = dailyRoot->at(day-1);

         RootNumber = NodeID % dailyRC->getRootsData()->size();
         ASSERT(RootNumber >= 0 && RootNumber < dailyRC->getRootsData()->size());

         currentRoot = new vector<HotSpotData *>();
         for (unsigned int i = 0; i < dailyRC->getRootsData()->at(RootNumber)->size(); i++) {
             currentRoot->push_back(new HotSpotData(dailyRC->getRootsData()->at(RootNumber)->at(i)));
         }
         cout << "NodeId = " << NodeID << ": "  << "Root made for day " << day << endl;
     }
     isRootReady = true;
}
