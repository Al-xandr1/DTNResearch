#ifndef STATISTICSCOLLECTOR2_H_
#define STATISTICSCOLLECTOR2_H_

/* логически модуль сбора статистики зависит
 * от модуля сбора истории (как минимум по формату файлов)
 */
#include <HistoryCollector.h>

using namespace std;

class StatisticsCollector2 : public cSimpleModule {
private:
    xml_document *packetsHistoryDoc; // заргуженный xml документ с историей пакетов
    xml_document *ictHistoryDoc;     // заргуженный xml документ с историей ICT
    xml_document *routeHistoryDoc;   // заргуженный xml документ с историей о маршрутах узлов

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
