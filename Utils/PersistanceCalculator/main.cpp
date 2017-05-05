#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <math.h>

using namespace std;

char* buildFullName(char* buffer, char* dir, char* fileName)
{
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}


class PersistanceCalculator{
protected:
    vector<char*> spotNames;
    vector<vector<int>*> roots;
    double coefPers;

public:
    PersistanceCalculator(char* SpotDir, char* RootDi);
   ~PersistanceCalculator();

    void CalculatePersistance(int etalonRootNum);
    double CoefficientOfSimilarity(vector<int>* root1, vector<int>* root2);
    void SaveResults();
};

PersistanceCalculator::PersistanceCalculator(char* SpotDir, char* RootDir)
{
    //загружаем локации
    char SpotNamePattern[256];
    buildFullName(SpotNamePattern, SpotDir, "*.hts");

    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(SpotNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            char* sname=new char[256];
            strcpy(sname, f.cFileName);
            spotNames.push_back(sname);
        } while(FindNextFile(h, &f));
    }

    //загружаем маршруты
    char RootNamePattern[256];
    buildFullName(RootNamePattern, RootDir, "*.rot");

    char spot[256]; double time = 0; int points = 0;
    double Xmin, Xmax, Ymin, Ymax;

    WIN32_FIND_DATA f2;
    HANDLE h2 = FindFirstFile(RootNamePattern, &f2);
    if(h2 != INVALID_HANDLE_VALUE) {
        do {
            char* name=new char[256];
            buildFullName(name, RootDir, f2.cFileName);
            ifstream* rfile= new ifstream(name);
            vector<int>* root = new vector<int>();
            for(unsigned int i=0; i<spotNames.size(); i++) root->push_back(0);
            while(!rfile->eof()) {
                (*rfile)>>spot>>Xmin>>Xmax>>Ymin>>Ymax>>time>>points;
                for(unsigned int i=0; i<spotNames.size(); i++)
                    if( strcmp(spotNames[i], spot) == 0 ) { root->at(i)++; break; }
            }
            roots.push_back(root);
            rfile->close();
        } while(FindNextFile(h2, &f2));
    }

    coefPers = 0.0;
}

PersistanceCalculator::~PersistanceCalculator()
{
    for(unsigned int i=0; i<spotNames.size(); i++) delete[] spotNames[i];
    for(unsigned int i=0; i<roots.size(); i++) delete roots.at(i);
}

void PersistanceCalculator::CalculatePersistance(int etalonRootNum)
{
    if (etalonRootNum<0 && etalonRootNum>=roots.size()) exit(-111);

    double L=roots.size();
    vector<int>* etalonRoot = roots.at(etalonRootNum);
    coefPers = 0.0;
    for(unsigned int i=0; i<roots.size(); i++)
        if (i != etalonRootNum) {
            double k = CoefficientOfSimilarity(etalonRoot, roots.at(i));
            cout<<"K("<<etalonRootNum<<","<<i<<")="<<k<<endl;
            coefPers += k;
        }

    coefPers = coefPers / (L-1);
}

#define MY_MAX(a, b) ((a>b)?a:b)

double PersistanceCalculator::CoefficientOfSimilarity(vector<int>* root1, vector<int>* root2)
{
    if(root1->size() != root2->size()) exit(-222);

    double sumDiff=0, sumOfMaxComponetns=0, k=0;
    for(unsigned int i=0; i<root1->size(); i++) {sumDiff += abs(root1->at(i) - root2->at(i));}
    for(unsigned int i=0; i<root1->size(); i++) {sumOfMaxComponetns += MY_MAX(root1->at(i), root2->at(i));}

    k = 1 - sumDiff / sumOfMaxComponetns;

    if (k > 1) {
        cout<<"sumDiff="<<sumDiff<<endl;
        cout<<"sumOfMaxComponetns="<<sumOfMaxComponetns<<endl;
        cout<<"sumDiff / sumOfMaxComponetns="<<(sumDiff/sumOfMaxComponetns)<<endl;
        cout<<endl;
        cout<<"root1: "<<endl;
        for(unsigned int i=0; i<root1->size(); i++) {
            cout<<root1->at(i)<<", ";
            if ((i+1)%35 == 0 ) cout<<endl;
        }
        cout<<endl;
        cout<<"root2: "<<endl;
        for(unsigned int i=0; i<root2->size(); i++) {
            cout<<root2->at(i)<<", ";
            if ((i+1)%35 == 0 ) cout<<endl;
        }
        cout<<endl;

        exit(-333);
    };
    return k;
}

void PersistanceCalculator::SaveResults()
{
    ofstream file("persistance.pst");
    file<<coefPers;
    file.close();

    cout<<endl;
    //for(unsigned int i=0; i<roots.size(); i++) {
    //    cout<<"root "<<i<<": ";
    //    for(unsigned int j=0; j<roots.at(i)->size(); j++)
    //    cout<<roots.at(i)->at(j)<<", ";
    //    cout<<endl;
    //}
    cout<<"coefPers="<<coefPers<<endl;
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

    PersistanceCalculator calc(hotspotFilesDir, rootFilesDir);
    calc.CalculatePersistance(0);//первый маршрут считается эталонным!
    calc.SaveResults();

    cout << endl<< "Hello world!" << endl;
    return 0;
}
