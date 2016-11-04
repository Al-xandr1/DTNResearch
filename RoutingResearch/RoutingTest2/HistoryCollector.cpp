#include <HistoryCollector.h>

ofstream* HistoryCollector::packetsHistoryFile = NULL;   // файл с информацией о всех пакетах
ofstream* HistoryCollector::ictHistoryFile = NULL;       // файл с информацией о времени взаимодействия узлов
RoutingDaemon* HistoryCollector::rd = NULL;

void HistoryCollector::initialize(RoutingDaemon* rd) {
    HistoryCollector::rd = rd;
    if (!packetsHistoryFile) {
        packetsHistoryFile = new ofstream(buildFullName(OUT_DIR, PACKETS_HIST));
        (*packetsHistoryFile)<<"<?xml version=\'1.0' ?>"<<endl;
        (*packetsHistoryFile)<<"<PACKETS-HISTORY>"<< endl;
    }
    if (!ictHistoryFile) {
        ictHistoryFile = new ofstream(buildFullName(OUT_DIR, ICT_HIST));
        (*ictHistoryFile)<<"<?xml version=\'1.0' ?>"<<endl;
        (*ictHistoryFile)<<"<ICT-HISTORY>"<< endl;
    }
}

void HistoryCollector::finish() {
//    ASSERT(packetsHistoryFile);
//    ASSERT(ictHistoryFile);

    (*packetsHistoryFile)<<"</PACKETS-HISTORY>"<<endl;
    (*ictHistoryFile)<<"</ICT-HISTORY>"<<endl;

    packetsHistoryFile->close();
    ictHistoryFile->close();

    delete ictHistoryFile;
    delete packetsHistoryFile;

    packetsHistoryFile = NULL;
    ictHistoryFile = NULL;
}

void HistoryCollector::collectDeliveredPacket(Packet* packet)       {/*ASSERT(packetsHistoryFile);*/ collectPacket(packetsHistoryFile, packet);}
void HistoryCollector::collectRemovedPacket(Packet* packet)         {/*ASSERT(packetsHistoryFile);*/ collectPacket(packetsHistoryFile, packet);}
void HistoryCollector::collectICT(simtime_t ict)                    {/*ASSERT(ictHistoryFile); ASSERT(ict>0);*/ write(ict, ictHistoryFile);}
void HistoryCollector::collectPacket(ofstream* out, Packet* packet) {/*ASSERT(out);*/ write(packet, out);}

void HistoryCollector::insertRowCreated(Packet* packet, int nodeId, Coord position)     {insertRow(packet, (char*) CREATED_EVENT,     nodeId, position);}
void HistoryCollector::insertRowRegistered(Packet* packet, int nodeId, Coord position)  {insertRow(packet, (char*) REGISTERED_EVENT,  nodeId, position);}
void HistoryCollector::insertRowBeforeSend(Packet* packet, int nodeId, Coord position)  {insertRow(packet, (char*) BEFORE_SEND_EVENT, nodeId, position);}
void HistoryCollector::insertRowRemoved(Packet* packet, int nodeId, Coord position)     {insertRow(packet, (char*) REMOVED_EVENT,     nodeId, position);}
void HistoryCollector::insertRowDelivered(Packet* packet, int nodeId, Coord position)   {insertRow(packet, (char*) DELIVERED_EVENT,   nodeId, position);}

void HistoryCollector::printHistory(Packet* packet) {write(packet, &cout);}

void HistoryCollector::insertRow(Packet* packet, char* event, int nodeId, Coord position) {
    // означает, если функция canCollectStatistics не NULL и вернула true
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

void HistoryCollector::write(Packet* packet, ostream* out) {
    // означает, если функция canCollectStatistics не NULL и вернула true
    if (rd && rd->canCollectStatistics()) {
        (*out) <<TAB<<"<PACKET>" << endl;
        (*out) <<TAB<<TAB<<"<SUMMARY>"<<packet->getSourceId()<<DLM<<packet->getDestinationId()
               <<DLM<<packet->getCreationTime()<<DLM<<packet->getReceivedTime()<<"</SUMMARY>"<<endl;
        (*out) <<TAB<<TAB<<"<HISTORY>"<<endl;

        for(unsigned int i=0; i<packet->IDhistory.size(); i++)
            (*out) <<TAB<<TAB<<TAB<<"<"<<packet->eventHistory[i]<<">"<<packet->IDhistory[i]<<DLM<<packet->timeHistory[i]
                   <<DLM<<packet->xCoordinates[i]<<DLM<<packet->yCoordinates[i]<<DLM<<packet->heuristicHistory[i]
                   <<"</"<<packet->eventHistory[i]<<">"<<endl;

        (*out) <<TAB<<TAB<<"</HISTORY>"<<endl;
        (*out) <<TAB<<"</PACKET>"<<endl;
    }
}

void HistoryCollector::write(simtime_t ict, ostream* out) {
    // означает, если функция canCollectStatistics не NULL и вернула true
    if (rd && rd->canCollectStatistics()) {
        (*out) <<TAB<<ict<< endl;
    }
}
