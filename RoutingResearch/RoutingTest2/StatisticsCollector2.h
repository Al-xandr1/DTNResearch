#ifndef STATISTICSCOLLECTOR2_H_
#define STATISTICSCOLLECTOR2_H_

/* ��������� ������ ����� ���������� �������
 * �� ������ ����� ������� (��� ������� �� ������� ������)
 */
#include <HistoryCollector.h>

using namespace std;

class StatisticsCollector2 : public cSimpleModule {
private:
    xml_document *packetsHistoryDoc; // ����������� xml �������� � �������� �������
    xml_document *ictHistoryDoc;     // ����������� xml �������� � �������� ICT
    xml_document *routeHistoryDoc;   // ����������� xml �������� � �������� � ��������� �����

    unsigned int createdPackets;
    unsigned int deliveredPackets;

    cDoubleHistogram* lifeTimePDF;
    cDoubleHistogram* ictPDF;
    cDoubleHistogram* commonRoutesDurationPDF;

    vector<cDoubleHistogram*>* routesDurationPDFbyNode;

protected:
    virtual void initialize();
    void processPacketHistory();
    void processICTHistory();
    void processRouteHistory();
    void write(cDoubleHistogram* routesDurationPDF, ofstream* out, int level);
    virtual void handleMessage(cMessage *msg) {};
    virtual void finish();
};

#endif /* STATISTICSCOLLECTOR2_H_ */
