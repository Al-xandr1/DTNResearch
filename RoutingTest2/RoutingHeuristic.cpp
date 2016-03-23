#include "RoutingHeuristic.h"


bool OneHopHeuristic::canProcess(Request* request, int& nodeForRouting) {
    if (rd->isConnected(request->getSourceId(), request->getDestinationId())) {
        nodeForRouting = request->getDestinationId();
        return true;
    }
    return false;
}


bool TwoHopsHeuristic::canProcess(Request* request, int& nodeForRouting) {
    //for debug
    if (rd->isConnected(request->getSourceId(), request->getDestinationId())) {exit(-321);}

    for (int neighbor = 0; neighbor < rd->getNumHosts(); neighbor++) {
        // просматриваем всех соседей, включая себя
        if (rd->isConnected(request->getSourceId(), neighbor)) {
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
        // просматриваем всех соседей, включая себя
        if (rd->isConnected(request->getSourceId(), neighbor)) {
            // for debug
            if (neighbor == request->getDestinationId()) exit(-333);

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
            //todo process case when spent time of differents nodes are equal and thus neighbors more than one
        }
    }

    // когда выбирается текущий узел как подходящий, тогда маршрутизация невозможна
    if (moreSuitableNode != request->getSourceId()) {
        nodeForRouting = moreSuitableNode;
        return true;
    }

    // for debug
    //cout << "req: " << request->getNodeIdSrc() << "->" << request->getNodeIdTrg() << " ! ";
    //for (int neighbor = 0; neighbor < rd->getNumHosts(); neighbor++)
    //    if (rd->isConnected(request->getNodeIdSrc(), neighbor))
    //        cout << neighbor << "-" << request->getNodeIdTrg() << ": "<< rd->getLostConnectionTime(neighbor, request->getNodeIdTrg()) << ", ";
    //cout << endl;
    // for debug

    return false;
}


bool MoreFrequentVisibleFHeuristic::canProcess(Request* request, int& nodeForRouting) {
    int moreSuitableNode = request->getSourceId();
    simtime_t maxConnectivity = rd->computeTotalConnectivity(request->getSourceId(), request->getDestinationId());

    for (int neighbor = 0; neighbor < rd->getNumHosts(); neighbor++) {
        // просматриваем всех соседей, включая себя
        if (rd->isConnected(request->getSourceId(), neighbor)) {
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
        return true;
    }

    return false;
}

