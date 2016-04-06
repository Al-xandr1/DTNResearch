#ifndef ROUTING_HEURISTIC_H_INCLUDED
#define ROUTING_HEURISTIC_H_INCLUDED

#include <omnetpp.h>

#include "RoutingDaemon.h"

using namespace std;


class RoutingHeuristic {
protected:
    RoutingDaemon* rd;

public:
    RoutingHeuristic(RoutingDaemon* rd) {
        this->rd = rd;
    }
    virtual bool canProcess(Request* request, int& nodeForSendResponse) = 0;
    bool isSuitableTransitNeighbor(int nodeId, Request* request);
};


// ������ ������������� � ���� ������
class OneHopHeuristic : public RoutingHeuristic {
public:
    OneHopHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// ������ ������������� � ��� ������
class TwoHopsHeuristic : public RoutingHeuristic {
public:
    TwoHopsHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// ������ ������������� "���� ��� ����� ���� ����� �������"
class LETHeuristic : public RoutingHeuristic {
private:
    simtime_t trustTimeThreshold; //������ �������, � ������ �������� ����� �������� LET ���������

public:
    LETHeuristic(RoutingDaemon* rd, simtime_t trustTimeThreshold) : RoutingHeuristic(rd) {
        this->trustTimeThreshold = trustTimeThreshold;
    };
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// ������ ������������� "��� ���� ����� ����� �������� � ������� ��������� ���������� ����
class MoreFrequentVisibleHeuristic : public RoutingHeuristic {
public:
    MoreFrequentVisibleHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};

#endif // ROUTING_HEURISTIC_H_INCLUDED


