#ifndef HISTORYCOLLECTOR_H_
#define HISTORYCOLLECTOR_H_

#include <iostream>
#include <fstream>
#include <omnetpp.h>
#include <Messages.h>
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


class HistoryCollector {
private:
    static ofstream* packetsHistoryFile;  // файл с информацией о всех пакетах

public:
    HistoryCollector() {};
    virtual ~HistoryCollector() {};

    static void initialize();
    static void finish();

    static void collectDeliveredPacket(Packet* packet);
    static void collectRemovedPacket(Packet* packet);

    static void insertRowCreated(Packet* packet, int nodeId, Coord position);
    static void insertRowRegistered(Packet* packet, int nodeId, Coord position);
    static void insertRowBeforeSend(Packet* packet, int nodeId, Coord position);
    static void insertRowRemoved(Packet* packet, int nodeId, Coord position);
    static void insertRowDelivered(Packet* packet, int nodeId, Coord position);

private:
    static void collectPacket(ofstream* out, Packet* packet);
    static void insertRow(Packet* packet, char* event, int nodeId, Coord position);

public:
    static void printHistory(Packet* packet);
    static void write(Packet* packet, ostream* out);
};

#endif /* HISTORYCOLLECTOR_H_ */
