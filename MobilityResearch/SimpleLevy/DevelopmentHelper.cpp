#include "DevelopmentHelper.h"


char* NamesAndDirs::getOutDir() {
    return OUT_DIR;
}

char* NamesAndDirs::getWpsDir() {
    char *wpsDir = buildFullName(OUT_DIR, WPS_DIR);
    return wpsDir;
}

char* NamesAndDirs::getTrsDir() {
    char *trsDir = buildFullName(OUT_DIR, TRS_DIR);
    return trsDir;
}

char* NamesAndDirs::getHsDir() {
    char *hsDir = buildFullName(OUT_DIR, HS_DIR);
    return hsDir;
}

char* NamesAndDirs::getLocFile() {
    char *locFile = buildFullName(OUT_DIR, LOC_FILE);
    return locFile;
}

char* NamesAndDirs::getSptCntFile() {
    char *sptCntFile = buildFullName(OUT_DIR, SPC_FILE);
    return sptCntFile;
}



char* buildFullName(char* dir, char* fileName) {
    char* buffer = new char[256];
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}

char* createFileName(char* buffer, int numberOfExperiment,
        const char* rawName, int index, const char* fileType) {

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

        result = strcat(strcat(result, "_"), ((postfix.str()).c_str()));
    }

    if (fileType != NULL) {
        result = strcat(result, fileType);
    }

    return result;
}

int countMaxValue(list<int> queueSizePoints) {
    int maxQueueSize = 0;
    list<int>::iterator iterSize;
    for (iterSize = queueSizePoints.begin(); iterSize != queueSizePoints.end(); ++iterSize) {
        if (maxQueueSize < (*iterSize))
            maxQueueSize = (*iterSize);
    }
    return maxQueueSize;
}

double getLength(double x1, double y1, double x2, double y2) {
    return sqrt((x1 - x2) * (x1 - x2)
              + (y1 - y2) * (y1 - y2));
}



