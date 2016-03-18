#include "RoutingDaemon.h"

Define_Module(RoutingDaemon);

int RoutingDaemon::numHosts;
double RoutingDaemon::interconnectionRadius;
bool** RoutingDaemon::connections = NULL;
simtime_t** RoutingDaemon::connectStart = NULL;
simtime_t** RoutingDaemon::connectLost = NULL;
vector<Request*>* RoutingDaemon::requests = NULL;
RoutingDaemon* RoutingDaemon::instance = NULL;


void RoutingDaemon::initialize() {
    if (instance == NULL) {
        instance = this;
    } else {
        cout << "Duplicate initialization exception!" << endl;
        exit(-654);
    }
    requests = new vector<Request*>();
    interconnectionRadius = getParentModule()->par("interconnectionRadius");
    numHosts = getParentModule()->par("numHosts");

    in = gate("in");
    collectorGate = getParentModule()->getSubmodule("collector")->gate("in");

    RD_Listener* listener = new RD_Listener();
    getParentModule()->subscribe(mobilityStateChangedSignal, listener);

    currentDay = 1;
    dayDuration = getParentModule()->par("dayDuration").doubleValue();
    scheduleAt(simTime() + (simtime_t) dayDuration, new cMessage("Start of the Day", DAY_START));
    cout << "Day " << currentDay << " started: " << endl;
}

void RoutingDaemon::handleMessage(cMessage *msg) {
    if (msg->getKind() == REQUEST_FOR_ROUTING) {
        // ������ �� �������������. ������������, ���� ����� ��� ������ � �������
        Request* request = check_and_cast<Request*>(msg);
        //cout << "RoutingDeamon: received request from node: " << request->getNodeIdSrc() << " to node: " << request->getNodeIdTrg() << endl;

        if (!processIfCan(request)) requests->push_back(request);

    } else if (msg->isSelfMessage() && msg->getKind() == DAY_START) {
        // ��������� � ������ ������ ���. ����� �������� ����������� ���� ����� ����
        currentDay++;
        cout << "Day " << currentDay << " started" << endl;

        for (int i=0; i < RoutingDaemon::numHosts; i++)
            sendDirect(new cMessage("Start of the Day", DAY_START), getParentModule()->getSubmodule("host", i)->gate("in"));

        scheduleAt(simTime() + (simtime_t) dayDuration, msg);

    } else {
        cout << "RoutingDaemon::handleMessage: msg->getKind() = " << msg->getKind() << endl;
        cout << "Sender: " << msg->getSenderModule()->getFullName() << endl;
        exit(-444);
    }
}

//todo ���� �� ��������� ������ ��������� ����������, �� ����� ����� �� ��������� � ���� �����
void RoutingDaemon::calculateICT(int i, int j, simtime_t oldStart, simtime_t oldLost, simtime_t newStart) {
    if (i <= j) exit(987); //����� �������� ���� �� ������
    if (oldStart == 0 && oldLost == 0) return; //������ ���������� ����������

    simtime_t ict = newStart - oldLost;

    ICTMessage* ictMsg = new ICTMessage(i, j, ict);
    take(ictMsg);
    sendDirect(ictMsg, collectorGate);
}

void RoutingDaemon::connectionsChanged() {
    for(vector<Request*>::iterator it = requests->begin(); it != requests->end(); ) {
        if (processIfCan((*it))) it = requests->erase(it);
        else ++it;
    }
}

bool RoutingDaemon::processIfCan(Request* request) {

    // ������ ������������� � ���� ������
    bool canProcess = isConnected(request->getNodeIdSrc(), request->getNodeIdTrg());
    if (canProcess) {
        process(request->getNodeIdTrg(), request);
        return true;
    }


    // ������ ������������� � ��� ������
    for (int neighbor=0; neighbor<RoutingDaemon::numHosts; neighbor++) {
        if (isConnected(request->getNodeIdSrc(), neighbor)) {//������������� ���� �������
            canProcess = isConnected(neighbor, request->getNodeIdTrg());
            if (canProcess) {
                process(neighbor, request);
                return true;
            }

        }
    }


    // ������ ������������� "���� ��� ����� ���� ����� �������" (�����������, ���� � ��� ������ �� ����������)
    int moreSuitableNode = request->getNodeIdSrc();
    simtime_t minTime = simTime() - getLostConnectionTime(request->getNodeIdSrc(), request->getNodeIdTrg());
    for (int neighbor=0; neighbor<RoutingDaemon::numHosts; neighbor++) {
        if (isConnected(request->getNodeIdSrc(), neighbor)) {//������������� ���� ������� � ���� � ��� �����
            simtime_t lost = getLostConnectionTime(neighbor, request->getNodeIdTrg());
            simtime_t spentTime = simTime() - lost;

            // for debug
            simtime_t start = getStartConnectionTime(neighbor, request->getNodeIdTrg());
            if (spentTime < 0) {cout << "spentTime = " << spentTime; exit(-432);}
            if ( !((lost > start) || (lost == start && lost == 0)) ) {
                cout << "getLostConnectionTime(neighbor, request->getNodeIdTrg()) = " << getLostConnectionTime(neighbor, request->getNodeIdTrg()) << endl;
                cout << "getStartConnectionTime(neighbor, request->getNodeIdTrg()) = " << getStartConnectionTime(neighbor, request->getNodeIdTrg());
                exit(-433);
            }// for debug

            if (spentTime < minTime) {
                minTime = spentTime;
                moreSuitableNode = neighbor;
            }
        }
    }
    // ����� ���������� ������� ���� ��� ����������, ����� ������������� ����������
    canProcess = (moreSuitableNode != -1) && (moreSuitableNode != request->getNodeIdSrc());
    if (canProcess) {
        process(moreSuitableNode, request);
        return true;
    }


    return false;
}

void RoutingDaemon::process(int nodeForRoutePacket, Request* request) {
    // �������� ������ �� ��������� ������� ��������� �������.
    // nodeForRoutePacket - ����, �������� ���� ������� ����� �� ���� ���������
    Response* response = new Response(nodeForRoutePacket, request);
    take(response);
    sendDirect(response, getParentModule()->getSubmodule("host", request->getNodeIdSrc())->gate("in"));
}

simtime_t RoutingDaemon::getLostConnectionTime(int nodeId1, int nodeId2) {
    if (nodeId1 > nodeId2) return RoutingDaemon::connectLost[nodeId1][nodeId2];
    else return RoutingDaemon::connectLost[nodeId2][nodeId1];
}

simtime_t RoutingDaemon::getStartConnectionTime(int nodeId1, int nodeId2) {
    if (nodeId1 > nodeId2) return RoutingDaemon::connectStart[nodeId1][nodeId2];
    else return RoutingDaemon::connectStart[nodeId2][nodeId1];
}

bool RoutingDaemon::isConnected(int nodeId1, int nodeId2) {
    if (nodeId1 > nodeId2) return RoutingDaemon::connections[nodeId1][nodeId2];
    else return RoutingDaemon::connections[nodeId2][nodeId1];
}

void RoutingDaemon::log() {
    cout << "NodeIds:" << endl;
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        MobileHost* host = check_and_cast<MobileHost*>(getParentModule()->getSubmodule("host", i));
        int nodeId = host->getNodeId();
        cout << "nodeId = " << nodeId << "  ";
    }
    cout << endl << endl;
}
