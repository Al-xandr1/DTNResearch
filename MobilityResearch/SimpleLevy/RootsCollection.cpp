#include "RootsCollection.h"


RootsCollection* RootsCollection::instance = NULL;            // ��������� �� singleton ������


RootsCollection* RootsCollection::getInstance()
{
    if (!instance) instance = new RootsCollection();
    return instance;
}


void RootsCollection::readRootsData(char* TracesDir, char* allRootsFile, char* rootsDir, char* filePatter)
{
    ASSERT(!RootsDataShort);
    ASSERT(!RootsData);

    RootsDataShort = new vector<RootDataShort>();
    RootsData = new vector<vector<HotSpotDataRoot>*>();

    ifstream rfile(buildFullName(TracesDir, allRootsFile));
    while(!rfile.eof()) {
        string rootinfo;
        getline(rfile, rootinfo);
        RootsDataShort->push_back(RootDataShort(rootinfo));
    }
    RootsDataShort->pop_back();

    char* rootFileNamePattern = buildFullName(rootsDir, filePatter);

    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(rootFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            char* inputFileName = buildFullName(rootsDir, f.cFileName);
            ifstream* infile = new ifstream(inputFileName);
            vector<HotSpotDataRoot>* root = new vector<HotSpotDataRoot>;
            while (!infile->eof()) {
                char hotSpotName[256];
                double Xmin, Xmax, Ymin, Ymax;
                double sumTime;
                unsigned int waypointNum;
                (*infile) >> hotSpotName >> Xmin >> Xmax >> Ymin >> Ymax >> sumTime >> waypointNum;
                root->push_back(HotSpotDataRoot(hotSpotName, Xmin, Xmax, Ymin, Ymax, sumTime, waypointNum));
            }
            infile->close();
            delete infile;
            delete[] inputFileName;
            RootsData->push_back(root);
        }
        while(FindNextFile(h, &f));
    }
    else cout << "Directory or files not found\n";
}


void RootsCollection::printRootsDataShort()
{
    for (unsigned int i=0; i<RootsDataShort->size(); i++) (*RootsDataShort)[i].print();
}


void RootsCollection::printRootsData()
{
    for(unsigned int i=0; i<RootsData->size(); i++) {
        cout << "Root " << i <<":" <<endl;
        for(unsigned int j=0; j<(*RootsData)[i]->size(); j++) (*RootsData)[i]->at(j).print();
    }
}