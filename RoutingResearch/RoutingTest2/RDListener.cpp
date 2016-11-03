#include "RDListener.h"

RD_Listener::RD_Listener()
{
    cout << "RD_Listener constructor: start... " << endl;

    NodeId = -1;
    position = Coord::ZERO;

    for (int i=0; i<RoutingDaemon::numHosts; i++) nodePositions.push_back(Coord::ZERO);

    // Создаём нижнетреугольную матрицу связности
    RoutingDaemon::connections = new bool*[RoutingDaemon::numHosts];
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        RoutingDaemon::connections[i] = new bool[i+1];
        RoutingDaemon::connections[i][i] = true;
        for (int j=0; j<i; j++) RoutingDaemon::connections[i][j] = false;
    }

    // Создаём нижнетреугольную матрицу моментов установления связи
    RoutingDaemon::connectStart = new simtime_t*[RoutingDaemon::numHosts];
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        RoutingDaemon::connectStart[i] = new simtime_t[i+1];
        RoutingDaemon::connectStart[i][i] = 0;
        for (int j=0; j<i; j++) RoutingDaemon::connectStart[i][j] = 0;
    }

    // Создаём нижнетреугольную матрицу моментов разрыва связи
    RoutingDaemon::connectLost = new simtime_t*[RoutingDaemon::numHosts];
    for (int i=0; i<RoutingDaemon::numHosts; i++) {
        RoutingDaemon::connectLost[i] = new simtime_t[i+1];
        RoutingDaemon::connectLost[i][i] = 0;
        for (int j=0; j<i; j++) RoutingDaemon::connectLost[i][j] = 0;
    }

   // Создаём нижнетреугольную матрицу длительностей контакта
    RoutingDaemon::sumOfConnectDuration = new simtime_t*[RoutingDaemon::numHosts];
    for (int i = 0; i < RoutingDaemon::numHosts; i++) {
        RoutingDaemon::sumOfConnectDuration[i] = new simtime_t[i+1];
        RoutingDaemon::sumOfConnectDuration[i][i] = RoutingDaemon::dayDuration;   // соединение самого себя с собой в течение дня = 1 день
        for (int j=0; j<i; j++) RoutingDaemon::sumOfConnectDuration[i][j] = 0;
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

//        ASSERT(checkReceivedData());
        if (processReceivedData()) {
            RoutingDaemon::instance->connectionsChanged();
        }
    }
}


bool RD_Listener::isConnected(int node1, int node2)
{
    Coord position1 = nodePositions[node1];
    Coord position2 = nodePositions[node2];
    double distance = position1.distance(position2);

    return distance < RoutingDaemon::interconnectionRadius;
}


bool RD_Listener::processReceivedData()
{
    nodePositions[NodeId] = position;

    bool anyChanged = false;

    for (int j=0; j<NodeId; j++) {
        bool conn = isConnected(NodeId, j);
        if(!RoutingDaemon::connections[NodeId][j] &&  conn ) {
            // Устанавливаем соединение
            RoutingDaemon::connectStart[NodeId][j] = simTime();
            anyChanged = true;

        } else if( RoutingDaemon::connections[NodeId][j] && !conn ) {
            // Разрываем соединение
            RoutingDaemon::connectLost[NodeId][j] = simTime();
            RoutingDaemon::sumOfConnectDuration[NodeId][j] += simTime() - RoutingDaemon::connectStart[NodeId][j];
            RoutingDaemon::instance->calculateICT(NodeId, j);
            anyChanged = true;
        }
        RoutingDaemon::connections[NodeId][j] = conn;
    }

    for (int i=NodeId+1; i<RoutingDaemon::numHosts; i++) {
        bool conn = isConnected(i, NodeId);
        if(!RoutingDaemon::connections[i][NodeId] &&  conn ) {
            // Устанавливаем соединение
            RoutingDaemon::connectStart[i][NodeId] = simTime();
            anyChanged = true;

        } else if( RoutingDaemon::connections[i][NodeId] && !conn ) {
            // Разрываем соединение
            RoutingDaemon::connectLost[i][NodeId] = simTime();
            RoutingDaemon::sumOfConnectDuration[i][NodeId] += simTime() - RoutingDaemon::connectStart[i][NodeId];
            RoutingDaemon::instance->calculateICT(i, NodeId);
            anyChanged = true;
        }
        RoutingDaemon::connections[i][NodeId] = conn;
    }

    return anyChanged;
}



//-------------------------------------- for debug ---------------------------------------------------
bool RD_Listener::checkReceivedData()
{
    if (0 <= NodeId && NodeId < RoutingDaemon::numHosts) return true;
    log();
    return false;
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
