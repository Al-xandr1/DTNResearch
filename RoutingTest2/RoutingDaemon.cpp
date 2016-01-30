#include "RoutingDaemon.h"

Define_Module(RoutingDaemon);

int RoutingDaemon::numHosts;
double RoutingDaemon::interconnectionRadius;
bool** RoutingDaemon::connections = NULL;
simtime_t** RoutingDaemon::connectStart = NULL;
simtime_t** RoutingDaemon::connectLost = NULL;


void RoutingDaemon::initialize()
{
    interconnectionRadius = getParentModule()->par("interconnectionRadius");
    numHosts = getParentModule()->par("numHosts");
    in = gate("in");
}

void RoutingDaemon::handleMessage(cMessage *msg)
{
    cout << "RoutingDeamon: processing of changed matrix of connection...";
    delete msg;

//    // бежим по все узлам с целью...
//    for (int i=0; i<RoutingDaemon::numHosts; i++) {
//        MobileHost* host = check_and_cast<MobileHost*>(RoutingDaemon::instance->getSubmodule("host", i));
//
//        // ...чтобы определить какому узлу с каким требуется соединение
//        // и собрать информацию о наличи фактического соединения
//        vector<int>* connectedTargetIds = new vector<int>();
//
//        vector<Packet*>* packets = host->getPacketsForSending();
//        for (int i=0; i<packets->size(); i++) {
//            int targetId = (*packets)[i]->getNodeIdTrg();
//            if (isConnected(host->getNodeId(), targetId)) connectedTargetIds->push_back(targetId);
//        }
//
////        ConnectionMessage* msg = new ConnectionMessage(host->getNodeId(), connectedTargetIds);
////        msg->setKind(1);
////        host->send(msg, host->gate("in"));
//    }
}
