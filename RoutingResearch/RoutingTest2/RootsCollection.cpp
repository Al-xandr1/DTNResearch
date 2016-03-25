#include "RootsCollection.h"

bool RootsCollection::isRootDataReady = false;
vector<RootShortInfo> RootsCollection::RootData;


void RootsCollection::readRootsInfo(char* TracesDir, char* rootsfile)
{
    ifstream rfile(buildFullName(TracesDir, rootsfile));
    while(!rfile.eof()) {
        string rootinfo;
        getline(rfile, rootinfo);
        RootData.push_back(RootShortInfo(rootinfo));
    }
    RootData.pop_back();
    isRootDataReady = true;
}

void RootsCollection::print()
{
    for (unsigned int i=0; i<RootData.size(); i++) RootData[i].print();
}

