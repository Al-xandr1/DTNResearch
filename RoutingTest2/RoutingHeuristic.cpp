#include "RoutingHeuristic.h"


bool OneHopHeuristic::canProcess(Request* request, int& nodeForSendResponse) {
    bool canProcess = rd->isConnected(request->getNodeIdSrc(), request->getNodeIdTrg());
    if (canProcess) {
        nodeForSendResponse = request->getNodeIdTrg();
        return true;
    }
    return false;
}


bool TwoHopsHeuristic::canProcess(Request* request, int& nodeForSendResponse) {
    for (int neighbor = 0; neighbor < rd->getNumHosts(); neighbor++) {
        // просматриваем всех соседей, включая себя
        if (rd->isConnected(request->getNodeIdSrc(), neighbor)) {
            bool canProcess = rd->isConnected(neighbor, request->getNodeIdTrg());
            if (canProcess) {
                nodeForSendResponse = neighbor;
                return true;
            }
        }
    }
    return false;
}


bool LETHeuristic::canProcess(Request* request, int& nodeForSendResponse) {
    int moreSuitableNode = request->getNodeIdSrc();
    simtime_t minTime = simTime() - rd->getLostConnectionTime(request->getNodeIdSrc(), request->getNodeIdTrg());
    for (int neighbor = 0; neighbor < RoutingDaemon::numHosts; neighbor++) {
        // просматриваем всех соседей, включая себя
        if (rd->isConnected(request->getNodeIdSrc(), neighbor)) {
            simtime_t lost = rd->getLostConnectionTime(neighbor, request->getNodeIdTrg());
            simtime_t spentTime = simTime() - lost;

            // for debug
            simtime_t start = rd->getStartConnectionTime(neighbor, request->getNodeIdTrg());
            if (spentTime < 0) {cout << "spentTime = " << spentTime; exit(-432);}
            if ( !((lost > start) || (lost == start && lost == 0)) ) {
                cout << "getLostConnectionTime(" << neighbor<< ", "
                       << request->getNodeIdTrg() << ") = " << rd->getLostConnectionTime(neighbor, request->getNodeIdTrg()) << endl;
                cout << "getStartConnectionTime(" << neighbor<< ", "
                        << request->getNodeIdTrg() << ") = " << rd->getStartConnectionTime(neighbor, request->getNodeIdTrg());
                //todo this may happaned in case of switching roots
                exit(-433);
            }// for debug

            if (spentTime < minTime) {
                minTime = spentTime;
                moreSuitableNode = neighbor;
            }
            //todo process case when spent time of differents nodes are equal
        }
    }

    // когда выбирается текущий узел как подходящий, тогда маршрутизация невозможна
    bool canProcess = (moreSuitableNode != request->getNodeIdSrc());
    if (canProcess) {
        nodeForSendResponse = moreSuitableNode;
        return true;
    }

    // for debug
//    cout << "request: " << request->getNodeIdSrc() << " -> " << request->getNodeIdTrg() << " ! ";
//    for (int neighbor=0; neighbor<RoutingDaemon::numHosts; neighbor++)
//        if (rd->isConnected(request->getNodeIdSrc(), neighbor))
//            cout << neighbor << " - " << rd->getLostConnectionTime(neighbor, request->getNodeIdTrg()) << ", ";
//    cout << endl;
    // for debug

    return false;
}


bool MoreFrequentVisibleFHeuristic::canProcess(Request* request, int& nodeForSendResponse) {
    int moreSuitableNode = request->getNodeIdSrc();
    simtime_t maxConnectivity = 0;
    for (int neighbor=0; neighbor<RoutingDaemon::numHosts; neighbor++) {
        // просматриваем всех соседей, включая себя
        if (rd->isConnected(request->getNodeIdSrc(), neighbor)) {
            if (neighbor == request->getNodeIdTrg()) exit(-916); // for debug

            simtime_t totalConnectivity = 0;
            for (unsigned int day = 0; day < RoutingDaemon::connectivityPerDay->size(); day++)
                totalConnectivity += rd->getConnectivity(day, neighbor, request->getNodeIdTrg());

            if (totalConnectivity > maxConnectivity) {
                maxConnectivity = totalConnectivity;
                moreSuitableNode = neighbor;
            }
            //todo process case when spent time of differents nodes are equal
        }
    }

    // когда выбирается текущий узел как подходящий, тогда маршрутизация невозможна
    bool canProcess = moreSuitableNode != request->getNodeIdSrc();
    if (canProcess) {
        nodeForSendResponse = moreSuitableNode;
        //todo while do not change time matrix this case unreachable
        //exit(-694);
        return true;
    }

    return false;
}

