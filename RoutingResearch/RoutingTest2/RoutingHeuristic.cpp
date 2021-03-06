#include "RoutingHeuristic.h"


bool OneHopHeuristic::canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting) {
    if (rd->isConnected(request->getSourceId(), request->getDestinationId())) {
        nodeForRouting = request->getDestinationId();
        return true;
    }
    return false;
}


bool TwoHopsHeuristic::canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting) {
    for (unsigned int i = 0;  i<neighbors->size(); i++)
        if (rd->isConnected(neighbors->at(i), request->getDestinationId())) {
            nodeForRouting = neighbors->at(i);
            return true;
        }
    //todo process case when thus neighbors more than one
    return false;
}


bool LETHeuristic::canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting) {
    int moreSuitableNode = request->getSourceId();
    simtime_t lost;

    // ����, ��� ��������� �� ��� � ������� ����� ��������
    simtime_t maxLost = rd->getLostConnectionTime(request->getSourceId(), request->getDestinationId());
    for (unsigned int i = 0;  i<neighbors->size(); i++) {
        lost = rd->getLostConnectionTime(neighbors->at(i), request->getDestinationId());
        if (lost > maxLost) { maxLost = lost; moreSuitableNode = neighbors->at(i); }
        //todo process case when best neighbors more than one
    }

    simtime_t threshold = settings->getLET_Threshold();        // ����� LET ���������
    simtime_t packetLET = request->getPacket()->getLastLET();  // ����� ������ �������� � ��������� ��� ��������� LET �������������
    if (moreSuitableNode == request->getSourceId() || packetLET >= maxLost || simTime() - maxLost > threshold) return false;
    else {
        nodeForRouting = moreSuitableNode;
        request->getPacket()->setLastLET(maxLost);
        return true;
    }
}


bool MoreFrequentVisibleHeuristic::canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting) {
    int moreSuitableNode = request->getSourceId();
    simtime_t maxConnectivity = rd->computeTotalConnectivity(request->getSourceId(), request->getDestinationId());

    for (unsigned int i = 0;  i<neighbors->size(); i++) {
        simtime_t totalConnectivity = rd->computeTotalConnectivity(neighbors->at(i), request->getDestinationId());
        if (totalConnectivity > maxConnectivity) {
           maxConnectivity = totalConnectivity;
           moreSuitableNode = neighbors->at(i);
        }
       //todo process case when spent time of differents nodes are equal
    }

    // ����� ���������� ������� ���� ��� ����������, ����� ������������� ����������
    if (moreSuitableNode != request->getSourceId()) {
        nodeForRouting = moreSuitableNode; return true;
    }
    else return false;
}


// PROPHET ---------------------------------------------------------------------------------
bool PROPHET_Heuristic::canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting)
{
    int    moreSuitableNode = request->getSourceId();
    int    i_max = request->getSourceId();
    int    dest  = request->getDestinationId();
    double P_max = RoutingDaemon::P_prophet[moreSuitableNode][dest];

    for(unsigned int i = 0;  i<neighbors->size(); i++)
        if( P_max < RoutingDaemon::P_prophet[neighbors->at(i)][dest])
            P_max = RoutingDaemon::P_prophet[ i_max=neighbors->at(i) ][dest];
    if ( P_max > RoutingDaemon::P_first_threshold ) moreSuitableNode = i_max;

    // ����� ���������� ������� ���� ��� ����������, ����� �������� �� �����
    if (moreSuitableNode != request->getSourceId()) {
        nodeForRouting = moreSuitableNode; return true;
    }
    else return false;
}
// --------------------------------------------------------------------------------------------
