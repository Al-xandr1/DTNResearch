#ifndef LEVY_HSLATP_H
#define LEVY_HSLATP_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>

#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"

#include "DevelopmentHelper.h"
#include "LeviStatic.h"
#include "HotSpotsCollection.h"
#include "SelfSimMap.h"

class SelfSimLATP : public LineSegmentsMobilityBase
{
  protected:

    int NodeID;

    bool nextMoveIsWait;

    double kForSpeed;
    double roForSpeed;

    LeviPause *pause;

    double powAforHS;
    double powAforWP;

    bool movementsFinished;    // показывает окончил ли пользователь движение или нет

    // текущая локация
    Coord currentHSMin, currentHSMax, currentHSCenter;
    vector<Coord> waypts;
    unsigned int currentWpt;

    HotSpotsCollection* hsc;
    unsigned int currentHSindex;

    HSDistanceMatrix* hsd;

    RootCollection* rc;
    unsigned int RootNumber;
    vector <HotSpotRootInfo> currentRoot;
    bool isRootReady;

    SelfSimMapGenerator* gen;

    vector<vector<double>*> dstMatrix;
    bool isDstMatrixReady;

    bool isWptLoaded;
    vector<vector<double>*> wptMatrix;
    bool isWptMatrixReady;

    //statistics collection
    char *wpFileName;
    char *trFileName;
    simtime_t waitTime;
    std::vector<simtime_t> inTimes;
    std::vector<simtime_t> outTimes;
    std::vector<double> xCoordinates;
    std::vector<double> yCoordinates;

  protected:
    virtual int numInitStages() const { return 3; }
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setInitialPosition();

    void generateNextPosition(Coord& targetPosition, simtime_t& nextChange);
    virtual bool findNextHotSpot();       // ищем новую локацию и устанавливаем её новые границы и центр

    virtual void move();                  /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void finish();

    void collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y);
    void saveStatistics();

  public:
    SelfSimLATP();
    int getNodeID();

    Coord getLastPosition()      {return this->lastPosition;};
    Coord getConstraintAreaMin() {return this->constraintAreaMin;};
    Coord getConstraintAreaMax() {return this->constraintAreaMax;};

    void makeRoot();
    void buildDstMatrix();
    double getDistance(unsigned int i, unsigned int j);
    void loadHSWaypts();
    void buildWptMatrix();
    bool findNextWpt();
    double getWptDist(unsigned int i, unsigned int j);

    void correctMatrix(vector<vector<double>*> &matrix, unsigned int delete_Index);
};

#endif
