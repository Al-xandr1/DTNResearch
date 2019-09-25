#ifndef ROUTING_HEURISTIC_H_INCLUDED
#define ROUTING_HEURISTIC_H_INCLUDED

#include <omnetpp.h>
#include "RoutingDaemon.h"
#include "DevelopmentHelper.h"

#define OH_HEURISTIC    (char*)"OneHopHeuristic"
#define TH_HEURISTIC    (char*)"TwoHopsHeuristic"
#define LET_HEURISTIC   (char*)"LETHeuristic"
#define MFV_HEURISTIC   (char*)"MoreFrequentVisibleHeuristic"
#define PROPHET         (char*)"PROPHET_Heuristic"

using namespace std;

// Общий объект с настройками для эвристик маршрутизации
class RoutingSettings {
private:
    simtime_t LET_Threshold;

public:
    RoutingSettings() {};
    virtual ~RoutingSettings() {};

    simtime_t getLET_Threshold() {return LET_Threshold;};
    void setLET_Threshold(simtime_t Threshold) {this->LET_Threshold = Threshold; };
};


class RoutingHeuristic {
protected:
    char* name;
    RoutingDaemon* rd;
    RoutingSettings* settings;

public:
    RoutingHeuristic(char* name, RoutingDaemon* rd, RoutingSettings* settings) {
        char* buffer = new char[64];
        this->name = strcpy(buffer, name);
        this->rd = rd;
        this->settings = settings;
    }

    ~RoutingHeuristic() {
        myDeleteArray(name);
        rd = NULL;
        settings = NULL;
    };

    char* getName() {return name;}
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForSendResponse) = 0;
};


// Логика маршрутизации в один прыжок
class OneHopHeuristic : public RoutingHeuristic {
public:
    OneHopHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic(OH_HEURISTIC, rd, settings) {};
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting);
};


// Логика маршрутизации в два прыжка
class TwoHopsHeuristic : public RoutingHeuristic {
public:
    TwoHopsHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic(TH_HEURISTIC, rd, settings) {};
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting);
};


// Логика мартшутизации "тому кто позже всех видел адресат"
class LETHeuristic : public RoutingHeuristic {
public:
    LETHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic(LET_HEURISTIC, rd, settings) {};
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting);
};


// Логика мартшутизации "кто чаще всего видит адресата в течение последних нескольких дней
class MoreFrequentVisibleHeuristic : public RoutingHeuristic {
public:
    MoreFrequentVisibleHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic(MFV_HEURISTIC, rd, settings) {};
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting);
};

// PROPHET -------------------------------------------------------------------------------
class PROPHET_Heuristic : public RoutingHeuristic {
public:
    PROPHET_Heuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic(PROPHET, rd, settings) {};
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting);

};
// ----------------------------------------------------------------------------------------

#endif // ROUTING_HEURISTIC_H_INCLUDED


