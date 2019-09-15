#ifndef MOVEMENTHISTORY_H_
#define MOVEMENTHISTORY_H_

#include <fstream>
#include <string>
#include "DevelopmentHelper.h"

/**
 * Class for storing generated traces and waypoints
 */
class MovementHistory {
private:
    //statistics collection
    int NodeId;
    char *wpFileName;
    char *trFileName;
    std::vector<simtime_t> inTimes;
    std::vector<simtime_t> outTimes;
    std::vector<double> xCoordinates;
    std::vector<double> yCoordinates;

public:
    MovementHistory(const int nodeId);
    MovementHistory(const MovementHistory &another);
    virtual ~MovementHistory();

    char* getWpFileName() {return wpFileName;};
    char* getTrFileName() {return trFileName;};
    std::vector<simtime_t>* getInTimes() {return &inTimes;};
    std::vector<simtime_t>* getOutTimes() {return &outTimes;};
    std::vector<double>* getXCoordinates() {return &xCoordinates;};
    std::vector<double>* getYCoordinates() {return &yCoordinates;};
    int getSize() {
        ASSERT(inTimes.size() == outTimes.size() && outTimes.size() == xCoordinates.size() && xCoordinates.size() == yCoordinates.size());
        return xCoordinates.size();
    }
    void collect(simtime_t inTime, simtime_t outTime, double x, double y);
    void save(const char *wpsDir, const char *trsDir);
};

#endif /* MOVEMENTHISTORY_H_ */
