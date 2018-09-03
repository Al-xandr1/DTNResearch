#include <LevyTester.h>

Define_Module(LevyTester);

LevyTester::LevyTester() {
}

LevyTester::~LevyTester() {
}

void LevyTester::initialize() {
    cout << "LevyTester::initialize() is started!" << endl;

    Coord constraintAreaMin, constraintAreaMax;
    constraintAreaMin.x = par("constraintAreaMinX");
    constraintAreaMin.y = par("constraintAreaMinY");
    constraintAreaMin.z = par("constraintAreaMinZ");
    constraintAreaMax.x = par("constraintAreaMaxX");
    constraintAreaMax.y = par("constraintAreaMaxY");
    constraintAreaMax.z = par("constraintAreaMaxZ");

    Movement* movement = new Movement(this, (constraintAreaMax - constraintAreaMin).length());

    long selectionVolume = par("selectionVolume").longValue();
    cout << "LevyTester::initialize: selectionVolume = " << selectionVolume << endl;



    cout << "LevyTester::initialize() is finished!" << endl;
}

void LevyTester::handleMessage(cMessage *msg) {
    cout << "LevyTester::handleMessage() is started!" << endl;
    cout << "LevyTester::handleMessage() is finished!" << endl;
}

void LevyTester::finish() {
    cout << "LevyTester::finish() is started!" << endl;
    cout << "LevyTester::finish() is finished!" << endl;
}
