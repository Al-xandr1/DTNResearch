#ifndef MESSAGES_H
#define MESSAGES_H

#include <stdlib.h>
#include "INETDefs.h"
#include "Coord.h"

using namespace std;

// ���� ���������
#define FOR_NEW_PACKET       1  // ��������� ��� �������� ������ ������
#define NEW_PACKET_CREATED   2  // ��������� � ��������� ����� ������
#define RESPONSE_FOR_REQUEST 3  // ��������� � ����������
#define PACKET               4  // ����������� �������� ������
#define REQUEST_FOR_ROUTING  5  // ������ �� �������������
#define PACKET_RECEIVED      6  // ��������� � ���������� ������
#define ICT_INFO             7  // ��������� � ����������� � ICT
#define DAY_START            8  // ��������� � ������ ������ "���" �������������
#define MOBILITY_START       9  // ��������� � ������ ������������� �����������


// ����� ��� ��������
class Packet : public cPacket
{
private:
    int   sourceId;         // const
    int   lastVisitiedId;
    int   destinationId;    // const
    char* lastHeuristric;

    simtime_t creationTime; // const
    simtime_t receivedTime;
    simtime_t lastLET;      // ����� ������ �������� � ��������� ��� ��������� LET �������������

public:
    //for collection of history
    vector<char*>       eventHistory;
    vector<int>         IDhistory;
    vector<simtime_t>   timeHistory;
    vector<double>      xCoordinates;
    vector<double>      yCoordinates;
    vector<char*>       heuristicHistory;

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
};



// ������ �� �������� ������ �� ���� sourceId ���� destinationId
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



// ������ �� ������ request ��� �������� ������ ���� destinationId
class Response : public cMessage
{
private:
    int      destinationId;   //������� ���� ����� ��������� �����, ��������������� �������
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

#endif
