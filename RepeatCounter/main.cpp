#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>

using namespace std;

char* buildFullName(char* buffer, char* dir, char* fileName)
{
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}


class RepeatFinder{
protected:
    vector<char*> spotNames;
    int* maxRepeat;

public:
    RepeatFinder(char* SpotDir);
   ~RepeatFinder();
    void CheckRoots(char* RootDir);
    void SaveResults();
};

RepeatFinder::RepeatFinder(char* SpotDir)
{
    char SpotNamePattern[256];
    buildFullName(SpotNamePattern,SpotDir, "*.hts");

    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(SpotNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            char* sname=new char[256];
            strcpy(sname, f.cFileName);
            spotNames.push_back(sname);
        } while(FindNextFile(h, &f));
    }
    maxRepeat=new int[spotNames.size()];
    for(unsigned int i=0; i<spotNames.size(); i++) maxRepeat[i]=0;
}

RepeatFinder::~RepeatFinder()
{
    for(unsigned int i=0; i<spotNames.size(); i++) delete[] spotNames[i];
}

void RepeatFinder::CheckRoots(char* RootDir)
{
    char RootNamePattern[256];
    buildFullName(RootNamePattern, RootDir, "*.rot");

    char spot[256]; double time = 0; int points = 0;
    int rp[spotNames.size()];

    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(RootNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            char* name=new char[256];
            buildFullName(name, RootDir, f.cFileName);
            ifstream* rfile= new ifstream(name);
            for(unsigned int i=0; i<spotNames.size(); i++) rp[i]=0;
            while(!rfile->eof()) {
                (*rfile)>>spot>>time>>points;
                for(unsigned int i=0; i<spotNames.size(); i++)
                    if( strcmp(spotNames[i], spot) == 0 ) { rp[i]++; break; }
            }
            rfile->close();
            for(unsigned int i=0; i<spotNames.size(); i++) if(rp[i]>maxRepeat[i]) maxRepeat[i]=rp[i];
        } while(FindNextFile(h, &f));
    }
}

void RepeatFinder::SaveResults()
{
    ofstream file("spotcount.cnt");
    for(unsigned int i=0; i<spotNames.size(); i++) file<<spotNames[i]<<"\t"<<maxRepeat[i]<<endl;
    file.close();

    cout<<endl;
    for(unsigned int i=0; i<spotNames.size(); i++) cout<<spotNames[i]<<"\t"<<maxRepeat[i]<<endl;
}


int main(int argc, char** argv)
{

    char* rootFilesDir;        //full path name of root files directory
    char* hotspotFilesDir;     //full path name of hot spot files directory
    switch(argc)
    {
    case 1 :
        rootFilesDir="./rootfiles";
        hotspotFilesDir="./hotspotfiles";
        break;
    case 2 :
        rootFilesDir=argv[1];
        hotspotFilesDir="./hotspotfiles";
        break;
    case 3 :
    default:
        rootFilesDir=argv[1];
        hotspotFilesDir=argv[2];
        break;
    }

    RepeatFinder rf(hotspotFilesDir);
    rf.CheckRoots(rootFilesDir);
    rf.SaveResults();

    cout << "Hello world!" << endl;
    return 0;
}
