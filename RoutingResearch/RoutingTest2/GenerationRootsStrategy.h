#ifndef GENERATIONROOTSSTRATEGY_H_
#define GENERATIONROOTSSTRATEGY_H_

#include <vector>
#include "Data.h"
#include "HotSpotsCollection.h"
#include "DevelopmentHelper.h"
#include "RootsPersistenceAndStatistics.h"


using namespace std;

/**
 * Общий класс для стратегий генерации маршрутов.
 */
class GenerationRootsStrategy {
protected:
    /**
     * Указатель на коллекцию локаций (ТОЛЬКО ДЛЯ ЧТЕНИЯ) (проставляется из мобильности)
     */
    HotSpotsCollection* hsc;

    /**
     * Использовать ФИКСИРОВАННЮ домашнюю локацию, или ГЕНЕРИРОВАТЬ (в зависимости от стратегии)
     */
    bool useFixedHomeLocation;

public:
    GenerationRootsStrategy(HotSpotsCollection* hsc, bool useFixedHomeLocation) {;
        this->hsc = hsc;
        this->useFixedHomeLocation = useFixedHomeLocation;
    }

    virtual ~GenerationRootsStrategy() {
        this->hsc = NULL;
    };

    /**
     * Значение параметров смотреть в описании файла заголовка RegularRootLATP.
     */
    virtual bool generateNewRoot(
            vector<HotSpotData*>* firstRoot, vector<unsigned int>* firstRootSnumber, vector<int>* firstRootCounter, vector<int>* firstRootWptsPerVisit,
            vector<HotSpotData*>*& currentRoot, vector<unsigned int>*& currentRootSnumber, vector<int>*& currentRootCounter, vector<int>*& currentRootWptsPerVisit) {
        exit(-200);
        return false;
    };
};




/**
 * Стратегия генерации маршрутов на основе коэффициента персистентности.
 */
class GenerationRootsByPersistenceStrategy : public GenerationRootsStrategy {
private:
    /**
     * Фиксированный коэффициент персистентности (проставляется из мобильности)
     */
    double rootPersistence;

public:
    GenerationRootsByPersistenceStrategy(HotSpotsCollection* hsc, double rootPersistence, bool useFixedHomeLocation)
            : GenerationRootsStrategy(hsc, useFixedHomeLocation) {
        this->rootPersistence = rootPersistence;
    };

    ~GenerationRootsByPersistenceStrategy() {
    };

    /**
     * Значение параметров смотреть в описании файла заголовка RegularRootLATP.
     */
    virtual bool generateNewRoot(
            vector<HotSpotData*>* firstRoot, vector<unsigned int>* firstRootSnumber, vector<int>* firstRootCounter, vector<int>* firstRootWptsPerVisit,
            vector<HotSpotData*>*& currentRoot, vector<unsigned int>*& currentRootSnumber, vector<int>*& currentRootCounter, vector<int>*& currentRootWptsPerVisit);
};




/**
 * Стратегия генерации маршрутов на основе статистических данных от анализа реальных трасс.
 * Связано с набором законов распределения в классе RootsPersistenceAndStatistics.
 */
class GenerationRootsByStatisticsStrategy : public GenerationRootsStrategy {
private:
    /**
     * Указатель на модуль с общей персистентностью и статистикой ТОЛЬКО ДЛЯ ЧТЕНИЯ) (проставляется из мобильности)
     * ВАЖНО: Последовательность локаций в RootsPersistenceAndStatistics расходится с последовательностью в HotSpotsCollection
     * ВАЖНО: Для маппинга индексов использовать RootsPersistenceAndStatistics::findHotSpotIndexByName & HotSpotsCollection::findHotSpotbyName
     */
    RootsPersistenceAndStatistics* rootStatistics;

public:
    GenerationRootsByStatisticsStrategy(HotSpotsCollection* hsc, RootsPersistenceAndStatistics* rootStatistics, bool useFixedHomeLocation)
            : GenerationRootsStrategy(hsc, useFixedHomeLocation) {
        this->rootStatistics = rootStatistics;
    };

    ~GenerationRootsByStatisticsStrategy() {
        this->rootStatistics = NULL;
    };

    /**
     * Значение параметров смотреть в описании файла заголовка RegularRootLATP.
     */
    virtual bool generateNewRoot(
            vector<HotSpotData*>* firstRoot, vector<unsigned int>* firstRootSnumber, vector<int>* firstRootCounter, vector<int>* firstRootWptsPerVisit,
            vector<HotSpotData*>*& currentRoot, vector<unsigned int>*& currentRootSnumber, vector<int>*& currentRootCounter, vector<int>*& currentRootWptsPerVisit);

private:

    int generate(vector<double>* pdf);

    /**
     * Метод генерирует случайное число пропорчионально распределению rootsDimensionHistogram.
     * Данное число используется для количества уникальных локаций в генерируемом маршруте (размерность маршрута)
     */
    unsigned int generateRootDimension();

    /**
     * Метод генерирует случайное число пропорчионально распределению summarizedIndicatorVector.
     * Данное число используется как индекс той локации, которую нужно включить в состав маршрута.
     *
     * ВАЖНО: возвращается это индекс в струкруте файла *.pst (последовательность в нём расходится с последовательностью в HotSpotsCollection)
     */
    unsigned int generateHotSpotIndexPST();

    /**
     * Получает количество посещений для данной локации.
     * Число берётся из просуммированного вектора от всех маршрутов, который поделён на суммарный индикаторный вектор.
     * Таким образом в каждой компоненте получается среднее количество посещений за те дни,
     * в которые было хотя бы одно посещение.
     *
     * ВАЖНО: hotSpotIndexPST - это индекс в струкруте файла *.pst (последовательность в нём расходится с последовательностью в HotSpotsCollection)
     */
    unsigned int getHotSpotCount(unsigned int hotSpotIndexPST);

    /**
     * Метод генерирует случайное число пропорчионально распределению homeHotspotHistogramPDF.
     * Данное число используется как индекс той локации, которую нужно включить в состав маршрута как домашнюю.
     *
     * ВАЖНО: возвращается это индекс в струкруте файла *.pst (последовательность в нём расходится с последовательностью в HotSpotsCollection)
     */
    unsigned int generateHomeHotSpotIndexPST();
};

#endif
