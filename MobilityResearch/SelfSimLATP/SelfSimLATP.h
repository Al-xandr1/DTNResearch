#ifndef SELFSIM_LATP_H
#define SELFSIM_LATP_H

#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"

#include "LeviStatic.h"
#include "Data.h"
#include "HotSpotsCollection.h"
#include "RootsCollection.h"
#include "SelfSimMap.h"
#include "Messages.h"
#include "DevelopmentHelper.h"

class SelfSimLATP : public LineSegmentsMobilityBase {
protected:

    int NodeID;

    bool isPause;
    long step;

    double kForSpeed;
    double roForSpeed;

    LeviPause *pause;

    double powAforHS;
    double powAforWP;

    bool movementsFinished;    // показывает окончил ли пользователь движение или нет

    // текущая локация
    Coord currentHSMin, currentHSMax, currentHSCenter;
    vector<Coord *> *waypts;
    unsigned int currentWpt;

    HotSpotsCollection *hsc;
    int currentHSindex;

    HSDistanceMatrix *hsd;

    RootsCollection *rc;
    unsigned int RootNumber;
    vector<HotSpotData *> *currentRoot;
    bool isRootReady;

    SelfSimMapGenerator *gen;

    vector<vector<double> *> dstMatrix;
    bool isDstMatrixReady;

    bool isWptLoaded;
    vector<vector<double> *> wptMatrix;
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
    virtual void handleMessage(cMessage *message);

    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setInitialPosition();

    bool generateNextPosition(Coord &targetPosition, simtime_t &nextChange);

    virtual bool findNextHotSpot();       // ищем новую локацию и устанавливаем её новые границы и центр

    virtual void finish() { saveStatistics(); };

    void collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y);

    void saveStatistics();

public:
    SelfSimLATP();

    int getNodeID() { return NodeID; };

    Coord getLastPosition() { return this->lastPosition; };

    Coord getConstraintAreaMin() { return this->constraintAreaMin; };

    Coord getConstraintAreaMax() { return this->constraintAreaMax; };

    void setCurrentHSindex(int hsIndex);

    void makeNewRoot();

    void makeRoot();

    void buildDstMatrix();

    double getDistance(unsigned int i, unsigned int j);

    void loadHSWaypts();

    void buildWptMatrix();

    bool findNextWpt();

    double getWptDist(unsigned int i, unsigned int j);

    void correctMatrix(vector<vector<double> *> &matrix, unsigned int delete_Index);
};

#endif
