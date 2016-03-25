/*
 * DevelopmentHelper.h
 *
 *  Created on: 21 окт. 2014 г.
 *      Author: Alexander
 */
#ifndef DEVELOPMENTHELPER_H_INCLUDED
#define DEVELOPMENTHELPER_H_INCLUDED

#include <string>
#include <vector>
#include <math.h>
#include <list>
#include <sstream>
#include <string>
#include <omnetpp.h>
using namespace std;

char* buildFullName(char* dir, char* fileName);
char* createFileName(char* buffer, int numberOfExperiment,
                     const char* rawName, int index, const char* fileType);
int countMaxValue(list<int> queueSizePoints);
double getLength(double x1, double y1, double x2, double y2);

#endif // DEVELOPMENTHELPER_H_INCLUDED
