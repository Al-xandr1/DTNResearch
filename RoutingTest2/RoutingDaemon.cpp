#include "RoutingDaemon.h"

Define_Module(RoutingDaemon);

int RoutingDaemon::numHosts;
double RoutingDaemon::interconnectionRadius;
bool** RoutingDaemon::connections = NULL;
simtime_t** RoutingDaemon::connectStart = NULL;
simtime_t** RoutingDaemon::connectLost = NULL;
vector<simtime_t**>* RoutingDaemon::connectivityPerDay = NULL;
vector<Request*>* RoutingDaemon::requests = NULL;
RoutingDaemon* RoutingDaemon::instance = NULL;


void RoutingDaemon::initialize() {
    if (instance == NULL) {
        instance = this;
    } else {
        cout << "Duplicate initialization exception!" << endl;
        exit(-654);
    }

    routingHeuristics = new vector<RoutingHeuristic*>();
    routingHeuristics->push_back(new OneHopHeuristic(this));
    routingHeuristics->push_back(new TwoHopsHeuristic(this));
    routingHeuristics->push_back(new LETHeuristic(this));
//    routingHeuristics->push_back(new MoreFrequentVisibleHeuristic(this));

    connectivityPerDay = new vector<simtime_t**>();
    requests = new vector<Request*>();
    interconnectionRadius = getParentModule()->par("interconnectionRadius");
    numHosts = getParentModule()->par("numHosts");

    collectorGate = getParentModule()->getSubmodule("collector")->gate("in");
    dayDuration = getParentModule()->par("dayDuration").doubleValue();
    countOfDays = getParentModule()->par("countOfDays").doubleValue();

    RD_Listener* listener = new RD_Listener();
    getParentModule()->subscribe(mobilityStateChangedSignal, listener);

    scheduleAt(simTime(), new cMessage("Start of the Day", DAY_START));
}

void RoutingDaemon::handleMessage(cMessage *msg) {
    if (msg->getKind() == REQUEST_FOR_ROUTING) {
        // Запрос на муршрутизацию. Обрабатываем, если можем или ставим в очередь
        Request* request = check_and_cast<Request*>(msg);
        //cout << "RoutingDeamon: received request from node: " << request->getNodeIdSrc() << " to node: " << request->getNodeIdTrg() << endl;

        if (!processIfCan(request)) requests->push_back(request);


    } else if (msg->isSelfMessage() && msg->getKind() == DAY_START) {
        // Сообщение о начале нового дня. Копия сообщени рассылается всем узлам сети
        processNewDay();

        for (int i=0; i < RoutingDaemon::numHosts; i++)
            sendDirect(new cMessage("Start of the Day", DAY_START), getParentModule()->getSubmodule("host", i)->gate("in"));

        scheduleAt(simTime() + (simtime_t) dayDuration, msg);


    } else {
        cout << "RoutingDaemon::handleMessage: msg->getKind() = " << msg->getKind() << endl;
        cout << "Sender: " << msg->getSenderModule()->getFullName() << endl;
        exit(-444);
    }
}

void RoutingDaemon::processNewDay() {
    // "Обрезаем" длительность текущих сединений перед окончанием дня
    for (int i = 0; i < RoutingDaemon::numHosts; i++)
        for (int j = 0; j < i; j++)
            if (RoutingDaemon::connections[i][j]) accumulateConnectivity(i, j);

    //todo truncate connection time matrix

    currentDay++;
    startTimeOfCurrentDay = simTime();
    finishTimeOfCurrentDay = startTimeOfCurrentDay + dayDuration;
    //cout << "Day " << currentDay << " started at " << startTimeOfCurrentDay << endl;

    // Создаём новую матрицу длительности соединений
    simtime_t** dayConnectivity = new simtime_t*[RoutingDaemon::numHosts];
    for (int i = 0; i < RoutingDaemon::numHosts; i++) {
        dayConnectivity[i] = new simtime_t[i+1];
        // соединение самого себя с собой в течение дня = 1 день
        dayConnectivity[i][i] = dayDuration;
        for (int j=0; j<i; j++) dayConnectivity[i][j] = 0;
    }

    RoutingDaemon::connectivityPerDay->push_back(dayConnectivity);

    if (RoutingDaemon::connectivityPerDay->size() > countOfDays) {
        simtime_t** oldest = RoutingDaemon::connectivityPerDay->front();
        RoutingDaemon::connectivityPerDay->erase(RoutingDaemon::connectivityPerDay->begin());
        for(int i = 1; i < RoutingDaemon::numHosts; i++) {
            //for (int j = 0; j <= i; j++) cout << oldest[i][j] << "  ";
            //cout << endl;
            delete[] oldest[i];
        }
        //cout << endl << endl;
        delete[] oldest;
    }
}

void RoutingDaemon::connectionsChanged() {
    for(vector<Request*>::iterator it = requests->begin(); it != requests->end(); ) {
        if (processIfCan((*it))) it = requests->erase(it);
        else ++it;
    }
}

bool RoutingDaemon::processIfCan(Request* request) {
    for(int i = 0; i < routingHeuristics->size(); i++) {
        int nodeForRouting = -1;

        if ((*routingHeuristics)[i]->canProcess(request, nodeForRouting)) {
            // for debug
            //if (nodeForRouting < 0 || nodeForRouting >= getNumHosts() || nodeForRouting == request->getSourceId()) exit(-443);

            // Посылаем отклик на обработку запроса источнику запроса.
            // nodeForRoutePacket - узел, которому бует передан пакет на узле источнике
            Response* response = new Response(nodeForRouting, request);
            take(response);
            sendDirect(response, getParentModule()->getSubmodule("host", request->getSourceId())->gate("in"));

            return true;
        }
    }

    return false;
}

void RoutingDaemon::calculateICT(int nodeId1, int nodeId2) {
    //todo учитывать или нет первое появление соединения
    simtime_t oldStart = getStartConnectionTime(nodeId1, nodeId2);
    simtime_t oldLost = getLostConnectionTime(nodeId1, nodeId2);
    simtime_t newStart = simTime();

    if (oldStart == 0 && oldLost == 0) return; //первое соединение игнорируем

    simtime_t ict = newStart - oldLost;

    ICTMessage* ictMsg = new ICTMessage(nodeId1, nodeId2, ict);
    take(ictMsg);
    sendDirect(ictMsg, collectorGate);
}

void RoutingDaemon::accumulateConnectivity(int nodeId1, int nodeId2) {
    if (RoutingDaemon::connectivityPerDay->size() == 0) return;

    simtime_t** current = (*RoutingDaemon::connectivityPerDay)[RoutingDaemon::connectivityPerDay->size()-1];
    simtime_t end = min(getLostConnectionTime(nodeId1, nodeId2), finishTimeOfCurrentDay);
    simtime_t start = max(getStartConnectionTime(nodeId1,nodeId2), startTimeOfCurrentDay);

    //for debug
    if (start >= end) exit(-543);

    if (nodeId1 > nodeId2) current[nodeId1][nodeId2] += (end - start);
    else current[nodeId2][nodeId1] += (end - start);;
}

simtime_t RoutingDaemon::computeTotalConnectivity(int nodeId1, int nodeId2) {
    simtime_t totalConnectivity = 0;
    for (unsigned int day = 0; day < RoutingDaemon::connectivityPerDay->size(); day++)
        totalConnectivity += getConnectivity(day, nodeId1, nodeId2);
    return totalConnectivity;
}

simtime_t RoutingDaemon::getConnectivity(int index, int nodeId1, int nodeId2) {
    if (nodeId1 > nodeId2) return (*RoutingDaemon::connectivityPerDay)[index][nodeId1][nodeId2];
    else return (*RoutingDaemon::connectivityPerDay)[index][nodeId2][nodeId1];
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



//-------------------------------------- for debug ---------------------------------------------------
void RoutingDaemon::log() {
    cout << "NodeIds:" << endl;
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        MobileHost* host = check_and_cast<MobileHost*>(getParentModule()->getSubmodule("host", i));
        int nodeId = host->getNodeId();
        cout << "nodeId = " << nodeId << "  ";
    }
    cout << endl << endl;
}
