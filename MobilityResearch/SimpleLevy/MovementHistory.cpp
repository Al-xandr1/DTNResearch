#include <MovementHistory.h>

MovementHistory::MovementHistory(const int nodeId) {
    NodeId = nodeId;
    wpFileName = new char[256];
    trFileName = new char[256];
    wpFileName = createFileName(wpFileName, 0, "traceOfNode", NodeId, WAYPOINTS_TYPE);
    trFileName = createFileName(trFileName, 0, "traceOfNode", NodeId, TRACE_TYPE);
}

MovementHistory::MovementHistory(const MovementHistory &another) {
    this->NodeId = another.NodeId;
    this->wpFileName = new char[256];
    this->trFileName = new char[256];
    this->wpFileName = strcpy(this->wpFileName, another.wpFileName);
    this->trFileName = strcpy(this->trFileName, another.trFileName);

    for (unsigned int i = 0; i < another.inTimes.size(); i++) inTimes.push_back(another.inTimes[i]);
    for (unsigned int i = 0; i < another.outTimes.size(); i++) outTimes.push_back(another.outTimes[i]);
    for (unsigned int i = 0; i < another.xCoordinates.size(); i++) xCoordinates.push_back(another.xCoordinates[i]);
    for (unsigned int i = 0; i < another.yCoordinates.size(); i++) yCoordinates.push_back(another.yCoordinates[i]);
}

MovementHistory::~MovementHistory() {
    myDelete(wpFileName);
    myDelete(trFileName);
}

void MovementHistory::collect(simtime_t inTime, simtime_t outTime, double x, double y) {
    inTimes.push_back(inTime);
    outTimes.push_back(outTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
}

void MovementHistory::save(const char *wpsDir, const char *trsDir) {
    //--- Write points ---
    if (outTimes.size() > 0) {
        ofstream wpFile(buildFullName(wpsDir, wpFileName));
        ofstream trFile(buildFullName(trsDir, trFileName));
        for (unsigned int i = 0; i < outTimes.size(); i++) {
            simtime_t inTime = inTimes[i];
            simtime_t outTime = outTimes[i];
            double x = xCoordinates[i];
            double y = yCoordinates[i];

            wpFile << x << "\t" << y << "\t" << inTime << "\t" << outTime << endl;
            trFile << inTime << "\t" << x << "\t" << y << endl;
        }

        wpFile.close();
        trFile.close();
    } else {
        cout << "WARNING!!! NodeId = " << NodeId << ", outTimes.size() = " << outTimes.size() << endl;
    }
}
