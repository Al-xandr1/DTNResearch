#ifndef HISTORYCOLLECTOR_H_
#define HISTORYCOLLECTOR_H_

#include <iostream>
#include <fstream>
#include <omnetpp.h>
#include <Messages.h>
#include <RoutingDaemon.h>
#include "DevelopmentHelper.h"

// Коды сообытий, происходящими с сообщениями
#define CREATED_EVENT        "CRT"    // событие создания пакета в системе
#define REGISTERED_EVENT     "RGS"    // событие регистрации пакета на узле (например, в результате получения или после создания)
#define BEFORE_SEND_EVENT    "BFS"    // событие перед отправкой события на какой-либо узел
#define REMOVED_EVENT        "RMV"    // событие удаления пакета из системы БЕЗ доставки до получателя
#define DELIVERED_EVENT      "DLV"    // событие о доставке сообщения и его последующем удалении

// Для xml текста
#define DLM                  "  "     // DELIMETER - разделитель значений в xml тексе
#define TAB                  "\t"     // TAB - табуляция для отсутпа в xml тексе


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
    static ofstream* packetsHistoryFile;  // файл с информацией о всех пакетах
    static ofstream* ictHistoryFile;      // файл с информацией о времени взаимодействия узлов
    static ofstream* routeHistoryFile;    // файл с информацией о пройденных маршрутах узлов
    static RoutingDaemon* rd;

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
    static void collectPacket(ofstream* out, Packet* packet);
    static void write(Packet* packet, ostream* out);
    static void write(simtime_t ict, ostream* out);
};

#endif /* HISTORYCOLLECTOR_H_ */
