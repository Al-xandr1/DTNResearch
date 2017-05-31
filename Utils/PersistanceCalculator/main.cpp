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

public:
    PersistanceCalculator(char* SpotDir, char* RootDi);
   ~PersistanceCalculator();

    double CalculatePersistance(int etalonRootNum);
    double CalculatePersistance(vector<int>* etalonRoot, char* rootName);
    double CoefficientOfSimilarity(vector<int>* root1, vector<int>* root2);
    vector<int>* GetMassCenter();
    void CalcAllAndSave();
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
}

PersistanceCalculator::~PersistanceCalculator()
{
    for(unsigned int i=0; i<spotNames.size(); i++) delete[] spotNames[i];
    for(unsigned int i=0; i<roots.size(); i++) delete roots.at(i);
}

/**
    Расчёт коэффициента персистентности относительно какого либо маршрута из первоначального набора
*/
double PersistanceCalculator::CalculatePersistance(int etalonRootNum)
{
    if (etalonRootNum<0 && etalonRootNum>=roots.size()) exit(-111);

    vector<int>* etalonRoot = roots.at(etalonRootNum);
    double coef = 0.0;
    for(unsigned int i=0; i<roots.size(); i++)
        if (i != etalonRootNum) {
            double k = CoefficientOfSimilarity(etalonRoot, roots.at(i));
            cout<<"K("<<etalonRootNum<<","<<i<<")="<<k<<endl;
            coef += k;
        }

    return coef / (roots.size()-1); // roots.size() = L
}

/**
    Расчёт коэффициента персистентности относительно маршрута НЕ из первоначального набора
*/
double PersistanceCalculator::CalculatePersistance(vector<int>* etalonRoot, char* rootName)
{
    double coef = 0.0;
    for(unsigned int i=0; i<roots.size(); i++) {
        double k = CoefficientOfSimilarity(etalonRoot, roots.at(i));
        cout<<"K("<<rootName<<","<<i<<")="<<k<<endl;
        coef += k;
    }

    return coef / (roots.size()-1); // roots.size() = L
}

#define MY_MAX(a, b) ((a>b)?a:b)

double PersistanceCalculator::CoefficientOfSimilarity(vector<int>* root1, vector<int>* root2)
{
    if(root1->size() != root2->size()) exit(-222);

    double sumDiff=0, sumOfMaxComponetns=0, k=0;
    for(unsigned int i=0; i<root1->size(); i++) {sumDiff += abs(root1->at(i) - root2->at(i));}
    for(unsigned int i=0; i<root1->size(); i++) {sumOfMaxComponetns += MY_MAX(root1->at(i), root2->at(i));}

    k = 1 - sumDiff / sumOfMaxComponetns;

    if (k > 1) { // for debug
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

/**
    Расчёт центра масс с многомерном пространстве, размерности spotNames.size()
    и векторами roots. Пространство натуральных чисел!!!
*/
vector<int>* PersistanceCalculator::GetMassCenter()
{
    vector<int>* massCenter = new vector<int>();
    for (unsigned int i=0; i<roots.at(0)->size(); i++) {
        double component = 0.0;
        for (unsigned int j=0; j<roots.size(); j++) {
            vector<int>* root = roots.at(j);
            component += root->at(i);
        }
        component /= (1.0 * roots.size());
        massCenter->push_back(int(component + 0.5));
    }

    return massCenter;
}

/**
    Расчёт коэффициентов персистентности относительно всех маршрутов
    и сохранение результатов
*/
void PersistanceCalculator::CalcAllAndSave()
{
    ofstream file("persistance.pst");
    cout<<"Coefficients: "<<endl;

    double averagePersistance = 0.0;
    for (unsigned int i=0; i<roots.size(); i++) {
        double persistance = CalculatePersistance(i);
        file<<i<<"\t"<<persistance<<endl;
        cout<<"\t"<<i<<"\t"<<persistance<<endl<<endl;
        averagePersistance += persistance;
    }

    averagePersistance /= roots.size();
    file<<"averagePersis"<<"\t"<<averagePersistance<<endl;
    cout<<"averagePersis"<<"\t"<<averagePersistance<<endl<<endl;

    vector<int>* massCenter = GetMassCenter();
    double persistance = CalculatePersistance(massCenter, "massCenter");
    file<<"massCenter"<<"\t"<<persistance<<endl;
    cout<<"\t"<<"massCenter"<<"\t"<<persistance<<endl<<endl;
    file<<"massCenterVector"<<"\t";
    cout<<"massCenterVector"<<"\t";
    for (unsigned int i=0; i<massCenter->size(); i++) {
        file<<massCenter->at(i)<<" ";
        cout<<massCenter->at(i)<<" ";
    }
    file.close();
    cout<<endl<<endl<<endl;

    // for debug
    //for(unsigned int i=0; i<roots.size(); i++) {
    //    cout<<"root "<<i<<": ";
    //    for(unsigned int j=0; j<roots.at(i)->size(); j++)
    //    cout<<roots.at(i)->at(j)<<", ";
    //    cout<<endl;
    //}
    //cout<<"massCenter: ";
    //for(unsigned int j=0; j<massCenter->size(); j++)
    //cout<<massCenter->at(j)<<", ";
    //cout<<endl;
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
    calc.CalcAllAndSave();

    cout << endl<< "Hello world!" << endl;
    return 0;
}
