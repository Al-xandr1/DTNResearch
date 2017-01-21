#include "RDListener.h"

RD_Listener::RD_Listener(RoutingDaemon* rd)
{
    cout << "RD_Listener constructor: start... " << endl;
    this->rd = rd;

    NodeId = -1;
    position = Coord::ZERO;

    for (int i=0; i<RoutingDaemon::numHosts; i++) nodePositions.push_back(Coord::ZERO);

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

        RealMobility* src3 = dynamic_cast<RealMobility*>(source);
        if (src3) NodeId = src3->getNodeID();

        //ASSERT(checkReceivedData());
        if (processReceivedData()) {
            rd->connectionsChanged();
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
            rd->calculateICT(NodeId, j);
            anyChanged = true;

        } else if( RoutingDaemon::connections[NodeId][j] && !conn ) {
            // Разрываем соединение
            RoutingDaemon::connectLost[NodeId][j] = simTime();
            RoutingDaemon::sumOfConnectDuration[NodeId][j] += (simTime() - RoutingDaemon::connectStart[NodeId][j]);
//            ASSERT(RoutingDaemon::sumOfConnectDuration[NodeId][j] <= RoutingDaemon::dayDuration);
            anyChanged = true;
        }
        RoutingDaemon::connections[NodeId][j] = conn;
    }

    for (int i=NodeId+1; i<RoutingDaemon::numHosts; i++) {
        bool conn = isConnected(i, NodeId);
        if(!RoutingDaemon::connections[i][NodeId] &&  conn ) {
            // Устанавливаем соединение
            RoutingDaemon::connectStart[i][NodeId] = simTime();
            rd->calculateICT(i, NodeId);
            anyChanged = true;

        } else if( RoutingDaemon::connections[i][NodeId] && !conn ) {
            // Разрываем соединение
            RoutingDaemon::connectLost[i][NodeId] = simTime();
            RoutingDaemon::sumOfConnectDuration[i][NodeId] += (simTime() - RoutingDaemon::connectStart[i][NodeId]);
//            ASSERT(RoutingDaemon::sumOfConnectDuration[i][NodeId] <= RoutingDaemon::dayDuration);
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
