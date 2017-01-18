//#ifndef LEVY_MOBILITY_H
//#define LEVY_MOBILITY_H
//
//#include "INETDefs.h"
//#include "LineSegmentsMobilityBase.h"
//#include "LeviStatic.h"
//#include <fstream>
//#include <string>
//#include "math.h"
//#include "DevelopmentHelper.h"
//
//
////todo это старая реализация. Её нужно превратить в RealMobility
//
//
//
//class RealMobility;
//typedef RealMobility RealMobilityDEF;
//// из-за цикличеких определений приходиться делать так
//#include "HotSpotsAlgorithm.h"
//
//class RealMobility : public LineSegmentsMobilityBase
//{
//  protected:
//    bool nextMoveIsWait;
//
//    LeviJump  *jump;
//    LeviPause *pause;
//    double kForSpeed;
//    double roForSpeed;
//    bool movementsFinished;    // показывает окончил ли пользователь движение или нет
//
//    const char* specification; // определяет конкретную спецификацию мобильности
//                               // одна из: SimpleLevy | LevyHotSpotsRandom | LevyHotSpotsLATPCenterLogic | LevyHotSpotsLATP | LevyHotSpotsLATPPathCounts
//    HotSpotsAlgorithmDEF* hsAlgorithm;
//
//    //statistics collection
//    std::vector<simtime_t> times;
//    std::vector<double> xCoordinates;
//    std::vector<double> yCoordinates;
//
//  protected:
//    virtual int numInitStages() const { return 3; }
//    /** @brief Initializes mobility model parameters.*/
//    virtual void initialize(int stage);
//    void initializeSpecification();
//    /** @brief Overridden from LineSegmentsMobilityBase.*/
//    virtual void setTargetPosition();
//    virtual void setInitialPosition();
//    void generateNextPosition(Coord& targetPosition, simtime_t& nextChange);
//    /** @brief Overridden from LineSegmentsMobilityBase.*/
//    virtual void move();
//    virtual void finish();
//    void collectStatistics(simtime_t appearenceTime, double x, double y);
//    void saveStatistics();
//
//  public:
//    RealMobility();
//
//    Coord getLastPosition()      {return this->lastPosition;};
//    Coord getConstraintAreaMin() {return this->constraintAreaMin;};
//    Coord getConstraintAreaMax() {return this->constraintAreaMax;};
//};
//
//#endif
