#include "RoutingHeuristic.h"

//Определяет подходящий ли узел для рассмотрения его как потенциального транзитного узла
bool RoutingHeuristic::isSuitableTransitNeighbor(int trinsitId, Request* request) {
    //for debug
    //if (trinsitId < 0 || trinsitId >= rd->getNumHosts()) exit(-111); // узел должен входить в диапазон
    //if (rd->isConnected(request->getSourceId(), request->getDestinationId())) {exit(-333);} // если ищем транзит, то явной связи нет
    //for debug

    if (rd->isConnected(request->getSourceId(), trinsitId)           //узел является соседом для источника
            && request->getSourceId() != trinsitId                   //узел не является сам себе соседом
            && request->getPacket()->getLastVisitedId() != trinsitId) { //сосед не есть последний посещённый пакетом узел
        //for debug
        //if (trinsitId == request->getDestinationId()) exit(-222);    // узел именно тразитный
        return true;
    }

    return false;
}


//todo помнить о возможности зацикливания пакета из-за принятия решения разными эвристиками
//временной порог на использование LETHeuristic ?


bool OneHopHeuristic::canProcess(Request* request, int& nodeForRouting) {
    if (rd->isConnected(request->getSourceId(), request->getDestinationId())) {
        nodeForRouting = request->getDestinationId();

        //for debug
        //request->print(); cout << "OneHop: moreSuitableNode = " << nodeForRouting << endl;
        return true;
    }
    return false;
}


bool TwoHopsHeuristic::canProcess(Request* request, int& nodeForRouting) {
    for (int neighbor = 0; neighbor < rd->getNumHosts(); neighbor++) {
        if (isSuitableTransitNeighbor(neighbor, request)) {

            if (rd->isConnected(neighbor, request->getDestinationId())) {
                nodeForRouting = neighbor;

                //for debug
                //request->print(); cout << "TwoHops: moreSuitableNode = " << nodeForRouting << endl;
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

            // for debug
            simtime_t start = rd->getStartConnectionTime(neighbor, request->getDestinationId());
            if (lost < 0 || lost > simTime()) {cout << "lost = " << lost; exit(-432);}
            if ( !((lost > start) || (lost == start && lost == 0)) ) {
                cout << "getLostConnectionTime(" << neighbor<< ", "
                       << request->getDestinationId() << ") = " << rd->getLostConnectionTime(neighbor, request->getDestinationId()) << endl;
                cout << "getStartConnectionTime(" << neighbor<< ", "
                        << request->getDestinationId() << ") = " << rd->getStartConnectionTime(neighbor, request->getDestinationId());
                //todo this may happaned in case of switching roots
                exit(-433);
            }// for debug

            if (lost > maxLost) {
                maxLost = lost;
                moreSuitableNode = neighbor;
            }
            //todo process case when spent time of different nodes are equal and thus neighbors more than one
        }
    }

    // когда выбирается текущий узел как подходящий, тогда маршрутизация невозможна
    if (moreSuitableNode != request->getSourceId()) {
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

