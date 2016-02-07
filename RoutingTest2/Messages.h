#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdlib.h>

#include "INETDefs.h"

using namespace std;

#define FOR_NEW_PACKET 1        // ��������� � �������� ������ ������
#define ESTABLISED_CONNECTION 2 // ��������� � ����������
#define PACKET 3                // ����������� �������� ������
#define REQUEST_FOR_ROUTING 4   // ������ �� �������������


// ����� ��� ��������
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



// ������ �� �������� ������
typedef Packet Request;



// ��������� � ������� ���������� ���� nodeIdSrc �� ����������� ������
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
