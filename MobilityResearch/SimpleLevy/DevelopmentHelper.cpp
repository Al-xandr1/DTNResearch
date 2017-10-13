#include "DevelopmentHelper.h"


const char *NamesAndDirs::getOutDir() {
    return OUT_DIR;
}

const char *NamesAndDirs::getOutWpsDir() {
    const char *wpsDir = buildFullName(OUT_DIR, WPS_DIR);
    return wpsDir;
}

const char *NamesAndDirs::getOutTrsDir() {
    const char *trsDir = buildFullName(OUT_DIR, TRS_DIR);
    return trsDir;
}

const char *NamesAndDirs::getOutHsDir() {
    const char *hsDir = buildFullName(OUT_DIR, HS_DIR);
    return hsDir;
}

const char *NamesAndDirs::getOutTheoryRtDir() {
    const char *hsDir = buildFullName(OUT_DIR, TH_RT_DIR);
    return hsDir;
}

const char *NamesAndDirs::getOutActualRtDir() {
    const char *hsDir = buildFullName(OUT_DIR, AC_RT_DIR);
    return hsDir;
}

const char *NamesAndDirs::getOutLocFile() {
    const char *locFile = buildFullName(OUT_DIR, LOC_FILE);
    return locFile;
}

const char *NamesAndDirs::getOutSptCntFile() {
    const char *sptCntFile = buildFullName(OUT_DIR, SPC_FILE);
    return sptCntFile;
}

const char *buildFullName(const char *dir, const char *fileName) {
    char *buffer = new char[256];
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}

char *createFileName(char *buffer, int numberOfExperiment,
                     const char *rawName, int index, const char *fileType) {

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

string extractSimpleName(const char *fullName) {
    string fullNameStr(fullName);
    std::size_t found = fullNameStr.find_last_of((char*) "/");
    return fullNameStr.substr(found+1);
}

const char* buildIntParameter(const char* name, int value) {
    return buildParameter(name, std::to_string(value).c_str());
}

const char* buildParameter(const char* name, const char* value) {
    string parameter(name);
    parameter += string("=");
    parameter += string(value);
    parameter += string("_");
    return parameter.c_str();
}

double* extractDoubleParameter(const char* fileName, const char* parameter) {
    const char* value = extractParameter(fileName, parameter);
    if (value == NULL) {
        return NULL;
    }
    return new double(std::stod(value));
}

const char* extractParameter(const char* fileName, const char* parameter) {
    string fileNameStr(fileName);
    string parameterStr(parameter);

    std::size_t found = fileNameStr.find(parameterStr);
    if (found == std::string::npos) {
        return NULL;
    }
    std::size_t start = found + parameterStr.length();
    ASSERT(fileNameStr.at(start) == '=');

    std::size_t end = start + 1;
    while ((fileNameStr.at(end)) != '_' && end < (fileNameStr.length()-1)) end++;
    ASSERT(end < fileNameStr.length());

    return fileNameStr.substr(start+1, end-start-1).c_str();
}

double getSum(vector<double>* vector) {
    double sum = 0;
    for(unsigned int i=0; i < vector->size(); i++) sum += vector->at(i);
    return sum;
}

