#ifndef LEVY_MOBILITY_H
#define LEVY_MOBILITY_H

#include <fstream>
#include <string>
#include <math.h>

#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"

#include "DevelopmentHelper.h"
#include "LeviStatic.h"

// ���������� ���������� ������������ �����������
enum LeviMobilitySubtype { SimpleLevy = 0,              // ������� ����, ��� ������������� ���������.
                       LevyHotSpotsRandom,              // ���� � �������������� ���������, ������� ���������� ��������� �������.
                       LevyHotSpotsLATP,                // ���� � �������������� ���������, ������� ���������� �� ��������� LATP,
                                                        // � ���������� ��� ��������� �������������� ����� ������� ���������� � ������� �������� ��������.
                       LevyHotSpotsLATPCenterLogic,     // ���� � �������������� ���������, ������� ���������� �� ��������� LATP,
                                                        // � ���������� ��� ��������� �������������� ����� �������� ���������.
                       LevyHotSpotsLATPPathCounts,      // ���� � �������������� ���������, ������� ���������� �� ��������� LATP,
                                                        // � ���������� ��� ��������� �������������� ����� ������� ���������� � ������� �������� ��������
                                                        // � ����� ����������� ���������� ��������� ��������� ��� ������� ��������.
		       LevyHotSpotsWayPointsLATPPathCounts  // ���� � �������������� ���������, ������� ���������� �� ��������� LATP,
                                                        // � ���������� ��� ��������� �������������� ����� ������� ���������� � ������� �������� ��������
                                                        // � ����� ����������� ���������� ��������� ��������� ��� ������� ��������.
                                                        // � �������� ������ ��������� �� ������� ������, ����������� �������.
                     };

class SimpleLevyMobility : public LineSegmentsMobilityBase
{
  protected:
    int NodeID;

    bool isPause;
    long step;

    LeviJump  *jump;
    LeviPause *pause;
    double kForSpeed;
    double roForSpeed;

    double angle;
    double distance;
    double speed;
    Coord deltaVector;
    simtime_t travelTime;

    double powA;

    bool movementsFinished;    // ���������� ������� �� ������������ �������� ��� ���

    // ������� �������
    Coord currentHSMin, currentHSMax, currentHSCenter;
    int currentHSindex;

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

    bool isHotSpotEmpty();
    bool generateNextPosition(Coord& targetPosition, simtime_t& nextChange);
    virtual bool findNextHotSpot() { return true; }  // ���� ����� ������� � ������������� � ����� ������� � �����

    virtual void finish();

    void collectStatistics(simtime_t inTime, simtime_t outTime, double x, double y);
    void saveStatistics();

    bool isCorrectCoordinates(double x, double y);
    void log();

  public:
    SimpleLevyMobility();
    int getNodeID();

    Coord getLastPosition()      {return this->lastPosition;};
    Coord getConstraintAreaMin() {return this->constraintAreaMin;};
    Coord getConstraintAreaMax() {return this->constraintAreaMax;};
};

#endif
