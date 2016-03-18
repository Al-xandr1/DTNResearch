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
    int nodeIdSrc;
    int nodeIdTrg;

    simtime_t creationTime;
    simtime_t receivedTime;

public:
    Packet(int nodeIdSrc, int nodeIdTrg){
        this->nodeIdSrc = nodeIdSrc;
        this->nodeIdTrg = nodeIdTrg;
        this->creationTime = 0;
        this->receivedTime = 0;
        this->setKind(PACKET);
    }

    int getNodeIdSrc() {return nodeIdSrc;}
    int getNodeIdTrg() {return nodeIdTrg;}

    void setCreationTime(simtime_t time) {creationTime = time;}
    void setReceivedTime(simtime_t time) {receivedTime = time;}

    simtime_t getCreationTime() {return creationTime;}
    simtime_t getReceivedTime() {return receivedTime;}

    simtime_t getLiveTime() {return receivedTime - creationTime;}
};



// Заявка на передачу пакета
class Request : public cMessage
{
private:
    int nodeIdSrc;
    int nodeIdTrg;
    Packet* packet;

public:
    Request(int nodeIdSrc, int nodeIdTrg, Packet* packet){
        this->nodeIdSrc = nodeIdSrc;
        this->nodeIdTrg = nodeIdTrg;
        this->packet = packet;
        this->setKind(REQUEST_FOR_ROUTING);
    }

    int getNodeIdSrc()  {return nodeIdSrc;}
    int getNodeIdTrg()  {return nodeIdTrg;}
    Packet* getPacket() {return packet;}
};



// Отклик на заявку на передачу пакета
class Response : public cMessage
{
private:
    int nodeIdTrg;      //данному узлу нужно отправить пакет, соответствующий запросу
    Request* request;

public:
    Response(int nodeIdTrg, Request* request){
        this->nodeIdTrg = nodeIdTrg;
        this->request = request;
        this->setKind(RESPONSE_FOR_REQUEST);
    }

    int getNodeIdTrg()  {return nodeIdTrg;}
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
