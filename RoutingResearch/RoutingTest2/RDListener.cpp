#include "RDListener.h"

RD_Listener::RD_Listener()
{
    cout << "RD_Listener constructor: start... " << endl;

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

    cout << "RD_Listener constructor: end! " << endl;
}

void RD_Listener::receiveSignal(cComponent *source, simsignal_t signalID, cObject *obj)
{
    if (signalID == mobilityStateChangedSignal) {
        IMobility *mobility = check_and_cast<IMobility*>(source);
        position = mobility->getCurrentPosition();

        LevyHotSpotsLATP* src1 = dynamic_cast<LevyHotSpotsLATP*>(source);
        if (src1) NodeId = src1->getNodeID();

        SelfSimLATP* src2 = dynamic_cast<SelfSimLATP*>(source);
        if (src2) NodeId = src2->getNodeID();

        //checkReceivedData(); // for debugging
        if (processReceivedData()) {
            RoutingDaemon::instance->connectionsChanged();
        }
        //log(); // for debugging
     }
}

bool RD_Listener::processReceivedData()
{
    nodePositions[NodeId] = position;

    bool anyChanged = false;

    for (int j=0; j<NodeId; j++) {
        bool conn = isConnected(NodeId, j);
        if(!RoutingDaemon::connections[NodeId][j] &&  conn ) {
            // Устанавливаем соединение
            RoutingDaemon::instance->calculateICT(NodeId, j);
            RoutingDaemon::connectStart[NodeId][j] = simTime();
            RoutingDaemon::connectLost[NodeId][j] = MAXTIME;
            anyChanged = true;

        } else if( RoutingDaemon::connections[NodeId][j] && !conn ) {
            // Разрываем соединение
            RoutingDaemon::connectLost[NodeId][j] = simTime();
            RoutingDaemon::instance->accumulateConnectivity(NodeId, j);
            anyChanged = true;
        }
        RoutingDaemon::connections[NodeId][j] = conn;
    }

    for (int i=NodeId+1; i<RoutingDaemon::numHosts; i++) {
        bool conn = isConnected(i, NodeId);
        if(!RoutingDaemon::connections[i][NodeId] &&  conn ) {
            // Устанавливаем соединение
            RoutingDaemon::instance->calculateICT(i, NodeId);
            RoutingDaemon::connectStart[i][NodeId] = simTime();
            RoutingDaemon::connectLost[i][NodeId] = MAXTIME;
            anyChanged = true;

        } else if( RoutingDaemon::connections[i][NodeId] && !conn ) {
            // Разрываем соединение
            RoutingDaemon::connectLost[i][NodeId] = simTime();
            RoutingDaemon::instance->accumulateConnectivity(i, NodeId);
            anyChanged = true;
        }
        RoutingDaemon::connections[i][NodeId] = conn;
    }

    return anyChanged;
}

bool RD_Listener::isConnected(int node1, int node2)
{
    Coord position1 = nodePositions[node1];
    Coord position2 = nodePositions[node2];
    double distance = position1.distance(position2);

    return distance < RoutingDaemon::interconnectionRadius;
}



//-------------------------------------- for debug ---------------------------------------------------
void RD_Listener::checkReceivedData()
{
    if (NodeId < 0 || NodeId >= RoutingDaemon::numHosts) {
        cout << "NodeId=" << NodeId << ", numHosts=" << RoutingDaemon::numHosts;
        exit(-987);
    }
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
    cout << endl << endl;
}
