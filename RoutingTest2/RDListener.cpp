#include "RDListener.h"

Define_Module(RD_Listener);

void RD_Listener::initialize()
{
    cout << "RD_Listener: initializing... " << endl;

    rd = check_and_cast<RoutingDaemon*>(getParentModule()->getSubmodule("routing"));

    NodeId = -1;
    position = Coord::ZERO;

    for (int i=0; i<RoutingDaemon::numHosts; i++) nodePositions.push_back(Coord::ZERO);

    // Создаём нижнетреугольную матрицу
    RoutingDaemon::connections = new bool*[RoutingDaemon::numHosts];
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        RoutingDaemon::connections[i] = new bool[i+1];
        RoutingDaemon::connections[i][i] = true;
        for (int j=0; j<i; j++) RoutingDaemon::connections[i][j] = false;
    }

    RoutingDaemon::connectStart = new simtime_t*[RoutingDaemon::numHosts];
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        RoutingDaemon::connectStart[i] = new simtime_t[i+1];
        RoutingDaemon::connectStart[i][i] = 0;
        for (int j=0; j<i; j++) RoutingDaemon::connectStart[i][j] = 0;
    }

    RoutingDaemon::connectLost = new simtime_t*[RoutingDaemon::numHosts];
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        RoutingDaemon::connectLost[i] = new simtime_t[i+1];
        RoutingDaemon::connectLost[i][i] = 0;
        for (int j=0; j<i; j++) RoutingDaemon::connectLost[i][j] = 0;
    }

    getParentModule()->subscribe(mobilityStateChangedSignal, this);

    cout << "RD_Listener: initialized" << endl;
}

void RD_Listener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
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

    bool anyChanged = false;
    for (int j=0; j<NodeId; j++) {
        bool conn = isConnected(NodeId, j);
        if(!RoutingDaemon::connections[NodeId][j] &&  conn ) RoutingDaemon::connectStart[NodeId][j]= simTime();
        if( RoutingDaemon::connections[NodeId][j] && !conn ) RoutingDaemon::connectLost[NodeId][j] = simTime();
        anyChanged |= RoutingDaemon::connections[NodeId][j] ^ conn;
        RoutingDaemon::connections[NodeId][j] = conn;
    }
    for (int i=NodeId+1; i<RoutingDaemon::numHosts; i++) {
        bool conn = isConnected(i, NodeId);
        if(!RoutingDaemon::connections[i][NodeId] &&  conn ) RoutingDaemon::connectStart[i][NodeId]= simTime();
        if( RoutingDaemon::connections[i][NodeId] && !conn ) RoutingDaemon::connectLost[i][NodeId] = simTime();
        anyChanged |= RoutingDaemon::connections[i][NodeId] ^ conn;
        RoutingDaemon::connections[i][NodeId] = conn;
    }

    if (anyChanged) {
        //todo ЧТО ДЕЛАТЬ????
        send(new cMessage(), gate("out"));
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
    cout << "NodeId = " << NodeId << endl;
    cout << "position = " << position << endl;
    cout << "RoutingDaemon::numHosts = " << RoutingDaemon::numHosts << endl;

    cout << "All positions: " << endl;
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        cout << "NodeId=" << i << "  x="<< nodePositions[i].x <<" y="<< nodePositions[i].y <<endl;
    }

    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        for (int j=0; j<=i; j++) {
            cout << RoutingDaemon::connections[i][j] << "  ";
        }
        cout << endl;
    }
    cout << endl;

    cout << "NodeIds:" << endl;
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        MobileHost* host = check_and_cast<MobileHost*>(getParentModule()->getSubmodule("host", i));
        int nodeId = host->getNodeId();
        cout << "nodeId = " << nodeId << "  ";
    }
    cout << endl << endl;
}
