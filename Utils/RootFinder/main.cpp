#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <math.h>
#include <queue>
#include <vector>
#include <windows.h>

using namespace std;

struct Waypoint{
    double X,Y,Tb,Te;
    char* traceName;
};

struct HotSpotVisit {
    double Tb, Te;
    double Xmin, Xmax, Ymin, Ymax;
    char* HotSpot;
};

char* buildFullName(char* buffer, char* dir, char* fileName)
{
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}

// -----------------------------------------------------------------------------------------------------
// –еализаци€ двоичной кучи дл€ хранени€ путевых точек в пор€дке возрастани€ времени прихода

class VisitMinHeap
{
private:
    vector<HotSpotVisit> visit;

    void BubbleDown(int index);
    void BubbleUp(int index);

public:

    void Insert(HotSpotVisit& newValue);
    bool GetMin(HotSpotVisit& Value);
    void DeleteMin();
};

void VisitMinHeap::BubbleDown(int index)
{
    int length = visit.size();
    int leftChildIndex = 2*index + 1;
    int rightChildIndex = 2*index + 2;

    if(leftChildIndex >= length)
        return; //index is a leaf

    int minIndex = index;

    if( (visit[index]).Tb > (visit[leftChildIndex]).Tb)
    {
        minIndex = leftChildIndex;
    }

    if((rightChildIndex < length) && ( (visit[minIndex]).Tb > (visit[rightChildIndex]).Tb))
    {
        minIndex = rightChildIndex;
    }

    if(minIndex != index)
    {
        //need to swap
        HotSpotVisit temp = visit[index];
        visit[index] = visit[minIndex];
        visit[minIndex] = temp;
        BubbleDown(minIndex);
    }
}

void VisitMinHeap::BubbleUp(int index)
{
    if(index == 0)
        return;

    int parentIndex = (index-1)/2;

    if( (visit[parentIndex]).Tb > (visit[index]).Tb )
    {
        HotSpotVisit temp = visit[parentIndex];
        visit[parentIndex] = visit[index];
        visit[index] = temp;
        BubbleUp(parentIndex);
    }
}

void VisitMinHeap::Insert(HotSpotVisit& newValue)
{
    int length = visit.size();
    visit.push_back(newValue);

    BubbleUp(length);
}

bool VisitMinHeap::GetMin(HotSpotVisit& Value)
{
    if( visit.size() == 0) return false;

    Value.Tb=(visit[0]).Tb;
    Value.Te=(visit[0]).Te;
    Value.Xmin=(visit[0]).Xmin;
    Value.Xmax=(visit[0]).Xmax;
    Value.Ymin=(visit[0]).Ymin;
    Value.Ymax=(visit[0]).Ymax;
    strcpy(Value.HotSpot, (visit[0]).HotSpot);
    return true;
}

void VisitMinHeap::DeleteMin()
{
    int length = visit.size();

    if(length == 0)
    {
        return;
    }

    visit[0] = visit[length-1];
    visit.pop_back();

    BubbleDown(0);
}

// -------------------------------------------------------------------------------------------------

class RootFinder {
protected:
    vector<char*> UserNames;
    vector<double> xmin;
    vector<double> xmax;
    vector<double> ymin;
    vector<double> ymax;
    VisitMinHeap* visitPoint;

public:
    RootFinder(char* wayPointFilesDir);
    void loadHotSpotFiles(char* hotSpotFilesDir);
    void writeAllRoots(char* rootFilesDir);
};

RootFinder::RootFinder(char* wayPointFilesDir)
{
    //  онструктор составл€ет спсок всех пользователей (имЄн файлов в директории путевых точек)
    // и помещает его в вектор UserNames. ƒл€ каждого пользовател€ он заводит двоичную кучу
    // дл€ хранени€ путевых точек в пор€дке возрастани€ времени прихода

    char waypointFileNamePattern[256];
    buildFullName(waypointFileNamePattern, wayPointFilesDir, "*.wpt");
    cout << "   wayPointFileNamePattern: " << waypointFileNamePattern << endl << endl;

    WIN32_FIND_DATA f;
    if (FindFirstFile(wayPointFilesDir, &f) == INVALID_HANDLE_VALUE)
        { cout << "no waypoint directory " << endl; exit(1); }

    HANDLE h = FindFirstFile(waypointFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE)
    {
        do
        {
            char UserName[256];
            buildFullName(UserName, wayPointFilesDir, f.cFileName);
            char* buffer=new char[256];
            strcpy(buffer, f.cFileName);
            UserNames.push_back(buffer);
        }
        while(FindNextFile(h, &f));
    }
    else
    {
        fprintf(stderr, "Directory or files not found\n");
    }

    for(unsigned int i=0; i<UserNames.size(); i++) cout<<UserNames[i]<<endl;
    visitPoint = new VisitMinHeap[UserNames.size()];
}

void RootFinder::loadHotSpotFiles(char* hotSpotFilesDir)
{
    // »з всех файлов в директории локаций читает данные о путевых точках и распредел€ет их
    // по двоичным кучам соответствующих пользователей c указанием локации каждой путевой точки

    HotSpotVisit hsv;
    double Tsum, X, Y;
    int numWp;


    char hotspotFileNamePattern[256];
    buildFullName(hotspotFileNamePattern, hotSpotFilesDir, "*.hts");
    cout << "   hotspotFileNamePattern: " << hotspotFileNamePattern << endl << endl;

    WIN32_FIND_DATA f;
    if (FindFirstFile(hotSpotFilesDir, &f) == INVALID_HANDLE_VALUE)
        { cout << "no hotspot directory " << endl; exit(1); }

    HANDLE h = FindFirstFile(hotspotFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE)
    {
        do {
            char HotSpotFile[256];
            buildFullName(HotSpotFile, hotSpotFilesDir, f.cFileName);
            hsv.HotSpot=new char[strlen(f.cFileName)+1];
            strcpy(hsv.HotSpot, f.cFileName);

            char buffer[256];

            ifstream file(HotSpotFile);
            file>>hsv.Xmin>>hsv.Xmax>>hsv.Ymin>>hsv.Ymax>>Tsum>>numWp;
            while(!file.eof()) {
                for(int i=0; i<256; i++) buffer[i]='\0';
                file>>X>>Y>>hsv.Tb>>hsv.Te>>buffer;
                for(unsigned int i=0; i<UserNames.size();i++)
                    if( strcmp(UserNames[i], buffer) == 0 ) { (visitPoint[i]).Insert(hsv); break; }
            }
            file.close(); cout<<hsv.HotSpot<<" loaded"<<endl;

        } while(FindNextFile(h, &f));
    }
    else
    {
        fprintf(stderr, "Directory or files not found\n");
    }
}


void RootFinder::writeAllRoots(char* rootFilesDir)
{
    // „итает двоичную кучу путевых точек каждого пользовател€ и записывает в файл в директории маршрутов
    // пор€док следовани€ локаций в пути этого пользовател€ (со временеим и кол-вом путевых точек в каждой).
    // “акже строит гистограмму количества локаций в пут€х пользователей и записывает в файл length.hst
    // ¬се пути записывает в файл allroots.roo по одному пути в строке в формате кол-во кластеров в пути,
    // потом список имЄн кластеров.

    WIN32_FIND_DATA f;
    if (FindFirstFile(rootFilesDir, &f) == INVALID_HANDLE_VALUE)
    {
        if (CreateDirectory(rootFilesDir, NULL))
            cout << "create output directory " << endl;
        else
            cout << "error create output directory" << endl;
    }

    HotSpotVisit hsv, nexthsv;
    hsv.HotSpot=new char[4096];
    nexthsv.HotSpot=new char[4096];
    double Tsum=0; int counter=0;
    char buffer[4096];

    ofstream allroot("allroots.roo");
    char rootstring[4096];

    int rootLength[UserNames.size()];
    long int rootT[UserNames.size()], rootP[UserNames.size()];
    for(unsigned int i=0; i<UserNames.size(); i++) { rootLength[i]=0; rootT[i]=rootP[i]=0; }
    int maxLength=0; long int rootTime, rootPoints;

    for(unsigned int i=0; i<UserNames.size(); i++)
    {
        buildFullName(buffer, rootFilesDir, UserNames[i]);
        strcat(buffer, ".rot");
        ofstream file(buffer);
        rootTime=0; rootPoints=0;
        strcpy(rootstring, "");
        while ((visitPoint[i]).GetMin(hsv)) {
            (visitPoint[i]).DeleteMin();
            Tsum=hsv.Te-hsv.Tb; counter=1;
            while( (visitPoint[i]).GetMin(nexthsv) && strcmp(hsv.HotSpot, nexthsv.HotSpot)==0) {
                    (visitPoint[i]).DeleteMin();
                    Tsum+=nexthsv.Te-nexthsv.Tb; counter++;
            }
            file<<hsv.HotSpot<<"\t"<<hsv.Xmin<<"\t"<<hsv.Xmax<<"\t"<<hsv.Ymin<<"\t"<<hsv.Ymax<<"\t"<<Tsum<<"\t"<<counter<<endl;
            rootLength[i]++; rootTime+=Tsum; rootPoints+=counter;
            strcat(rootstring, hsv.HotSpot); strcat(rootstring, "\t");
        }
        file.close();
        if(rootLength[i]>maxLength) maxLength=rootLength[i];
        rootT[i]=rootTime; rootP[i]=rootPoints;
        allroot<<buffer<<"\t"<<rootLength[i]<<"\t"<<rootstring<<endl;
    }
    delete[] hsv.HotSpot;
    delete[] nexthsv.HotSpot;
    allroot.close();

    int lengthHistogram[maxLength+1];
    long int rT[maxLength+1], rP[maxLength+1];
    for(int i=0; i<=maxLength; i++) { lengthHistogram[i]=0; rT[i]=rP[i]=0; }
    for(unsigned int i=0; i<UserNames.size(); i++) {
        lengthHistogram[rootLength[i]]++;
        rT[rootLength[i]]+=rootT[i]; rP[rootLength[i]]+=rootP[i];
    }
    ofstream lhst("length.hst");
    for(int i=0; i<=maxLength; i++) lhst<<i<<"\t"<<lengthHistogram[i]<<"\t"<<rT[i]<<"\t"<<rP[i]<<endl;
    lhst.close();
}


int main(int argc, char** argv)
{
    cout << "Hello world!" << endl;

    char* wayPointFilesDir;    //full path name of way point files directory
    char* hotspotFilesDir;     //full path name of hot spot files directory
    char* rootFilesDir;        //full path name of root files directory
    switch(argc)
    {
    case 1 :
        wayPointFilesDir="./waypointfiles";
        hotspotFilesDir="./hotspotfiles";
        rootFilesDir="./rootfiles";
        break;
    case 2 :
        wayPointFilesDir=argv[1];
        hotspotFilesDir="./hotspotfiles";
        rootFilesDir="./rootfiles";
        break;
    case 3 :
        wayPointFilesDir=argv[1];
        hotspotFilesDir=argv[2];
        rootFilesDir="./rootfiles";
        break;
    case 4:
    default:
        wayPointFilesDir=argv[1];
        hotspotFilesDir=argv[2];
        rootFilesDir=argv[3];
        break;
    }

    RootFinder h(wayPointFilesDir);
    h.loadHotSpotFiles(hotspotFilesDir);
    h.writeAllRoots(rootFilesDir);

    return 0;
}
