#include <HistoryCollector.h>

ofstream* HistoryCollector::packetsHistoryFile = NULL;   // файл с информацией о всех пакетах
ofstream* HistoryCollector::ictHistoryFile = NULL;       // файл с информацией о времени взаимодействия узлов
ofstream* HistoryCollector::routeHistoryFile = NULL;     // файл с информацией о времени взаимодействия узлов
unsigned int HistoryCollector::currentFilePartOfCollectedPackets = 0;
unsigned int HistoryCollector::collectedPackets = 0;
unsigned int HistoryCollector::createdPackets = 0;
unsigned int HistoryCollector::deliveredPackets = 0;
RoutingDaemon* HistoryCollector::rd = NULL;
vector<vector<RouteInfoForNode*>*>* HistoryCollector::routeHistory = NULL;

#define MAX_COLLECTED_PACKETS_IN_HISTORY_PART 1000

void HistoryCollector::initialize(RoutingDaemon* rd) {
    HistoryCollector::rd = rd;

    if (!ictHistoryFile) {
        ictHistoryFile = createXmlFile(ICT_HIST, "<ICT-HISTORY>");
    }

    if (!routeHistoryFile) {
        string openTag = string("<ROUTE-HISTORY maxDayDuration=\"") + to_string(rd->getDayDuration()) + string("\">");
        routeHistoryFile = createXmlFile(RT_HIST, openTag.c_str());
    }

    if (!routeHistory) {
        routeHistory = new vector<vector<RouteInfoForNode*>*>();
        for (int i=0; i<rd->getNumHosts(); i++) {
            routeHistory->push_back(new vector<RouteInfoForNode*>());
        }
    }
}

void HistoryCollector::finish() {
    if (packetsHistoryFile) {
        closeXmlFile(packetsHistoryFile, "</PACKETS-HISTORY>");
        myDelete(packetsHistoryFile);
    }

    closeXmlFile(ictHistoryFile, "</ICT-HISTORY>");
    myDelete(ictHistoryFile);

    ASSERT(routeHistoryFile);
    ASSERT(routeHistory->size() == rd->getNumHosts());
    for (unsigned int nodeId=0; nodeId<routeHistory->size(); nodeId++) {
        write(nodeId, (*routeHistory)[nodeId], routeHistoryFile);
    }

    for (unsigned int nodeId=0; nodeId<routeHistory->size(); nodeId++) {
        for (unsigned int i=0; i<(*routeHistory)[nodeId]->size(); i++) {
            RouteInfoForNode* info = (*(*routeHistory)[nodeId])[i];
            delete info;
        }
        delete (*routeHistory)[nodeId];
    }
    delete routeHistory;

    closeXmlFile(routeHistoryFile, "</ROUTE-HISTORY>");
    myDelete(routeHistoryFile);
}

void HistoryCollector::collectPacket(Packet* packet) {
    ASSERT(packet);

    if (!packetsHistoryFile) {
        string filename;
        std::size_t found;
        string tmp(PACKETS_HIST);
        if ((found = tmp.find(".")) != string::npos) {
            filename = tmp.substr(0, found) + string("_")
                    + string(buildIntParameter("part", currentFilePartOfCollectedPackets, 3))
                    + tmp.substr(found, tmp.size());
        }
        packetsHistoryFile = createXmlFile(filename.c_str(), "<PACKETS-HISTORY>");
    }

    if (write(packet, packetsHistoryFile)) {
        collectedPackets++;

        if (collectedPackets % MAX_COLLECTED_PACKETS_IN_HISTORY_PART == 0) {
            // откусываем кусок файла
            closeXmlFile(packetsHistoryFile, "</PACKETS-HISTORY>");
            myDelete(packetsHistoryFile);
            currentFilePartOfCollectedPackets++;
        }
    }
}

void HistoryCollector::collectICT(simtime_t ict)           {ASSERT(ictHistoryFile); ASSERT(ict>=0); if (ict>0) write(ict, ictHistoryFile);}

void HistoryCollector::insertRouteInfo(int nodeId, unsigned int day, simtime_t startTimeRoute, simtime_t endTimeRoute) {
    if (rd && rd->canCollectStatistics()) {
        ASSERT(0 <= nodeId && nodeId <= rd->getNumHosts());
        ASSERT(startTimeRoute <= endTimeRoute);

        RouteInfoForNode* routeInfo = new RouteInfoForNode(day, startTimeRoute, endTimeRoute);
        (*routeHistory)[nodeId]->push_back(routeInfo);
    }
}

void HistoryCollector::insertRowCreated(Packet* packet, int nodeId, Coord position)     {insertRow(packet, CREATED_EVENT,     nodeId, position);
                                                                                            createdPackets++; ASSERT(deliveredPackets <= createdPackets);}
void HistoryCollector::insertRowRegistered(Packet* packet, int nodeId, Coord position)  {insertRow(packet, REGISTERED_EVENT,  nodeId, position);}
void HistoryCollector::insertRowBeforeSend(Packet* packet, int nodeId, Coord position)  {insertRow(packet, BEFORE_SEND_EVENT, nodeId, position);}
void HistoryCollector::insertRowRemoved(Packet* packet, int nodeId, Coord position)     {insertRow(packet, REMOVED_EVENT,     nodeId, position);}
void HistoryCollector::insertRowDelivered(Packet* packet, int nodeId, Coord position)   {insertRow(packet, DELIVERED_EVENT,   nodeId, position);
                                                                                            deliveredPackets++; ASSERT(deliveredPackets <= createdPackets);}

void HistoryCollector::printHistory(Packet* packet) {write(packet, &cout);}


//-------------------------------------- private ------------------------------------------------

void HistoryCollector::write(int nodeId, vector<RouteInfoForNode*>* routesForNode, ostream* out) {
    (*out) <<TAB<<"<NODE nodeId=\""<<nodeId<<"\">"<< endl;
    for (unsigned int i=0; i<routesForNode->size(); i++) {
        RouteInfoForNode* info = (*routesForNode)[i];
        double dayDuration = (info->endTimeRoute - info->startTimeRoute).dbl();
        ASSERT(dayDuration >= 0);
        (*out) <<TAB<<TAB<<"<ROUTE>"<<info->day<<DLM<<info->startTimeRoute<<DLM<<info->endTimeRoute<<DLM<<dayDuration<<"</ROUTE>"<<endl;
    }
    (*out) <<TAB<<"</NODE>"<< endl;
}

void HistoryCollector::insertRow(Packet* packet, char* event, int nodeId, Coord position) {
    if (rd && rd->canCollectStatistics()) {
        packet->eventHistory.push_back(event);
        packet->IDhistory.push_back(nodeId);
        packet->timeHistory.push_back(simTime());
        packet->xCoordinates.push_back(position.x);
        packet->yCoordinates.push_back(position.y);
        packet->heuristicHistory.push_back(
                (char*) (packet->getLastHeuristric()!=NULL ? packet->getLastHeuristric() : "NULL"));
    }
}

bool HistoryCollector::write(Packet* packet, ostream* out) {
    if (rd && rd->canCollectStatistics()) {
        double threshold = rd->getUseCountOfDaysForStat() ? (rd->getCountOfDays() * rd->getDayDuration()) : 0;
        if (packet->getCreationTime() >= threshold) {
            (*out) <<TAB<<"<PACKET>" << endl;
            (*out) <<TAB<<TAB<<"<SUMMARY>"<<packet->getSourceId()<<DLM<<packet->getDestinationId()
                   <<DLM<<packet->getCreationTime()<<DLM<<packet->getReceivedTime()<<"</SUMMARY>"<<endl;
            (*out) <<TAB<<TAB<<"<HISTORY>"<<endl;

            for(unsigned int i=0; i<packet->IDhistory.size(); i++) {
                if (i==0) ASSERT(strcmp(packet->eventHistory[0], CREATED_EVENT) == 0); //проверить, что учитываемый пакет имеет событие о создании
                (*out) <<TAB<<TAB<<TAB<<"<"<<packet->eventHistory[i]<<">"<<packet->IDhistory[i]<<DLM<<packet->timeHistory[i]
                       <<DLM<<packet->xCoordinates[i]<<DLM<<packet->yCoordinates[i]<<DLM<<packet->heuristicHistory[i]
                       <<"</"<<packet->eventHistory[i]<<">"<<endl;
            }

            (*out) <<TAB<<TAB<<"</HISTORY>"<<endl;
            (*out) <<TAB<<"</PACKET>"<<endl;
            return true;
        } else {
            //проверить, что учитываемый пакет НЕ имеет событие о создании
            if (packet->eventHistory.size()>0) ASSERT(strcmp(packet->eventHistory[0], CREATED_EVENT) != 0);
        }
    }
    return false;
}

void HistoryCollector::write(simtime_t ict, ostream* out) {
    if (rd && rd->canCollectStatistics()) {
        (*out) <<TAB<<ict<< endl;
    }
}
