#include "RoutingHeuristic.h"

//Определяет подходящий ли узел для рассмотрения его как потенциального транзитного узла
bool RoutingHeuristic::isSuitableTransitNeighbor(int trinsitId, Request* request) {
    ASSERT(0 <= trinsitId && trinsitId < rd->getNumHosts()); // узел должен входить в диапазон
    ASSERT(!rd->isConnected(request->getSourceId(), request->getDestinationId())); // если ищем транзит, то явной связи нет

    if (rd->isConnected(request->getSourceId(), trinsitId)           //узел является соседом для источника
            && request->getSourceId() != trinsitId                   //узел не является сам себе соседом
            && request->getPacket()->getLastVisitedId() != trinsitId) { //сосед не есть последний посещённый пакетом узел
        ASSERT(trinsitId != request->getDestinationId());    // узел именно тразитный
        return true;
    }

    return false;
}


//todo помнить о возможности зацикливания пакета из-за принятия решения разными эвристиками
//временной порог на использование LETHeuristic ?


bool OneHopHeuristic::canProcess(Request* request, int& nodeForRouting) {
    if (rd->isConnected(request->getSourceId(), request->getDestinationId())) {
        nodeForRouting = request->getDestinationId();
        return true;
    }
    return false;
}


bool TwoHopsHeuristic::canProcess(Request* request, int& nodeForRouting) {
    for (int neighbor = 0; neighbor < rd->getNumHosts(); neighbor++) {
        if (isSuitableTransitNeighbor(neighbor, request)) {

            if (rd->isConnected(neighbor, request->getDestinationId())) {
                nodeForRouting = neighbor;
                return true;
            }
            //todo process case when thus neighbors more than one
        }
    }
    return false;
}


bool LETHeuristic::canProcess(Request* request, int& nodeForRouting) {
    int moreSuitableNode = request->getSourceId();
    simtime_t maxLost = rd->getLostConnectionTime(request->getSourceId(), request->getDestinationId());

    for (int neighbor = 0; neighbor < rd->getNumHosts(); neighbor++) {
        if (isSuitableTransitNeighbor(neighbor, request)) {

            simtime_t lost = rd->getLostConnectionTime(neighbor, request->getDestinationId());

            simtime_t start = rd->getStartConnectionTime(neighbor, request->getDestinationId());
            ASSERT(lost >= 0 && lost <= simTime());
            ASSERT((lost > start) || (lost == start && lost == 0));

            if (lost > maxLost) {
                maxLost = lost;
                moreSuitableNode = neighbor;
            }
            //todo process case when spent time of different nodes are equal and thus neighbors more than one
        }
    }

    // когда выбирается текущий узел как подходящий, тогда маршрутизация невозможна
    if (maxLost < trustTimeThreshold && moreSuitableNode != request->getSourceId()) {
        nodeForRouting = moreSuitableNode;

        //for debug
        //request->print(); cout << "LET: moreSuitableNode = " << moreSuitableNode << endl;
        return true;
    }

    // for debug
    //request->print();
    //for (int neighbor = 0; neighbor < rd->getNumHosts(); neighbor++)
    //    if (rd->isConnected(request->getSourceId(), neighbor))
    //        cout << neighbor << "-" << request->getDestinationId() << ": "<< rd->getLostConnectionTime(neighbor, request->getDestinationId()) << ", ";
    //cout << endl;
    // for debug

    return false;
}


bool MoreFrequentVisibleHeuristic::canProcess(Request* request, int& nodeForRouting) {
    int moreSuitableNode = request->getSourceId();
    simtime_t maxConnectivity = rd->computeTotalConnectivity(request->getSourceId(), request->getDestinationId());

    for (int neighbor = 0; neighbor < rd->getNumHosts(); neighbor++) {
        if (isSuitableTransitNeighbor(neighbor, request)) {

            simtime_t totalConnectivity = rd->computeTotalConnectivity(neighbor, request->getDestinationId());
            if (totalConnectivity > maxConnectivity) {
                maxConnectivity = totalConnectivity;
                moreSuitableNode = neighbor;
            }
            //todo process case when spent time of differents nodes are equal
        }
    }

    // когда выбирается текущий узел как подходящий, тогда маршрутизация невозможна
    if (moreSuitableNode != request->getSourceId()) {
        nodeForRouting = moreSuitableNode;
        //todo while do not change time matrix this case unreachable

        //for debug
        //request->print(); cout << "MoreFreq: moreSuitableNode = " << moreSuitableNode << endl;
        return true;
    }

    return false;
}

