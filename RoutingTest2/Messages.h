#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdlib.h>

#include "INETDefs.h"

using namespace std;

#define FOR_NEW_PACKET 1        // ��������� ��� �������� ������ ������
#define NEW_PACKET_CREATED 2    // ��������� � ��������� ����� ������
#define ESTABLISED_CONNECTION 3 // ��������� � ����������
#define PACKET 4                // ����������� �������� ������
#define REQUEST_FOR_ROUTING 5   // ������ �� �������������
#define PACKET_RECEIVED 6       // ��������� � ���������� ������
#define ICT_INFO 7              // ��������� � ����������� � ICT


// ����� ��� ��������
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
    }

    int getNodeIdSrc() {return nodeIdSrc;}
    int getNodeIdTrg() {return nodeIdTrg;}

    void setCreationTime(simtime_t time) {creationTime = time;}
    void setReceivedTime(simtime_t time) {receivedTime = time;}

    simtime_t getCreationTime() {return creationTime;}
    simtime_t getReceivedTime() {return receivedTime;}

    simtime_t getLiveTime() {return receivedTime - creationTime;}
};



// ������ �� �������� ������
typedef Packet Request;


// ��������� � ��������� ����� �����
class PacketReceived : public cMessage
{
private:
    simtime_t liveTime;

public:
    PacketReceived(simtime_t liveTime){
        this->liveTime = liveTime;
    }

    simtime_t getLiveTime() {return liveTime;}
};



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



class ICTMessage : public cMessage
{
private:
    int i; // id ������ ����
    int j; // id ������� ����
    simtime_t ict;

public:
    ICTMessage(int i, int j, simtime_t ict) {
        this->i = i;
        this->j = j;
        this->ict = ict;
    }

    int getI() {return i;}
    int getJ() {return j;}
    simtime_t getICT() {return ict;}
};

#endif
