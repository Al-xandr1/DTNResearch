#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdlib.h>

#include "INETDefs.h"

using namespace std;

#define FOR_NEW_PACKET 1        // сообщение о создании нового пакета
#define ESTABLISED_CONNECTION 2 // сообщение о соединении
#define PACKET 3                // обозначение простого пакета
#define REQUEST_FOR_ROUTING 4   // заявка на маршрутизацию


// Пакет для передачи
class Packet : public cPacket
{
private:
    int nodeIdSrc;
    int nodeIdTrg;

public:
    Packet(int nodeIdSrc, int nodeIdTrg){
        this->nodeIdSrc = nodeIdSrc;
        this->nodeIdTrg = nodeIdTrg;
    }

    int getNodeIdSrc() {return nodeIdSrc;}
    int getNodeIdTrg() {return nodeIdTrg;}
};



// Заявка на передачу пакета
typedef Packet Request;



// Сообщение о наличии соединение узла nodeIdSrc со несколькими узлами
class ConnectionMessage : public cMessage
{
private:
    int nodeIdSrc;
    vector<int>* connectedTargetIds;

public:
    ConnectionMessage(int nodeIdSrc, vector<int>* connectedTargetIds){
        this->nodeIdSrc = nodeIdSrc;
        this->connectedTargetIds = connectedTargetIds;
    }

    ~ConnectionMessage() {
        if (connectedTargetIds != NULL) delete connectedTargetIds;
    }

    int getNodeIdSrc() {return nodeIdSrc;}
    vector<int>* getConnectedTargetIds() {return connectedTargetIds;}
};

#endif
