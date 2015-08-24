#ifndef LEVY_MOBILITY_H
#define LEVY_MOBILITY_H

#include <fstream>
#include <string>
#include <math.h>

#include "INETDefs.h"
#include "LineSegmentsMobilityBase.h"

#include "DevelopmentHelper.h"
#include "LeviStatic.h"
#include "HotSpot.h"

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

class LevyMobility : public LineSegmentsMobilityBase
{
  protected:
    bool nextMoveIsWait;

    LeviJump  *jump;
    LeviPause *pause;
    double kForSpeed;
    double roForSpeed;

    // ������� �������
    Coord currentHSMin, currentHSMax, currentHSCenter;

    bool movementsFinished;    // ���������� ������� �� ������������ �������� ��� ���

    //statistics collection
    std::vector<simtime_t> times;
    std::vector<double> xCoordinates;
    std::vector<double> yCoordinates;

  protected:
    virtual int numInitStages() const { return 3; }
    virtual void initialize(int stage);   /** @brief Initializes mobility model parameters.*/

    virtual void setTargetPosition();     /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void setInitialPosition();

    void generateNextPosition(Coord& targetPosition, simtime_t& nextChange);
    virtual bool findNextHotSpot() { return true; }  // ���� ����� ������� � ������������� � ����� ������� � �����

    virtual void move();                  /** @brief Overridden from LineSegmentsMobilityBase.*/
    virtual void finish();

    void collectStatistics(simtime_t appearenceTime, double x, double y);
    void saveStatistics();

  public:
    LevyMobility();

    Coord getLastPosition()      {return this->lastPosition;};
    Coord getConstraintAreaMin() {return this->constraintAreaMin;};
    Coord getConstraintAreaMax() {return this->constraintAreaMax;};
};

#endif
