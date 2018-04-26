#ifndef MOVEMENTHISTORY_H_
#define MOVEMENTHISTORY_H_

#include <fstream>
#include <string>
#include "DevelopmentHelper.h"

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
    virtual ~MovementHistory();

    char* getWpFileName() {return wpFileName;};
    char* getTrFileName() {return trFileName;};
    void collect(simtime_t inTime, simtime_t outTime, double x, double y);
    void save(const char *wpsDir, const char *trsDir);
};

#endif /* MOVEMENTHISTORY_H_ */
