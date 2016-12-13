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

// PROPHET data structures initialization ---------------------------------------------
double**    RoutingDaemon::P_prophet = NULL;
simtime_t*  RoutingDaemon::U_prophet = NULL;
unsigned**  RoutingDaemon::C_prophet = NULL;
// PROPHET parameters initialization ---------------------------------------------------
double      RoutingDaemon::P_encounter_max    = 0.7;
double      RoutingDaemon::P_encounter_first  = 0.5;
double      RoutingDaemon::P_first_threshold  = 0.1;
double      RoutingDaemon::alpha_prophet      = 0.5;
double      RoutingDaemon::beta_prophet       = 0.9;
double      RoutingDaemon::gamma_prophet      = 0.999;
double      RoutingDaemon::delta_prophet      = 0.01;
double      RoutingDaemon::I_typ              = 100;
int         RoutingDaemon::several            = 5;
// -------------------------------------------------------------------------------------

#define ALL         0   // 0 - нет ограничений (все эвристики) - LET_Threshold работает
#define LET_ONLY    1   // 1 - использовать только LET - LET_Threshold отключается (равен "бесконечности)
#define MFV_ONLY    2   // 2 - использоват только MFV - LET_Threshold не имеет смысла, не используется


void RoutingDaemon::initialize() {
    if (instance == NULL) instance = this; //todo remove this & all static fields
    else { cout << "Duplicate initialization exception!" << endl; exit(-654); }

    RoutingSettings* settings = new RoutingSettings();
    simtime_t LET_Threshold =
       (getParentModule()->par("LET_Threshold").doubleValue() != -1) ?
        getParentModule()->par("LET_Threshold").doubleValue() : MAXTIME;
    int usedHeuristics = getParentModule()->par("usedHeuristics").doubleValue();

    ASSERT(LET_Threshold >= 0);
    cout << " LET_Threshold = "  << LET_Threshold  << endl;

    settings->setLET_Threshold(LET_Threshold);

    routingHeuristics = new vector<RoutingHeuristic*>();
    routingHeuristics->push_back(new OneHopHeuristic(this, settings));
    routingHeuristics->push_back(new TwoHopsHeuristic(this, settings));
    switch (usedHeuristics) {
        case LET_ONLY: {
            settings->setLET_Threshold(MAXTIME); // turn off this constraint
            routingHeuristics->push_back(new LETHeuristic(this, settings));
            break;
        }
        case MFV_ONLY: {
            routingHeuristics->push_back(new MoreFrequentVisibleHeuristic(this, settings));
            break;
        }
        case ALL: {
            routingHeuristics->push_back(new LETHeuristic(this, settings));
            routingHeuristics->push_back(new MoreFrequentVisibleHeuristic(this, settings));
            break;
        }
        default:
            ASSERT(false); //unreacheable statement
    }

    connectivityPerDay    = new vector<simtime_t**>();
    requests              = new vector<Request*>();
    interconnectionRadius = getParentModule()->par("interconnectionRadius");
    numHosts              = getParentModule()->par("numHosts");

    dayDuration   = getParentModule()->par("dayDuration").doubleValue();
    countOfDays   = getParentModule()->par("countOfDays").doubleValue();
    useCODForStat = getParentModule()->par("useCODForStat").boolValue();

    matricesCreation();
    matricesInitialization();

    HistoryCollector::initialize(this);
    getParentModule()->subscribe(mobilityStateChangedSignal, new RD_Listener(this));

    scheduleAt(simTime(), new cMessage("Start of the Day", DAY_START));

    // PROPHET ----------------------------------------------------------------------------------
    scheduleAt(simTime()+(simtime_t)(several*I_typ), new cMessage("PROPHET Timer", SET_TIMER));
    // ------------------------------------------------------------------------------------------
}

void RoutingDaemon::matricesCreation() {
    // Создаём нижнетреугольную матрицу связности
    connections = new bool*[numHosts];
    // Создаём нижнетреугольную матрицу моментов установления связи
    connectStart = new simtime_t*[numHosts];
    // Создаём нижнетреугольную матрицу моментов разрыва связи
    connectLost = new simtime_t*[numHosts];
    // Создаём нижнетреугольную матрицу длительностей контакта
    sumOfConnectDuration = new simtime_t*[numHosts];
    for (int i=0; i<numHosts; i++) {
        connections[i]   = new bool[i+1];
        connectStart[i]  = new simtime_t[i+1];
        connectLost[i]   = new simtime_t[i+1];
        sumOfConnectDuration[i] = new simtime_t[i+1];
    }

    // PROPHET ---------------------------------------------------------------
    // Создаём квадратную матрицу шансов доставки
    P_prophet = new double*[numHosts];
    // Создаём нижнетреугольную матрицу счётчиков контактов
    C_prophet = new unsigned*[numHosts];
    for (int i=0; i<numHosts; i++) {
        P_prophet[i] = new double[numHosts];
        C_prophet[i] = new unsigned[i+1];
    }
    // Создаём вектор моментов последнего обновления матрицы шансов
    U_prophet = new simtime_t[numHosts];
    // -----------------------------------------------------------------------
}

void RoutingDaemon::matricesInitialization() {
    // Приводим в начальное состояние все матрицы
    for (int i=0; i<numHosts; i++) {
        connections[i][i]  = true;
        connectStart[i][i] = 0;
        connectLost[i][i]  = 0;
        sumOfConnectDuration[i][i] = dayDuration;
        for (int j=0; j<i; j++) {
            connections[i][j]  = false;
            connectStart[i][j] = 0;
            connectLost[i][j]  = 0;
            sumOfConnectDuration[i][j] = 0;
        }
    }

	// PROPHET -------------------------------------------------------------------
    for (int i=0; i<numHosts; i++) {
        U_prophet[i]    = 0;
        P_prophet[i][i] = 1;
        C_prophet[i][i] = 0;
        for (int j=0; j<numHosts; j++) P_prophet[i][j] = 0;
        for (int j=0; j<i; j++) C_prophet[i][j] = 0;
    }
    // ---------------------------------------------------------------------------

}

void RoutingDaemon::handleMessage(cMessage *msg) {
    switch (msg->getKind()) {

        case DAY_START: {          // Сообщение о начале нового дня. Копия сообщения рассылается всем узлам сети
            if ( msg->isSelfMessage() ) {
                //синхронное оповещение об окончании дня
                if (getCurrentDay() >= 1)
                    for (int i=0; i < RoutingDaemon::numHosts; i++)
                        check_and_cast<MobileHost*>(getParentModule()->getSubmodule("host", i))->ensureEndRoute();

                processNewDay();

                //"асинхронное" оповещение об окончании дня
                if (getCurrentDay() >= 1)
                    for (int i=0; i < numHosts; i++)
                        sendDirect(new cMessage("Start of the Day", DAY_START), getParentModule()->getSubmodule("host", i)->gate("in"));

                scheduleAt(simTime() + (simtime_t) dayDuration, msg);
            }
            break;
        }

        case REQUEST_FOR_ROUTING: {// Запрос на муршрутизацию. Обрабатываем, если можем или ставим в очередь
            Request* request = check_and_cast<Request*>(msg);
            if (!processIfCan(request)) requests->push_back(request);
            break;
        }

		// PROPHET message handling --------------------------------------------------------
        case SET_TIMER:
            scheduleAt(simTime()+(simtime_t)I_typ, msg);
            PROPHET_timer_processing();
            break;
        // ---------------------------------------------------------------------------------

        default: {// неизвестное сообщение, выводим для отладки
            cout << "RoutingDaemon::handleMessage: msg->getKind() = " << msg->getKind() << endl;
            cout << "Sender: " << msg->getSenderModule()->getFullName() << endl;
            ASSERT(false); //unreacheable statement
            break;
        }
    }
}

bool RoutingDaemon::canCollectStatistics() {
    return !useCODForStat || currentDay > countOfDays;
}

void RoutingDaemon::processNewDay() {

    currentDay++;
    startTimeOfCurrentDay = simTime();
    finishTimeOfCurrentDay = startTimeOfCurrentDay + dayDuration;

    if(currentDay == 1) return; // самое начало, нечего обрабатывать

    // "Обрезаем" длительность текущих сединений перед окончанием дня и корректируем их матрицу
    for (int i = 0; i < RoutingDaemon::numHosts; i++)
        for (int j = 0; j < i; j++)
            if (RoutingDaemon::connections[i][j]) {
                RoutingDaemon::sumOfConnectDuration[i][j] += (simTime() - RoutingDaemon::connectStart[i][j]);
                ASSERT(RoutingDaemon::sumOfConnectDuration[i][j] <= RoutingDaemon::dayDuration);
            }

    // Создаём матрицу длительности соединений для хранения результаов дня
    simtime_t** dayConnectivity = new simtime_t*[RoutingDaemon::numHosts];
    for (int i = 0; i < RoutingDaemon::numHosts; i++) {
        dayConnectivity[i] = new simtime_t[i+1];
        dayConnectivity[i][i] = RoutingDaemon::sumOfConnectDuration[i][i];
        ASSERT(dayConnectivity[i][i] == RoutingDaemon::dayDuration);
        for (int j=0; j<i; j++) {
            dayConnectivity[i][j] = RoutingDaemon::sumOfConnectDuration[i][j];
            ASSERT(dayConnectivity[i][j] <= RoutingDaemon::dayDuration);
        }
    }

    // Сохраняем её в контейнере
    RoutingDaemon::connectivityPerDay->push_back(dayConnectivity);

    // Если в контейнере слишком много матриц, удаляем самую старую
    if (RoutingDaemon::connectivityPerDay->size() > countOfDays) {
        simtime_t** oldest = RoutingDaemon::connectivityPerDay->front();
        RoutingDaemon::connectivityPerDay->erase(RoutingDaemon::connectivityPerDay->begin());
        for(int i = 1; i < RoutingDaemon::numHosts; i++) delete[] oldest[i];
        delete[] oldest;
    }

    // Приводим в начальное состояние все матрицы
    matricesInitialization();
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

    for(int i=senderID+1; i<getNumHosts(); i++)
       if(isConnected(senderID, i)) neighbors->push_back(i);

    for(int i = 0; i < routingHeuristics->size(); i++) {
        int nodeForRouting = -1;

        if ((*routingHeuristics)[i]->canProcess(request, neighbors, nodeForRouting)) {
            ASSERT(nodeForRouting >= 0 && nodeForRouting < getNumHosts() && nodeForRouting != request->getSourceId());
            // проставляем имя сработавшей эвристики для данного пакета
            request->getPacket()->setLastHeuristric((*routingHeuristics)[i]->getName());

            // Посылаем отклик на обработку запроса источнику запроса.
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

    if (Start == 0 && Lost == 0) return; // начальное состояние игнорируем

    //ict = S_i+1 - L_i, поэтому считать нужно после УСТАНОВЛЕНИЯ соединения
    HistoryCollector::collectICT(Start - Lost);
}


simtime_t RoutingDaemon::getLostConnectionTime(int nodeId1, int nodeId2) {
    ASSERT(nodeId1 != nodeId2);
    if (nodeId1 > nodeId2) return RoutingDaemon::connectLost[nodeId1][nodeId2];
    else return RoutingDaemon::connectLost[nodeId2][nodeId1];
}


simtime_t RoutingDaemon::getStartConnectionTime(int nodeId1, int nodeId2) {
    ASSERT(nodeId1 != nodeId2);
    if (nodeId1 > nodeId2) return RoutingDaemon::connectStart[nodeId1][nodeId2];
    else return RoutingDaemon::connectStart[nodeId2][nodeId1];
}


bool RoutingDaemon::isConnected(int nodeId1, int nodeId2) {
    ASSERT(nodeId1 != nodeId2);
    if (nodeId1 > nodeId2) return RoutingDaemon::connections[nodeId1][nodeId2];
    else return RoutingDaemon::connections[nodeId2][nodeId1];
}


simtime_t RoutingDaemon::getConnectivity(int index, int nodeId1, int nodeId2) {
    ASSERT(nodeId1 != nodeId2);
    ASSERT(index >= 0 && index < RoutingDaemon::connectivityPerDay->size());
    if (nodeId1 > nodeId2) return (*RoutingDaemon::connectivityPerDay)[index][nodeId1][nodeId2];
    else return (*RoutingDaemon::connectivityPerDay)[index][nodeId2][nodeId1];
}


simtime_t RoutingDaemon::computeTotalConnectivity(int nodeId1, int nodeId2) {
    ASSERT(nodeId1 != nodeId2);
    ASSERT(RoutingDaemon::connectivityPerDay->size() <= countOfDays);
    simtime_t totalConnectivity = 0;
    for (unsigned int day = 0; day < RoutingDaemon::connectivityPerDay->size(); day++)
        totalConnectivity += getConnectivity(day, nodeId1, nodeId2);
    ASSERT(totalConnectivity <= RoutingDaemon::dayDuration * RoutingDaemon::connectivityPerDay->size());
    return totalConnectivity;
}


// PROPHET methods ------------------------------------------------------------------------
void  RoutingDaemon::PROPHET_aging_P(int node1, int node2, simtime_t lastUpdate)
{
    P_prophet[node1][node2] = P_prophet[node1][node2]*pow(gamma_prophet, SIMTIME_DBL(simTime()-lastUpdate) );
}


void  RoutingDaemon::PROPHET_growing_P(int node1, int node2, simtime_t lastContact)
{
    double P_encounter = ( simTime()-lastContact >= (simtime_t)I_typ)? P_encounter_max : P_encounter_max*SIMTIME_DBL(simTime()-lastContact)/I_typ;
    P_prophet[node1][node2] =  P_prophet[node1][node2] + (1-delta_prophet-P_prophet[node1][node2])*P_encounter;
}


double  RoutingDaemon::PROPHET_transitivity_P(int node1, int node2, int node3)
{
   return max( P_prophet[node1][node3], P_prophet[node1][node2]*P_prophet[node2][node3]*beta_prophet);
}


void RoutingDaemon::PROPHET_connection_starts(int node1, int node2)
 {
    for(int i=0; i<numHosts; i++) {
        if(i != node1 && i != node2) {        // это не тот узел, у которого начался контакт
            if( !isConnected(i, node1) && !isConnected(i, node2)) {  // обработка узлов, не соединённых с обоими сторонами контакта
                PROPHET_aging_P( node1, i, U_prophet[node1]);
                PROPHET_aging_P( node2, i, U_prophet[node2]);
            }
            if( isConnected(node1, i) ) PROPHET_growing_P(node1, i, connectStart[max(node1,i)][min(node1,i)]);
            if( isConnected(node2, i) ) PROPHET_growing_P(node2, i, connectStart[max(node2,i)][min(node2,i)]);
        } else if( i == min(node1, node2) ) { // это узел c меньшим номером, у которого начался контакт, чтобы исключить дублирование
            PROPHET_aging_P( node1, node2, U_prophet[node1]);
            PROPHET_aging_P( node2, node1, U_prophet[node2]);
            if( P_prophet[node1][node2] < P_first_threshold ) P_prophet[node1][node2] = P_encounter_first;
            else PROPHET_growing_P(node1, node2, connectLost[max(node1,node2)][min(node1,node2)]);
            if( P_prophet[node2][node1] < P_first_threshold ) P_prophet[node2][node1] = P_encounter_first;
            else PROPHET_growing_P(node2, node1, connectLost[max(node1,node2)][min(node1,node2)]);
        }
    }
    for(int i=0; i<numHosts; i++) if(i != node1 && i != node2) {
        P_prophet[node1][i] = PROPHET_transitivity_P(node1, node2, i);
        P_prophet[node2][i] = PROPHET_transitivity_P(node2, node1, i);
        C_prophet[max(node1,i)][min(node1,i)]++;
        C_prophet[max(node2,i)][min(node2,i)]++;
        // TODO Послать C_prophet[max(node1,node2)][min(node1,node2)] в коллектор сстатистики
        C_prophet[max(node1,node2)][min(node1,node2)]=0;
    }
    U_prophet[node1]=U_prophet[node2]=simTime();
}


// затычка на всякий случай
void RoutingDaemon::PROPHET_connection_ends(int node1, int node2) {}


void RoutingDaemon::PROPHET_timer_processing()
{
    int node;
    bool isConn;
    for(node=0; node<numHosts; node++) {
        isConn = false;
        for(int i=0; i<node; i++) isConn = (isConn || isConnected(node, i));         // соединён ли узел хоть с кем-нибудь?
        if( isConn && ((simTime()-U_prophet[node])>=(simtime_t)(several*I_typ)) ) {  // если да и уже давно, обрабатываем
            for(int i=0; i<numHosts; i++) {
               if( i != node &&  isConnected(node, i) ) PROPHET_growing_P(node, i, U_prophet[node]);
               if( i != node && !isConnected(node, i) ) PROPHET_aging_P  (node, i, U_prophet[node]);
            }
            for(int i=0; i<numHosts; i++)
            for(int j=0; j<i; j++)
               if(i != node && j != node ) {
                  double         tmp = PROPHET_transitivity_P(node, j, i);
                  P_prophet[node][j] = PROPHET_transitivity_P(node, i, j);
                  P_prophet[node][i] = tmp;
               }
            U_prophet[node]=simTime();
        }
    }
}
// ----------------------------------------------------------------------------------------

//-------------------------------------- for debug ---------------------------------------------------
void RoutingDaemon::log() {
    cout << "NodeIds:" << endl;
    for (int i=0; i<numHosts; i++) {
        MobileHost* host = check_and_cast<MobileHost*>(getParentModule()->getSubmodule("host", i));
        int nodeId = host->getNodeId();
        cout << "nodeId = " << nodeId << "  ";
    }
    cout << endl << endl;
}
