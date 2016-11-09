#ifndef STATISTICSCOLLECTOR2_H_
#define STATISTICSCOLLECTOR2_H_

/* ��������� ������ ����� ���������� �������
 * �� ������ ����� ������� (��� ������� �� ������� ������)
 */
#include <HistoryCollector.h>

using namespace std;


class StatisticsCollector2 : public cSimpleModule {
private:
    cXMLElement *packetsHistoryDoc; // ����������� xml �������� � �������� �������
    cXMLElement *ictHistoryDoc;     // ����������� xml �������� � �������� ICT
    cXMLElement *routeHistoryDoc;   // ����������� xml �������� � �������� � ��������� �����

    unsigned int createdPackes;
    unsigned int deliveredPackets;

    cDoubleHistogram* lifeTimePDF;
    cDoubleHistogram* ictPDF;

protected:
    virtual void initialize();
    void processPacketHistory();
    void processICTHistory();
    void processRouteHistory();
    virtual void handleMessage(cMessage *msg) {};
    virtual void finish();
};

#endif /* STATISTICSCOLLECTOR2_H_ */
