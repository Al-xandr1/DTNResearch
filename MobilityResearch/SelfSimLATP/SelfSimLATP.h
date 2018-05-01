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
#include "Movement.h"
#include "SelfSimMap.h"
#include "Messages.h"
#include "MovementHistory.h"
#include "DevelopmentHelper.h"

class SelfSimLATP : public LineSegmentsMobilityBase {
protected:
    int NodeID;

    bool isPause;
    long step;
    bool movementsFinished;    // показывает окончил ли пользователь движение или нет
    double powAforHS;
    double powAforWP;

    Movement *movement;

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
    MovementHistory* mvnHistory;

protected:
    virtual int numInitStages() const { return 3; }

    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    void setCurrentWpt(unsigned int i);
    unsigned int getCurrentWpt();

    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setInitialPosition();

    bool generateNextPosition(Coord &targetPosition, simtime_t &nextChange);

    virtual bool findNextHotSpot();       // ищем новую локацию и устанавливаем её новые границы и центр

    virtual void finish() { saveStatistics(); };

    void log(string log);

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

    virtual void makeRoot();

    void buildDstMatrix();

    double getDistance(unsigned int i, unsigned int j);

    void loadHSWaypts();

    void buildWptMatrix();

    bool findNextWpt();

    double getWptDist(unsigned int i, unsigned int j);

    void correctMatrix(vector<vector<double> *> &matrix, unsigned int delete_Index);
};

#endif
