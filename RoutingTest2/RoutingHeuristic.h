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


// Логика маршрутизации в один прыжок
class OneHopHeuristic : public RoutingHeuristic {
public:
    OneHopHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// Логика маршрутизации в два прыжка
class TwoHopsHeuristic : public RoutingHeuristic {
public:
    TwoHopsHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// Логика мартшутизации "тому кто позже всех видел адресат"
class LETHeuristic : public RoutingHeuristic {
public:
    LETHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// Логика мартшутизации "кто чаще всего видит адресата в течение последних нескольких дней
class MoreFrequentVisibleFHeuristic : public RoutingHeuristic {
public:
    MoreFrequentVisibleFHeuristic(RoutingDaemon* rd) : RoutingHeuristic(rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};

#endif // ROUTING_HEURISTIC_H_INCLUDED


