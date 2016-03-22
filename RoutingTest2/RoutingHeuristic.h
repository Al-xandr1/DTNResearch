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
};


// ������ ������������� � ���� ������
class OneHopHeuristic : public RoutingHeuristic {
public:
    OneHopHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForSendResponse);
};


// ������ ������������� � ��� ������
class TwoHopsHeuristic : public RoutingHeuristic {
public:
    TwoHopsHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForSendResponse);
};


// ������ ������������� "���� ��� ����� ���� ����� �������"
class LETHeuristic : public RoutingHeuristic {
public:
    LETHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForSendResponse);
};


// ������ ������������� "��� ���� ����� ����� �������� � ������� ��������� ���������� ����
class MoreFrequentVisibleFHeuristic : public RoutingHeuristic {
public:
    MoreFrequentVisibleFHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForSendResponse);
};

#endif // ROUTING_HEURISTIC_H_INCLUDED


