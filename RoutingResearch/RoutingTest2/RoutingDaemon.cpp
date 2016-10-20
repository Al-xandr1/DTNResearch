#include "RoutingDaemon.h"

Define_Module(RoutingDaemon);

double      RoutingDaemon::dayDuration;
int         RoutingDaemon::numHosts;
double      RoutingDaemon::interconnectionRadius;
bool**      RoutingDaemon::connections  = NULL;
simtime_t** RoutingDaemon::connectStart = NULL;
simtime_t** RoutingDaemon::connectLost  = NULL;
simtime_t** RoutingDaemon::sumOfConnectDuration=NULL;
vector<simtime_t**>* RoutingDaemon::connectivityPerDay = NULL;
vector<Request*>*    RoutingDaemon::requests = NULL;
RoutingDaemon*       RoutingDaemon::instance = NULL;


void RoutingDaemon::initialize() {
    if (instance == NULL) instance = this;
    else { cout << "Duplicate initialization exception!" << endl; exit(-654); }

    RoutingSettings* settings = new RoutingSettings();
    simtime_t LET_Threshold =
       (getParentModule()->par("LET_Threshold").doubleValue() != -1) ?
        getParentModule()->par("LET_Threshold").doubleValue() : MAXTIME;

    ASSERT(LET_Threshold >= 0);
    cout << " LET_Threshold = "  << LET_Threshold  << endl;

    settings->setLET_Threshold(LET_Threshold);

    routingHeuristics = new vector<RoutingHeuristic*>();
    routingHeuristics->push_back(new OneHopHeuristic(this, settings));
    routingHeuristics->push_back(new TwoHopsHeuristic(this, settings));
    routingHeuristics->push_back(new LETHeuristic(this, settings));
    routingHeuristics->push_back(new MoreFrequentVisibleHeuristic(this, settings));

    connectivityPerDay    = new vector<simtime_t**>();
    requests              = new vector<Request*>();
    interconnectionRadius = getParentModule()->par("interconnectionRadius");
    numHosts              = getParentModule()->par("numHosts");

    dayDuration   = getParentModule()->par("dayDuration").doubleValue();
    countOfDays   = getParentModule()->par("countOfDays").doubleValue();

    RD_Listener* listener = new RD_Listener();
    getParentModule()->subscribe(mobilityStateChangedSignal, listener);

    scheduleAt(simTime(), new cMessage("Start of the Day", DAY_START));
}


void RoutingDaemon::handleMessage(cMessage *msg) {

    switch (msg->getKind()) {

        case DAY_START: {          // —ообщение о начале нового дн€.  опи€ сообщени€ рассылаетс€ всем узлам сети
            if ( msg->isSelfMessage() ) {
                processNewDay();
                for (int i=0; i < RoutingDaemon::numHosts; i++)
                    sendDirect(new cMessage("Start of the Day", DAY_START), getParentModule()->getSubmodule("host", i)->gate("in"));
                scheduleAt(simTime() + (simtime_t) dayDuration, msg);
            }
            break;
        }

        case REQUEST_FOR_ROUTING: {// «апрос на муршрутизацию. ќбрабатываем, если можем или ставим в очередь
            Request* request = check_and_cast<Request*>(msg);
            if (!processIfCan(request)) requests->push_back(request);
            break;
        }

        default: {// неизвестное сообщение, выводим дл€ отладки
            cout << "RoutingDaemon::handleMessage: msg->getKind() = " << msg->getKind() << endl;
            cout << "Sender: " << msg->getSenderModule()->getFullName() << endl;
            exit(-444);
            break;
        }
    }
}


void RoutingDaemon::processNewDay() {

    currentDay++;
    startTimeOfCurrentDay = simTime();
    finishTimeOfCurrentDay = startTimeOfCurrentDay + dayDuration;

    if(currentDay == 1) return; // самое начало, нечего обрабатывать

    // "ќбрезаем" длительность текущих сединений перед окончанием дн€ и корректируем их матрицу
    for (int i = 0; i < RoutingDaemon::numHosts; i++)
        for (int j = 0; j < i; j++)
            if (RoutingDaemon::connections[i][j])
                RoutingDaemon::sumOfConnectDuration[i][j] += simTime() - RoutingDaemon::connectStart[i][j];

    // —оздаЄм матрицу длительности соединений дл€ хранени€ результаов дн€
    simtime_t** dayConnectivity = new simtime_t*[RoutingDaemon::numHosts];
    for (int i = 0; i < RoutingDaemon::numHosts; i++) {
        dayConnectivity[i] = new simtime_t[i+1];
        dayConnectivity[i][i] = RoutingDaemon::sumOfConnectDuration[i][i];
        for (int j=0; j<i; j++) dayConnectivity[i][j] = RoutingDaemon::sumOfConnectDuration[i][j];
    }

    // —охран€ем еЄ в контейнере
    RoutingDaemon::connectivityPerDay->push_back(dayConnectivity);

    // ≈сли в контейнере слишком много матриц, удал€ем самую старую
    if (RoutingDaemon::connectivityPerDay->size() > countOfDays) {
        simtime_t** oldest = RoutingDaemon::connectivityPerDay->front();
        RoutingDaemon::connectivityPerDay->erase(RoutingDaemon::connectivityPerDay->begin());
        for(int i = 1; i < RoutingDaemon::numHosts; i++) delete[] oldest[i];
        delete[] oldest;
    }

    // ѕриводим в начальное состо€ние все матрицы
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        RoutingDaemon::connections[i][i]  = true;
        RoutingDaemon::connectStart[i][i] = 0;
        RoutingDaemon::connectLost[i][i]  = 0;
        RoutingDaemon::sumOfConnectDuration[i][i] = RoutingDaemon::dayDuration;
        for (int j=0; j<i; j++) {
            RoutingDaemon::connections[i][j]  = false;
            RoutingDaemon::connectStart[i][j] = 0;
            RoutingDaemon::connectLost[i][j]  = 0;
            RoutingDaemon::sumOfConnectDuration[i][j] = 0;
        }
    }
}

void RoutingDaemon::connectionsChanged() {
    for(vector<Request*>::iterator it = requests->begin(); it != requests->end(); ) {
        if (processIfCan((*it))) it = requests->erase(it);
        else ++it;
    }
}

bool RoutingDaemon::processIfCan(Request* request) {

    // ищем соседей узла, пославшего запрос
    int senderID = request->getSourceId();
    neighbors    = new vector<int>();
    for(int i=0; i<senderID; i++)
        if(isConnected(senderID, i)) neighbors->push_back(i);

    for(int i=senderID+1; i<instance->getNumHosts(); i++)
       if(isConnected(senderID, i)) neighbors->push_back(i);

    for(int i = 0; i < routingHeuristics->size(); i++) {
        int nodeForRouting = -1;

        if ((*routingHeuristics)[i]->canProcess(request, neighbors, nodeForRouting)) {
            ASSERT(nodeForRouting >= 0 && nodeForRouting < getNumHosts() && nodeForRouting != request->getSourceId());
            // проставл€ем им€ сработавшей эвристики дл€ данного пакета
            request->getPacket()->setLastHeuristric((*routingHeuristics)[i]->getName());

            // ѕосылаем отклик на обработку запроса источнику запроса.
            // nodeForRoutePacket - узел, которому бует передан пакет на узле источнике
            Response* response = new Response(nodeForRouting, request);
            take(response);
            sendDirect(response, getParentModule()->getSubmodule("host", request->getSourceId())->gate("in"));

            delete neighbors;
            return true;
        }
    }
    delete neighbors;
    return false;
}

void RoutingDaemon::calculateICT(int nodeId1, int nodeId2) {
    simtime_t Start = getStartConnectionTime(nodeId1, nodeId2);
    simtime_t Lost  = getLostConnectionTime(nodeId1, nodeId2);

    if (Start == 0 && Lost == 0) return; // начальное состо€ние игнорируем

    HistoryCollector::collectICT(Lost - Start);
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


simtime_t RoutingDaemon::getConnectivity(int index, int nodeId1, int nodeId2) {
    if (nodeId1 > nodeId2) return (*RoutingDaemon::connectivityPerDay)[index][nodeId1][nodeId2];
    else return (*RoutingDaemon::connectivityPerDay)[index][nodeId2][nodeId1];
}


simtime_t RoutingDaemon::computeTotalConnectivity(int nodeId1, int nodeId2) {
    simtime_t totalConnectivity = 0;
    for (unsigned int day = 0; day < RoutingDaemon::connectivityPerDay->size(); day++)
        totalConnectivity += getConnectivity(day, nodeId1, nodeId2);
    return totalConnectivity;
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
