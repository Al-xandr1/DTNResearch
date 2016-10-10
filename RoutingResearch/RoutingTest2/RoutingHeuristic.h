#ifndef ROUTING_HEURISTIC_H_INCLUDED
#define ROUTING_HEURISTIC_H_INCLUDED

#include <omnetpp.h>

#include "RoutingDaemon.h"

using namespace std;

// ����� ������ � ����������� ��� �������� �������������
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
        if (name) {delete name; name = NULL;}
    };

    char* getName() {return name;}
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForSendResponse) = 0;
};


// ������ ������������� � ���� ������
class OneHopHeuristic : public RoutingHeuristic {
public:
    OneHopHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic("OneHopHeuristic", rd, settings) {};
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting);
};


// ������ ������������� � ��� ������
class TwoHopsHeuristic : public RoutingHeuristic {
public:
    TwoHopsHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic("TwoHopsHeuristic", rd, settings) {};
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting);
};


// ������ ������������� "���� ��� ����� ���� ����� �������"
class LETHeuristic : public RoutingHeuristic {
public:
    LETHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic("LETHeuristic", rd, settings) {};
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting);
};


// ������ ������������� "��� ���� ����� ����� �������� � ������� ��������� ���������� ����
class MoreFrequentVisibleHeuristic : public RoutingHeuristic {
public:
    MoreFrequentVisibleHeuristic(RoutingDaemon* rd, RoutingSettings* settings) : RoutingHeuristic("MoreFrequentVisibleHeuristic", rd, settings) {};
    virtual bool canProcess(Request* request, vector<int>* neighbors, int& nodeForRouting);
};

#endif // ROUTING_HEURISTIC_H_INCLUDED


