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


// Логика маршрутизации в один прыжок
class OneHopHeuristic : public RoutingHeuristic {
public:
    OneHopHeuristic(RoutingDaemon* rd) : RoutingHeuristic("OneHopHeuristic", rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// Логика маршрутизации в два прыжка
class TwoHopsHeuristic : public RoutingHeuristic {
public:
    TwoHopsHeuristic(RoutingDaemon* rd) : RoutingHeuristic("TwoHopsHeuristic", rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// Логика мартшутизации "тому кто позже всех видел адресат"
class LETHeuristic : public RoutingHeuristic {
private:
    simtime_t trustTimeThreshold; //порого времени, в рамках которого можно доверять LET эвристике

public:
    LETHeuristic(RoutingDaemon* rd, simtime_t trustTimeThreshold) : RoutingHeuristic("LETHeuristic", rd) {
        this->trustTimeThreshold = trustTimeThreshold;
    };
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// Логика мартшутизации "кто чаще всего видит адресата в течение последних нескольких дней
class MoreFrequentVisibleHeuristic : public RoutingHeuristic {
public:
    MoreFrequentVisibleHeuristic(RoutingDaemon* rd) : RoutingHeuristic("MoreFrequentVisibleHeuristic", rd) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};

#endif // ROUTING_HEURISTIC_H_INCLUDED


