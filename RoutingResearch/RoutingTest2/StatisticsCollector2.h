#ifndef STATISTICSCOLLECTOR2_H_
#define STATISTICSCOLLECTOR2_H_

/* логически модуль сбора статистики зависит
 * от модуля сбора истории (как минимум по формату файлов)
 */
#include <HistoryCollector.h>

using namespace std;


class StatisticsCollector2 : public cSimpleModule {
private:
    cXMLElement *packetsHistoryDoc; // заргуженный xml документ с историей пакетов
    cXMLElement *ictHistoryDoc;     // заргуженный xml документ с историей ICT
    cXMLElement *routeHistoryDoc;   // заргуженный xml документ с историей о маршрутах узлов

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
