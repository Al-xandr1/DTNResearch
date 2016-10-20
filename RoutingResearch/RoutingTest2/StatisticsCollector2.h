#ifndef STATISTICSCOLLECTOR2_H_
#define STATISTICSCOLLECTOR2_H_

/* ��������� ������ ����� ���������� �������
 * �� ������ ����� ������� (��� ������� �� ������� ������)
 */
#include <HistoryCollector.h>

using namespace std;


class StatisticsCollector2 : public cSimpleModule {
private:
    //ifstream* packetsHistoryFile;   // ���� � ����������� � ���� �������
    cXMLElement *packetsHistoryDoc; // ����������� xml �������� � �������� �������

    //ifstream* ictHistoryFile;       // ���� � ����������� � ������� �������������� �����
    cXMLElement *ictHistoryDoc;     // ����������� xml �������� � �������� �������

    unsigned int createdPackes;
    unsigned int deliveredPackets;

    cDoubleHistogram* lifeTimePDF;
    cDoubleHistogram* ictPDF;

protected:
    virtual void initialize();
    void processPacketHistory();
    void processICTHistory();
    virtual void handleMessage(cMessage *msg) {};
    virtual void finish();
};

#endif /* STATISTICSCOLLECTOR2_H_ */
