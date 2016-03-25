/*
 * DevelopmentHelper.h
 *
 *  Created on: 21 окт. 2014 г.
 *      Author: Alexander
 */
#ifndef DEVELOPMENTHELPER_H_INCLUDED
#define DEVELOPMENTHELPER_H_INCLUDED

#include <list>

#define FILE_TYPE ".xml"

char* buildWayPointFileName(char* name) {
    char* buffer = new char[256];
    strcpy(buffer, name);
    return strcat(buffer, ".wpt");
}


char* buildBoundsFileName(char* name) {
    char* buffer = new char[256];
    strcpy(buffer, name);
    return strcat(name, ".bnd");
}


char* buildStatisticFileName(char* name) {
    char* buffer = new char[256];
    strcpy(buffer, name);
    return strcat(name, ".stat");
}


char* buildFullName(char* dir, char* fileName) {
    char* buffer = new char[256];
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}


vector<double>* parseDoubleVector(string str)
{
    Tokenizer tokenizer(str, "  ");
    vector<double>* dxPerLevel = new vector<double>();
    while (tokenizer.NextToken()) dxPerLevel->push_back(atof(tokenizer.GetToken().c_str()));
    return dxPerLevel;
}


char* createFileName(char* buffer, int numberOfExperiment,
        const char* rawName, int index) {

    ostringstream prefix, postfix;
    char *result;

    if (numberOfExperiment > 0) {
        prefix << numberOfExperiment;

        result = strcat(strcat(strcpy(buffer, prefix.str().c_str()), "_"),
                rawName);
    } else {
        result = strcpy(buffer, rawName);
    }

    if (index >= 0) {
        postfix << index;

        result = strcat(strcat(strcat(result, "_"), ((postfix.str()).c_str())),
        FILE_TYPE);
    } else {
        result = strcat(result, FILE_TYPE);
    }

    return result;
}


int countMaxValue(list<int> queueSizePoints) {
    int maxQueueSize = 0;
    list<int>::iterator iterSize;
    for (iterSize = queueSizePoints.begin(); iterSize != queueSizePoints.end();
            ++iterSize) {
        if (maxQueueSize < (*iterSize))
            maxQueueSize = (*iterSize);
    }
    return maxQueueSize;
}

double getLength(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2)
              + (y1 - y2) * (y1 - y2));
}

#endif // DEVELOPMENTHELPER_H_INCLUDED
