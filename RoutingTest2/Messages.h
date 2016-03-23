#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdlib.h>

#include "INETDefs.h"

using namespace std;

#define FOR_NEW_PACKET 1        // сообщение для создании нового пакета
#define NEW_PACKET_CREATED 2    // сообщение о созданном новом пакете
#define RESPONSE_FOR_REQUEST 3  // сообщение о соединении
#define PACKET 4                // обозначение простого пакета
#define REQUEST_FOR_ROUTING 5   // заявка на маршрутизацию
#define PACKET_RECEIVED 6       // сообщение о полученном пакете
#define ICT_INFO 7              // сообщение с информацией о ICT
#define DAY_START 8             // сообщение о начале нового "дня" моделирования


// Пакет для передачи
class Packet : public cPacket
{
private:
    int sourceId;
    int destinationId;

    simtime_t creationTime;
    simtime_t receivedTime;

public:
    Packet(int sourceId, int destinationId){
        this->sourceId = sourceId;
        this->destinationId = destinationId;
        this->creationTime = 0;
        this->receivedTime = 0;
        this->setKind(PACKET);
    }

    int getSourceId() {return sourceId;}
    int getDestinationId() {return destinationId;}

    void setCreationTime(simtime_t time) {creationTime = time;}
    void setReceivedTime(simtime_t time) {receivedTime = time;}

    simtime_t getCreationTime() {return creationTime;}
    simtime_t getReceivedTime() {return receivedTime;}

    simtime_t getLiveTime() {return receivedTime - creationTime;}
};



// Заявка на передачу пакета от узла sourceId узлу destinationId
class Request : public cMessage
{
private:
    int sourceId;
    int destinationId;
    Packet* packet;

public:
    Request(int sourceId, int destinationId, Packet* packet){
        this->sourceId = sourceId;
        this->destinationId = destinationId;
        this->packet = packet;
        this->setKind(REQUEST_FOR_ROUTING);
    }

    int getSourceId()  {return sourceId;}
    int getDestinationId()  {return destinationId;}
    Packet* getPacket() {return packet;}
    void print() {cout << "req: " << sourceId << "->" << destinationId << " ! ";}
};



// Отклик на заявку request для передачи пакета узлу destinationId
class Response : public cMessage
{
private:
    int destinationId;      //данному узлу нужно отправить пакет, соответствующий запросу
    Request* request;

public:
    Response(int destinationId, Request* request){
        this->destinationId = destinationId;
        this->request = request;
        this->setKind(RESPONSE_FOR_REQUEST);
    }

    int getDestinationId()  {return destinationId;}
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
