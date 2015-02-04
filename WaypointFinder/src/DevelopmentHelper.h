#ifndef DEVELOPMENTHELPER_H_INCLUDED
#define DEVELOPMENTHELPER_H_INCLUDED

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
    strcat(buffer, fileName);
}

vector<double>* parseDoubleVector(string str)
{
    Tokenizer tokenizer(str, "  ");
    vector<double>* dxPerLevel = new vector<double>();
    while (tokenizer.NextToken()) dxPerLevel->push_back(atof(tokenizer.GetToken().c_str()));
    return dxPerLevel;
}

#endif // DEVELOPMENTHELPER_H_INCLUDED
