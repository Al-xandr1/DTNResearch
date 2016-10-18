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

// ���� ��������, ������������� � �����������
#define CREATED_EVENT        "CRT"    // ������� �������� ������ � �������
#define REGISTERED_EVENT     "RGS"    // ������� ����������� ������ �� ���� (��������, � ���������� ��������� ��� ����� ��������)
#define BEFORE_SEND_EVENT    "BFS"    // ������� ����� ��������� ������� �� �����-���� ����
#define REMOVED_EVENT        "RMV"    // ������� �������� ������ �� ������� ��� �������� �� ����������
#define DELIVERED_EVENT      "DLV"    // ������� � �������� ��������� � ��� ����������� ��������

// ��� xml ������
#define DLM                  "  "     // DELIMETER - ����������� �������� � xml �����
#define TAB                  "\t"     // TAB - ��������� ��� ������� � xml �����


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

    //for statistics collection
    vector<char*>       eventHistory;
    vector<int>         IDhistory;
    vector<simtime_t>   timeHistory;
    vector<double>      xCoordinates;
    vector<double>      yCoordinates;
    vector<char*>       heuristicHistory;

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
    void collectCreated(int nodeId, Coord position)     {collect((char*) CREATED_EVENT,     nodeId, position);}
    void collectRegistered(int nodeId, Coord position)  {collect((char*) REGISTERED_EVENT,  nodeId, position);}
    void collectBeforeSend(int nodeId, Coord position)  {collect((char*) BEFORE_SEND_EVENT, nodeId, position);}
    void collectRemoved(int nodeId, Coord position)     {collect((char*) REMOVED_EVENT,     nodeId, position);}
    void collectDelivered(int nodeId, Coord position)   {collect((char*) DELIVERED_EVENT,   nodeId, position);}

private:

    void collect(char* event, int nodeId, Coord position) {
        eventHistory.push_back(event);
        IDhistory.push_back(nodeId);
        timeHistory.push_back(simTime());
        xCoordinates.push_back(position.x);
        yCoordinates.push_back(position.y);
        heuristicHistory.push_back((char*) (getLastHeuristric()!=NULL ? getLastHeuristric() : "NULL"));
    }

public:

    void write(ostream* out) {
        (*out) <<"<PACKET>" << endl;
        (*out) <<TAB<<"<SUMMARY>"<<sourceId<<DLM<<destinationId<<DLM<<creationTime<<DLM<<receivedTime<<"</SUMMARY>"<<endl;
        (*out) <<TAB<<"<HISTORY>"<<endl;

        for(int i=0; i<IDhistory.size(); i++)
            (*out) <<TAB<<TAB<<"<"<<eventHistory[i]<<">"<<IDhistory[i]<<DLM<<timeHistory[i]
                   <<DLM<<xCoordinates[i]<<DLM<<yCoordinates[i]<<DLM<<heuristicHistory[i]<<"</"<<eventHistory[i]<<">"<<endl;

        (*out) <<TAB<<"</HISTORY>"<<endl<<endl;
        (*out) <<"</PACKET>"<<endl;
    }

    void printHistory() {
        write(&cout);
    }
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



// ��������� � �������� ������ ������
class NewPacketCreated : public cMessage
{
public:
    NewPacketCreated(){
        this->setKind(NEW_PACKET_CREATED);
    }
};



// ��������� � ��������� ������ �����
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



// ��������� � �������� ICT ����� ������ i � j
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
        this->setKind(ICT_INFO);
    }

    int getI() {return i;}
    int getJ() {return j;}
    simtime_t getICT() {return ict;}
};



// ��������� � ������� ���������� ���� nodeIdSrc �� ����������� ������
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
