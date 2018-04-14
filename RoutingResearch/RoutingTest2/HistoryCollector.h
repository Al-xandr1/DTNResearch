#ifndef HISTORYCOLLECTOR_H_
#define HISTORYCOLLECTOR_H_

#include <iostream>
#include <fstream>
#include <omnetpp.h>
#include <string>
#include <Messages.h>
#include <RoutingDaemon.h>
#include "DevelopmentHelper.h"

using namespace std;

// ���� ��������, ������������� � �����������
#define CREATED_EVENT        (char*) "CRT"    // ������� �������� ������ � �������
#define REGISTERED_EVENT     (char*) "RGS"    // ������� ����������� ������ �� ���� (��������, � ���������� ��������� ��� ����� ��������)
#define BEFORE_SEND_EVENT    (char*) "BFS"    // ������� ����� ��������� ������� �� �����-���� ����
#define REMOVED_EVENT        (char*) "RMV"    // ������� �������� ������ �� ������� ��� �������� �� ����������
#define DELIVERED_EVENT      (char*) "DLV"    // ������� � �������� ��������� � ��� ����������� ��������

// ��� xml ������
#define DLM                  (char*) "  "     // DELIMETER - ����������� �������� � xml �����
#define TAB                  (char*) "\t"     // TAB - ��������� ��� ������� � xml �����


class RoutingDaemon;


struct RouteInfoForNode {
public:
    unsigned int day;
    simtime_t startTimeRoute;
    simtime_t endTimeRoute;

    RouteInfoForNode(unsigned int day, simtime_t startTimeRoute, simtime_t endTimeRoute) {
        this->day = day;
        this->startTimeRoute = startTimeRoute;
        this->endTimeRoute = endTimeRoute;
    }
};


class HistoryCollector {
private:
    static ofstream* packetsHistoryFile;  // ���� � ����������� � ���� �������
    static ofstream* ictHistoryFile;      // ���� � ����������� � ������� �������������� �����
    static ofstream* routeHistoryFile;    // ���� � ����������� � ���������� ��������� �����
    static RoutingDaemon* rd;

    static unsigned int currentFilePartOfCollectedPackets;
    static unsigned int collectedPackets;
    static unsigned int createdPackets;
    static unsigned int deliveredPackets;
    // for route history: 1 dimension - nodeId, 2 - routes
    static vector<vector<RouteInfoForNode*>*>* routeHistory;

public:
    static void initialize(RoutingDaemon* rd);
    static void finish();

    static void collectPacket(Packet* packet);
    static void collectICT(simtime_t ict);

    static void insertRouteInfo(int nodeId, unsigned int day, simtime_t startTimeRoute, simtime_t endTimeRoute);
    static void insertRowCreated(Packet* packet, int nodeId, Coord position);
    static void insertRowRegistered(Packet* packet, int nodeId, Coord position);
    static void insertRowBeforeSend(Packet* packet, int nodeId, Coord position);
    static void insertRowRemoved(Packet* packet, int nodeId, Coord position);
    static void insertRowDelivered(Packet* packet, int nodeId, Coord position);

    static void printHistory(Packet* packet);

private:
    static void write(int nodeId, vector<RouteInfoForNode*>* routesForNode, ostream* out);
    static void insertRow(Packet* packet, char* event, int nodeId, Coord position);
    static bool write(Packet* packet, ostream* out);
    static void write(simtime_t ict, ostream* out);
};

#endif /* HISTORYCOLLECTOR_H_ */
