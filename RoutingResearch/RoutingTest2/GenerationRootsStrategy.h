#ifndef GENERATIONROOTSSTRATEGY_H_
#define GENERATIONROOTSSTRATEGY_H_

#include <vector>
#include "Data.h"
#include "HotSpotsCollection.h"
#include "RootsPersistenceAndStatistics.h"


using namespace std;

/**
 * Общий класс для стратегий генерации маршрутов.
 */
class GenerationRootsStrategy {
public:
    GenerationRootsStrategy() {;
    }
    virtual ~GenerationRootsStrategy() {
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
    double rootPersistence;     // фиксированный коэффициент персистентности (проставляеся из мобильности)
    HotSpotsCollection* hsc;    // указатель на коллекция локаций (ТОЛЬКО ДЛЯ ЧТЕНИЯ) (проставляеся из мобильности)

public:
    GenerationRootsByPersistenceStrategy(double rootPersistence, HotSpotsCollection* hsc) : GenerationRootsStrategy() {
        this->rootPersistence = rootPersistence;
        this->hsc = hsc;
    };

    ~GenerationRootsByPersistenceStrategy() {
        hsc = NULL;
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

    /** Ссылка на модуль с общей персистентностью и статистикой ТОЛЬКО ДЛЯ ЧТЕНИЯ) (проставляеся из мобильности)
     */
    RootsPersistenceAndStatistics* rootStatistics;

public:
    GenerationRootsByStatisticsStrategy(RootsPersistenceAndStatistics* rootStatistics) : GenerationRootsStrategy() {
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
    int generateRootDimension();

    /**
     * Метод генерирует случайное число пропорчионально распределению summarizedIndicatorVector.
     * Данное число используется как индекс той локации, которую нужно включить в состав маршрута.
     */
    int generateHotSpotIndex();

    /**
     * todo что генерирует данный метод???
     */
    int generateHotSpotCount();
};

#endif
