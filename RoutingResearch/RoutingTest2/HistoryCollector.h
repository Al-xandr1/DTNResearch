#ifndef HISTORYCOLLECTOR_H_
#define HISTORYCOLLECTOR_H_

#include <iostream>
#include <fstream>
#include <omnetpp.h>
#include <Messages.h>
#include "DevelopmentHelper.h"

// ���� ��������, ������������� � �����������
#define CREATED_EVENT        "CRT"    // ������� �������� ������ � �������
#define REGISTERED_EVENT     "RGS"    // ������� ����������� ������ �� ���� (��������, � ���������� ��������� ��� ����� ��������)
#define BEFORE_SEND_EVENT    "BFS"    // ������� ����� ��������� ������� �� �����-���� ����
#define REMOVED_EVENT        "RMV"    // ������� �������� ������ �� ������� ��� �������� �� ����������
#define DELIVERED_EVENT      "DLV"    // ������� � �������� ��������� � ��� ����������� ��������

// ��� xml ������
#define DLM                  "  "     // DELIMETER - ����������� �������� � xml �����
#define TAB                  "\t"     // TAB - ��������� ��� ������� � xml �����


class HistoryCollector {
private:
    static ofstream* packetsHistoryFile;  // ���� � ����������� � ���� �������
    static ofstream* ictHistoryFile;      // ���� � ����������� � ������� �������������� �����

public:
    static void initialize();
    static void finish();

    static void collectDeliveredPacket(Packet* packet);
    static void collectRemovedPacket(Packet* packet);
    static void collectICT(simtime_t ict);

    static void insertRowCreated(Packet* packet, int nodeId, Coord position);
    static void insertRowRegistered(Packet* packet, int nodeId, Coord position);
    static void insertRowBeforeSend(Packet* packet, int nodeId, Coord position);
    static void insertRowRemoved(Packet* packet, int nodeId, Coord position);
    static void insertRowDelivered(Packet* packet, int nodeId, Coord position);

    static void printHistory(Packet* packet);

private:
    static void collectPacket(ofstream* out, Packet* packet);
    static void insertRow(Packet* packet, char* event, int nodeId, Coord position);
    static void write(Packet* packet, ostream* out);
    static void write(simtime_t ict, ostream* out);
};

#endif /* HISTORYCOLLECTOR_H_ */
