#ifndef STATISTICSCOLLECTOR2_H_
#define STATISTICSCOLLECTOR2_H_

/* логически модуль сбора статистики зависит
 * от модуля сбора истории (как минимум по формату файлов)
 */
#include <HistoryCollector.h>

using namespace std;


class StatisticsCollector2 : public cSimpleModule {
private:
    //ifstream* packetsHistoryFile;   // файл с информацией о всех пакетах
    cXMLElement *packetsHistoryDoc; // заргуженный xml документ с историей пакетов

    //ifstream* ictHistoryFile;       // файл с информацией о времени взаимодействия узлов
    cXMLElement *ictHistoryDoc;     // заргуженный xml документ с историей пакетов

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
