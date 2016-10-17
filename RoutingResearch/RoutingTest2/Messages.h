#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdlib.h>

#include "INETDefs.h"

#include "Coord.h"

using namespace std;

// Коды сообщений
#define FOR_NEW_PACKET       1  // сообщение для создании нового пакета
#define NEW_PACKET_CREATED   2  // сообщение о созданном новом пакете
#define RESPONSE_FOR_REQUEST 3  // сообщение о соединении
#define PACKET               4  // обозначение простого пакета
#define REQUEST_FOR_ROUTING  5  // заявка на маршрутизацию
#define PACKET_RECEIVED      6  // сообщение о полученном пакете
#define ICT_INFO             7  // сообщение с информацией о ICT
#define DAY_START            8  // сообщение о начале нового "дня" моделирования

// Коды сообытий, происходящими с сообщениями
#define CREATED_EVENT        "CRT"    // событие создания пакета в системе
#define REGISTERED_EVENT     "RGS"    // событие регистрации пакета на узле (например, в результате получения или после создания)
#define BEFORE_SEND_EVENT    "BFS"    // событие перед отправкой события на какой-либо узел
#define REMOVED_EVENT        "RMV"    // событие удаления пакета из системы БЕЗ доставки до получателя
#define DELIVERED_EVENT      "DLV"    // событие о доставке сообщения и его последующем удалении


// Пакет для передачи
class Packet : public cPacket
{
private:
    int   sourceId;         // const
    int   lastVisitiedId;
    int   destinationId;    // const
    char* lastHeuristric;

    simtime_t creationTime; // const
    simtime_t receivedTime;
    simtime_t lastLET;      // время потери контакта с адресатом при последней LET маршрутизации

    //for statistics collection
    vector<int>         IDhistory;
    vector<simtime_t>   timeHistory;
    vector<double>      xCoordinates;
    vector<double>      yCoordinates;
    vector<char*>       heuristicHistory;
    vector<char*>       eventHistory;

public:

    Packet(int sourceId, int destinationId) {
        this->sourceId       = sourceId;
        this->lastVisitiedId = sourceId;
        this->destinationId  = destinationId;
        this->lastHeuristric = NULL;
        this->creationTime   = simTime();
        this->receivedTime   = 0;
        this->lastLET        = 0;
        this->setKind(PACKET);
    }

    ~Packet() {
        lastHeuristric = NULL;
    }

    int   getSourceId()       {return sourceId;}
    int   getLastVisitedId()  {return lastVisitiedId;}
    int   getDestinationId()  {return destinationId;}
    char* getLastHeuristric() {return lastHeuristric;}

    void  setLastVisitedId(int lastVisitiedId)    {this->lastVisitiedId = lastVisitiedId;}
    void  setLastHeuristric(char* lastHeuristric) {this->lastHeuristric = lastHeuristric;}
    void  setReceivedTime(simtime_t time) {receivedTime = time;}
    void  setLastLET(simtime_t time)      {lastLET = time;}

    simtime_t getCreationTime() {return creationTime;}
    simtime_t getReceivedTime() {return receivedTime;}
    simtime_t getLiveTime()     {return receivedTime - creationTime;}
    simtime_t getLastLET()      {return lastLET;}

    //for statistics collection
    void collectCreated(int nodeId, Coord position)     {collect(nodeId, position, (char*) CREATED_EVENT);}
    void collectRegistered(int nodeId, Coord position)  {collect(nodeId, position, (char*) REGISTERED_EVENT);}
    void collectBeforeSend(int nodeId, Coord position)  {collect(nodeId, position, (char*) BEFORE_SEND_EVENT);}
    void collectRemoved(int nodeId, Coord position)     {collect(nodeId, position, (char*) REMOVED_EVENT);}
    void collectDelivered(int nodeId, Coord position)   {collect(nodeId, position, (char*) DELIVERED_EVENT);}

private:

    void collect(int nodeId, Coord position, char* event) {
        IDhistory.push_back(nodeId);
        timeHistory.push_back(simTime());
        xCoordinates.push_back(position.x);
        yCoordinates.push_back(position.y);
        heuristicHistory.push_back((char*) getLastHeuristric());
        eventHistory.push_back(event);
    }

public:

    //todo написать метод по формированию записи с данными в файл (xml ?). Использовать его в двух разных контекстах

    void printHistory() {
        cout<<"Source:"<<getSourceId()<<"\t Destination:"<<getDestinationId()<<endl;
        cout<<"Creation time:"<<getCreationTime()<<"\t Received time:"<<getReceivedTime()<<endl;
        cout<<"Live time:"<<getLiveTime()<<endl;
        cout<<"Routing history:\n";
        for(int i=0; i<IDhistory.size(); i++)
            cout<<IDhistory[i]<<"\t"<<timeHistory[i]<<"\t"<<xCoordinates[i]<<"\t"<<yCoordinates[i]
                <<"\t"<<heuristicHistory[i]<<"\t"<<eventHistory[i]<<endl;
    }
};



// Заявка на передачу пакета от узла sourceId узлу destinationId
class Request : public cMessage
{
private:
    int     sourceId;
    int     destinationId;
    Packet* packet;

public:
    Request(int sourceId, int destinationId, Packet* packet){
        this->sourceId = sourceId;
        this->destinationId = destinationId;
        this->packet = packet;
        this->setKind(REQUEST_FOR_ROUTING);
    }

    int     getSourceId()       {return sourceId;}
    int     getDestinationId()  {return destinationId;}
    Packet* getPacket()         {return packet;}
    void    print()             {cout << "req: " << sourceId << "->" << destinationId << " ! ";}
};



// Отклик на заявку request для передачи пакета узлу destinationId
class Response : public cMessage
{
private:
    int      destinationId;   //данному узлу нужно отправить пакет, соответствующий запросу
    Request* request;

public:
    Response(int destinationId, Request* request){
        this->destinationId = destinationId;
        this->request = request;
        this->setKind(RESPONSE_FOR_REQUEST);
    }

    int      getDestinationId()  {return destinationId;}
    Request* getRequest() {return request;}
};



// Сообщение о создании нового пакета
class NewPacketCreated : public cMessage
{
public:
    NewPacketCreated(){
        this->setKind(NEW_PACKET_CREATED);
    }
};



// Сообщение о получении пакета узлом
class PacketReceived : public cMessage
{
private:
    simtime_t liveTime;

public:
    PacketReceived(simtime_t liveTime){
        this->liveTime = liveTime;
        this->setKind(PACKET_RECEIVED);
    }

    simtime_t getLiveTime() {return liveTime;}
};



// Сообщение с временем ICT между узлами i и j
class ICTMessage : public cMessage
{
private:
    int i; // id одного узла
    int j; // id другого узла
    simtime_t ict;

public:
    ICTMessage(int i, int j, simtime_t ict) {
        this->i = i;
        this->j = j;
        this->ict = ict;
        this->setKind(ICT_INFO);
    }

    int getI() {return i;}
    int getJ() {return j;}
    simtime_t getICT() {return ict;}
};



// Сообщение о наличии соединение узла nodeIdSrc со несколькими узлами
//class ConnectionMessage : public cMessage
//{
//private:
//    int nodeIdSrc;
//    vector<int>* connectedTargetIds;
//
//public:
//    ConnectionMessage(int nodeIdSrc, vector<int>* connectedTargetIds){
//        this->nodeIdSrc = nodeIdSrc;
//        this->connectedTargetIds = connectedTargetIds;
//        if (connectedTargetIds->size() != 1) {cout << "ConnectionMessage::connectedTargetIds != 1" << endl; exit(-887);}
//    }
//
//    ~ConnectionMessage() {
//        if (connectedTargetIds != NULL) delete connectedTargetIds;
//    }
//
//    int getNodeIdSrc() {return nodeIdSrc;}
//    vector<int>* getConnectedTargetIds() {return connectedTargetIds;}
//};

#endif
