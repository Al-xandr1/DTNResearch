#ifndef ROUTING_HEURISTIC_H_INCLUDED
#define ROUTING_HEURISTIC_H_INCLUDED

#include <omnetpp.h>

#include "RoutingDaemon.h"

using namespace std;


class RoutingHeuristic {
protected:
    RoutingDaemon* rd;
    char* name;

public:
    RoutingHeuristic(char* name, RoutingDaemon* rd) {
        char* buffer = new char[64];
        this->name = strcpy(buffer, name);
        this->rd = rd;
    }

    ~RoutingHeuristic() {
        if (rd) {delete rd; rd = NULL;}
        if (name) {delete name; name = NULL;}
    };

    char* getName() {return name;}
    virtual bool canProcess(Request* request, int& nodeForSendResponse) = 0;
    bool isSuitableTransitNeighbor(int nodeId, Request* request);
};


// ������ ������������� � ���� ������
class OneHopHeuristic : public RoutingHeuristic {
public:
    OneHopHeuristic(RoutingDaemon* rd) : RoutingHeuristic("OneHopHeuristic", rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// ������ ������������� � ��� ������
class TwoHopsHeuristic : public RoutingHeuristic {
public:
    TwoHopsHeuristic(RoutingDaemon* rd) : RoutingHeuristic("TwoHopsHeuristic", rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// ������ ������������� "���� ��� ����� ���� ����� �������"
class LETHeuristic : public RoutingHeuristic {
private:
    simtime_t trustTimeThreshold; //������ �������, � ������ �������� ����� �������� LET ���������

public:
    LETHeuristic(RoutingDaemon* rd, simtime_t trustTimeThreshold) : RoutingHeuristic("LETHeuristic", rd) {
        this->trustTimeThreshold = trustTimeThreshold;
    };
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// ������ ������������� "��� ���� ����� ����� �������� � ������� ��������� ���������� ����
class MoreFrequentVisibleHeuristic : public RoutingHeuristic {
public:
    MoreFrequentVisibleHeuristic(RoutingDaemon* rd) : RoutingHeuristic("MoreFrequentVisibleHeuristic", rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};

#endif // ROUTING_HEURISTIC_H_INCLUDED


