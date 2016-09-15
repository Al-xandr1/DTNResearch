#ifndef ROUTING_HEURISTIC_H_INCLUDED
#define ROUTING_HEURISTIC_H_INCLUDED

#include <omnetpp.h>

#include "RoutingDaemon.h"

using namespace std;

// ����� ������ � ����������� ��� �������� �������������
class RoutingSettings {
private:
    simtime_t trustTimeThresholdLow;
    simtime_t trustTimeThresholdHigh;

public:
    RoutingSettings() {};
    virtual ~RoutingSettings() {};

    simtime_t getTrustTimeThresholdLow() {return trustTimeThresholdLow;};
    void setTrustTimeThresholdLow(simtime_t trustTimeThresholdLow) {
        this->trustTimeThresholdLow = trustTimeThresholdLow;
    };

    simtime_t getTrustTimeThresholdHigh() {return trustTimeThresholdHigh;};
    void setTrustTimeThresholdHigh(simtime_t trustTimeThresholdHigh) {
        this->trustTimeThresholdHigh = trustTimeThresholdHigh;
    };

    simtime_t determinTrustTimeThreshold(char* heuristricName) {
        simtime_t trustTimeThreshold = trustTimeThresholdHigh;
        if (heuristricName != NULL && strcmp(heuristricName, "MoreFrequentVisibleHeuristic") == 0) {
            trustTimeThreshold = trustTimeThresholdLow;
        }
        return trustTimeThreshold;
    }
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
        if (name) {delete name; name = NULL;}
    };

    char* getName() {return name;}
    virtual bool canProcess(Request* request, int& nodeForSendResponse) = 0;
    bool isSuitableTransitNeighbor(int nodeId, Request* request);
};


// ������ ������������� � ���� ������
class OneHopHeuristic : public RoutingHeuristic {
public:
    OneHopHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic("OneHopHeuristic", rd, settings) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// ������ ������������� � ��� ������
class TwoHopsHeuristic : public RoutingHeuristic {
public:
    TwoHopsHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic("TwoHopsHeuristic", rd, settings) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// ������ ������������� "���� ��� ����� ���� ����� �������"
class LETHeuristic : public RoutingHeuristic {
public:
    LETHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic("LETHeuristic", rd, settings) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};


// ������ ������������� "��� ���� ����� ����� �������� � ������� ��������� ���������� ����
class MoreFrequentVisibleHeuristic : public RoutingHeuristic {
public:
    MoreFrequentVisibleHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic("MoreFrequentVisibleHeuristic", rd, settings) {};
    virtual bool canProcess(Request* request, int& nodeForRouting);
};

#endif // ROUTING_HEURISTIC_H_INCLUDED


