#include "RoutingDaemon.h"


//---------------------------------------------RD_Listener-------------------------------------------------------------
RD_Listener::RD_Listener()
{
    NodeId = -1;
    position = Coord::ZERO;

    for (int i=0; i<RoutingDaemon::numHosts; i++) nodePositions.push_back(Coord::ZERO);

    // Создаём нижнетреугольную матрицу
    connections = new int*[RoutingDaemon::numHosts];
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        connections[i] = new int[i+1];
        connections[i][i] = -1;
        for (int j=0; j<i; j++) {
            connections[i][j] = 0;
        }
    }
}

void RD_Listener::receiveSignal (cComponent *source, simsignal_t signalID, cObject *obj)
{
     if (signalID == mobilityStateChangedSignal) {
          LevyHotSpotsLATP *src = check_and_cast<LevyHotSpotsLATP*>(source);
          NodeId = src->getNodeID();
          position = src->getCurrentPosition();

          checkReceivedData(); // for debugging
          processReceivedData();

          log(); // for debugging
     }
}

void RD_Listener::checkReceivedData()
{
    if (NodeId < 0 || NodeId >= RoutingDaemon::numHosts) {
        cout << "NodeId=" << NodeId << ", numHosts=" << RoutingDaemon::numHosts;
        exit(-987);
    }
}

void RD_Listener::processReceivedData()
{
    nodePositions[NodeId] = position;

    for (int j=0; j<NodeId; j++) connections[NodeId][j] = isConnected(NodeId, j);
    for (int i=NodeId+1; i<RoutingDaemon::numHosts; i++) connections[i][NodeId] = isConnected(i, NodeId);


    // бежим по все узлам с целью...
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        MobileHost* host = check_and_cast<MobileHost*>(RoutingDaemon::instance->getSubmodule("host", i));

        // ...чтобы определить какому узлу с каким требуется соединение
        // и собрать информацию о наличи фактического соединения
        vector<int>* connectedTargetIds = new vector<int>();

        vector<Packet*>* packets = host->getPacketsForSending();
        for (int i=0; i<packets->size(); i++) {
            int targetId = (*packets)[i]->getNodeIdTrg();
            if (isConnected(host->getNodeId(), targetId)) connectedTargetIds->push_back(targetId);
        }

        ConnectionMessage* msg = new ConnectionMessage(host->getNodeId(), connectedTargetIds);
        msg->setKind(1);
        host->send(msg, host->gate("in"));
    }
}

bool RD_Listener::isConnected(int node1, int node2)
{
    Coord position1 = nodePositions[node1];
    Coord position2 = nodePositions[node2];
    double distance = position1.distance(position2);

    return distance < RoutingDaemon::interconnectionRadius;
}

void RD_Listener::log()
{
    cout << "All positions: " << endl;
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        cout << "NodeId=" << i << "  x="<< nodePositions[i].x <<" y="<< nodePositions[i].y <<endl;
    }

    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        for (int j=0; j<=i; j++) {
            cout << connections[i][j] << "  ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "NodeIds:" << endl;
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        MobileHost* host = check_and_cast<MobileHost*>(RoutingDaemon::instance->getSubmodule("host", i));
        int nodeId = host->getNodeId();
        cout << "nodeId = " << nodeId << "  ";
    }
    cout << endl << endl;
}



//--------------------------------------------RoutingDaemon------------------------------------------------------------

RoutingDaemon* RoutingDaemon::instance = NULL;
int RoutingDaemon::numHosts = 0;
double RoutingDaemon::interconnectionRadius = 0;

Define_Module(RoutingDaemon);

void RoutingDaemon::initialize()
{
    if (instance == NULL) {
        instance = this;
    } else {
        cout << "Duplicate instance creation RoutingDaemon" << endl;
        exit(-789);
    }

    interconnectionRadius = par("interconnectionRadius");
    numHosts = par("numHosts");
    listener = new RD_Listener();
    subscribe(mobilityStateChangedSignal, listener);
}

