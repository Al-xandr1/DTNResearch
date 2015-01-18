/** \brief
    Old no-completed version of WaypointFinder. NOT FOR DIRECTLY USE!
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <math.h>
#include <queue>
#include "Reader.cpp"

using namespace std;

class AnilizeQueue {
private:
    std::queue<TracePoint>* queue;
    TracePoint* summOfPoints; //store sum of coordinates for average value AND time range between the latest point and the earliest
    TracePoint* averagePoint;

    void updateTimeRange() {
        if (queue->size() > 0) {
            summOfPoints->t = queue->back().t - queue->front().t;
        } else {
            summOfPoints->t = 0;
        }
    }

public:
    AnilizeQueue() {
        queue = new std::queue<TracePoint>();
        summOfPoints = new TracePoint(0, 0, 0);
        averagePoint = NULL;
    }

    ~AnilizeQueue() {
        delete queue;
        delete summOfPoints;
    }

    //Adds an element to the back of the queue.
    void push(TracePoint* point) {
        queue->push(point);

        summOfPoints->x += point->x;
        summOfPoints->y += point->y;
        updateTimeRange();
    }

    //Retrieves and Removes an element from the front of the queue.
    TracePoint* pop() {
        TracePoint& point = queue->front();
        queue->pop();

        summOfPoints->x -= point.x;
        summOfPoints->y -= point.y;
        updateTimeRange();
        return &point;
    }

    TracePoint getAveragePoint() {
        double averageX = summOfPoints->x / queue->size();
        double averageY = summOfPoints->y / queue->size();
        if (averagePoint)
            delete averagePoint;

        return (averagePoint = new TracePoint(summOfPoints->t, averageX, averageY));
    }

    int size() {
        return queue->size();
    }
};

// check TracePoint for enclosing in circle with specified center and radius
bool isPointInCircle(double centerX, double centerY, double radius, TracePoint* point) {
    return (pow(point->x - centerX, 2) + pow(point->y - centerY, 2)) <= radius;
}

#define RADIUS 5        //meters
#define TIMEOUT 30      //seconds

int test() {
    cout << "Start!" << endl;

    AnilizeQueue* queue = new AnilizeQueue();

    TracePointReader* reader = new TracePointReader("NewYork_30sec_001.txt");
    int row = 1;
    while (reader->hasNext()) {
        TracePoint* point = reader->next();
        queue->push(point);
    }
    delete reader;
    cout << endl;
    //todo в конце считывается 0 0 0

    cout << "   Average: x=" << queue->getAveragePoint().x << ", y="
            << queue->getAveragePoint().y << ", timeRange="
            << queue->getAveragePoint().t << endl;

    int size = queue->size();
    for (int i = 0; i < size; i++) {
        TracePoint* point = queue->pop();
        cout << row++ << "  " << point->t << "  " << point->x << "  "
                << point->y << endl;
    }

    cout << "   Average: x=" << queue->getAveragePoint().x << ", y="
            << queue->getAveragePoint().y << ", timeRange="
            << queue->getAveragePoint().t << endl;

    cout << "End.";
    return 0;
}
