#include "RoutingDaemon.h"

Define_Module(RoutingDaemon);

int RoutingDaemon::numHosts;
double RoutingDaemon::interconnectionRadius;
bool** RoutingDaemon::connections = NULL;
simtime_t** RoutingDaemon::connectStart = NULL;
simtime_t** RoutingDaemon::connectLost = NULL;
vector<Request*>* RoutingDaemon::requests = NULL;
RoutingDaemon* RoutingDaemon::instance = NULL;


void RoutingDaemon::initialize()
{
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
}

void RoutingDaemon::handleMessage(cMessage *msg)
{
    if (msg->getKind() == REQUEST_FOR_ROUTING) {
        Request* request = check_and_cast<Request*>(msg);
//        cout << "RoutingDeamon: received request from node: " << request->getNodeIdSrc() << " to node: " << request->getNodeIdTrg() << endl;

        if (processIfCan(request)) delete request;
        else requests->push_back(request);

    } else {
        cout << "RoutingDaemon::handleMessage: msg->getKind() = " << msg->getKind() << endl;
        cout << "Sender: " << msg->getSenderModule()->getFullName() << endl;
        exit(-444);
    }
}

//todo если не учитывать первое появление соединения, то можно почти всё перенести в этот метод
void RoutingDaemon::calculateICT(int i, int j, simtime_t oldStart, simtime_t oldLost, simtime_t newStart)
{
    if (i <= j) exit(987); //такой ситуации быть не должно
    if (oldStart == 0 && oldLost == 0) return; //первое соединение игнорируем

    simtime_t ict = newStart - oldLost;

    ICTMessage* ictMsg = new ICTMessage(i, j, ict);
    ictMsg->setKind(ICT_INFO);
    take(ictMsg);
    sendDirect(ictMsg, collectorGate);
}

void RoutingDaemon::connectionsChanged()
{
    for(vector<Request*>::iterator it = requests->begin(); it != requests->end(); ) {
        if (processIfCan((*it))) it = requests->erase(it);
        else ++it;
    }
}

bool RoutingDaemon::processIfCan(Request* request) {
    bool canProcess;
    if (request->getNodeIdSrc() > request->getNodeIdTrg()) {
        canProcess = RoutingDaemon::connections[request->getNodeIdSrc()][request->getNodeIdTrg()];
    } else {
        canProcess = RoutingDaemon::connections[request->getNodeIdTrg()][request->getNodeIdSrc()];
    }

    if (canProcess) {
        vector<int>* trgs = new vector<int>();
        trgs->push_back(request->getNodeIdTrg());
        ConnectionMessage* connMsg = new ConnectionMessage(request->getNodeIdSrc(), trgs);
        connMsg->setKind(ESTABLISED_CONNECTION);

        cGate *dst = getParentModule()->getSubmodule("host", request->getNodeIdSrc())->gate("in");
        take(connMsg);
        sendDirect(connMsg, dst);
    }

    return canProcess;
}

void RoutingDaemon::log()
{
    cout << "NodeIds:" << endl;
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        MobileHost* host = check_and_cast<MobileHost*>(getParentModule()->getSubmodule("host", i));
        int nodeId = host->getNodeId();
        cout << "nodeId = " << nodeId << "  ";
    }
    cout << endl << endl;
}
