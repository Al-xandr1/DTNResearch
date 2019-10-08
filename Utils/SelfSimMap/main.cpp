#include <iostream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <math.h>
#include <vector>

using namespace std;

#define PI 3.1415926

bool GVrand(double R, double& q1, double& q2, double& q3, double& q4)
{
    if( R<0 ) { cout<<"Wrong argument: R = " << R<<endl; return false; }

    if( R>3 ) R=3;

    if( R>2 ) {
        double epsilon=(24-sqrt(24*24-4*39*(3-R)))/2/39;
        double q[]={epsilon, epsilon, epsilon, epsilon};
        q[rand()%4]=1-3*epsilon;
        q1=q[0]; q2=q[1]; q3=q[2]; q4=q[3];
        return true;
    }
    else {
        double r,phi,theta,z1,x2,x3;
        r=sqrt(R)/2;
        for(int i=0; i<1000; i++){
           phi=2*PI*(double)rand()/RAND_MAX;
           theta=PI*(double)rand()/RAND_MAX;

           z1=r*sin(theta)*cos(phi);
           x2=r*sin(theta)*sin(phi);
           x3=r*cos(theta);

           q1=z1/2/sqrt(3)-x3*2/sqrt(6)+0.25;
           q2=z1/2/sqrt(3)+x2/sqrt(2)+x3/sqrt(6)+0.25;
           q3=z1/2/sqrt(3)-x2/sqrt(2)+x3/sqrt(6)+0.25;
           q4=1-q1-q2-q3;

          if(q1>=0 && q2>=0 && q3>=0 && q4>=0 ) return true;
       }
    }
    return false;
}


class SelfSimMapGenerator {
protected:
    double Xmin, Xmax, Ymin, Ymax;
    int levels;
    long int arraySize;
    unsigned int* pointsInArea;
    double* variance;
    double* newvar;

public:
    SelfSimMapGenerator(char* boundFile, int lvl);
    ~SelfSimMapGenerator();
    void MakeSelfSimSet(char* varfile, int waypoints);
    void PutPointsInArea(int lvl, long int index, double xmin, double xmax, double ymin, double ymax, ofstream* mapfile );
    void PutSetOnMap(char* mapfile);
};

// т.к. нулевой пропускаетс€, а lvl не включаетс€, то всего уровней 9: [1,9]
SelfSimMapGenerator::SelfSimMapGenerator(char* boundFile, int lvl=10)
{
    int i;
    long int h4;

    ifstream* bfile=new ifstream(boundFile);
    if(bfile == NULL) { cout<<"No bound file\n"; exit(1); }
    (*bfile)>>Xmin>>Xmax>>Ymin>>Ymax;
    cout<<"Area bounds:"<<Xmin<<"\t"<<Xmax<<"\t"<<Ymin<<"\t"<<Ymax<<endl;

    levels=lvl;
    for(i=1, h4=1; i<=levels; i++) h4*=4;
    arraySize=(4*h4-1)/3;

    pointsInArea=new unsigned int[arraySize];
    if(pointsInArea==NULL) {cout<<"out of memory\n"; exit(3);}
    for(long int i=0; i<arraySize; i++) pointsInArea[i]=0;
    variance=new double[levels];
    newvar=new double[levels];

    bfile->close();
}

SelfSimMapGenerator::~SelfSimMapGenerator()
{
    delete[] pointsInArea;
    delete[] variance;
    delete[] newvar;
}



void SelfSimMapGenerator::MakeSelfSimSet(char* varfile, int waypoints)
{
    int lvl, ii, j;
    long int index, hsize;
    unsigned int points;
    double R, r, q1, q2, q3, q4, MX2, MX, deltaX, deltaY;

    ifstream* vfile= new ifstream(varfile);
    if(vfile==NULL) {cout<<"No variance file\n"; exit(2); }
    for(ii=0; ii<levels && !vfile->eof(); ii++) {
        (*vfile)>>j>>variance[ii]>>deltaX>>deltaY;
        if(j!=ii) { cout<<"Bad variance file!"<<endl; exit(3);}
        if (ii == 0) continue; // пропускаем нулевой уровень из файла
    }
    if(ii<levels) { levels=ii; cout<<"Warning: variance file is too small: "<<levels<<" levels"<<endl; }
    vfile->close();

    pointsInArea[index=0]=waypoints;
    newvar[0]=0;
    for(lvl=0, hsize=1; lvl<levels-1; lvl++) {

        R=(variance[lvl+1]+1)/(newvar[lvl]+1)-1;
        MX2=0; MX=0;

        for(long int i=0; i<hsize; i++, index++) {

            if( (points=pointsInArea[index])==0 ) continue;

            if( !GVrand(R, q1, q2, q3, q4) ) { cout<<"GVrand failed"<<endl; exit(4); }
            while(points>0) {
                r=(double)rand()/RAND_MAX;

                if( r<q1 ) pointsInArea[4*index+1]++;
                else if( r<q1+q2 ) pointsInArea[4*index+2]++;
                else if( r<q1+q2+q3 ) pointsInArea[4*index+3]++;
                else pointsInArea[4*index+4]++;

                points--;
           }
           MX2+= pointsInArea[4*index+1]*pointsInArea[4*index+1]+pointsInArea[4*index+2]*pointsInArea[4*index+2]+
                 pointsInArea[4*index+3]*pointsInArea[4*index+3]+pointsInArea[4*index+4]*pointsInArea[4*index+4];
           MX += pointsInArea[4*index+1]+pointsInArea[4*index+2]+pointsInArea[4*index+3]+pointsInArea[4*index+4];
        }
        hsize*=4;
        newvar[lvl+1]=MX2*hsize/(MX*MX)-1;
        cout<<lvl+1<<"\t R="<<R<<"\t"<<newvar[lvl+1]<<"\t"<<variance[lvl+1]<<endl;
    }

}


void SelfSimMapGenerator::PutPointsInArea(int lvl, long int index, double xmin, double xmax, double ymin, double ymax, ofstream* mapfile )
{
    unsigned int points;

    if( (points=pointsInArea[index])==0 ) return;

    if(lvl>=levels-1) {
        while(points>0) {
           (*mapfile)<<xmin+(xmax-xmin)*rand()/RAND_MAX<<"\t"<<ymin+(ymax-ymin)*rand()/RAND_MAX<<"\t";
           (*mapfile)<<-1<<"\t"<<-1<<endl;
           points--;
        }
    } else {
        PutPointsInArea(lvl+1, 4*index+1, xmin, (xmin+xmax)/2, ymin, (ymin+ymax)/2, mapfile );
        PutPointsInArea(lvl+1, 4*index+2, (xmin+xmax)/2, xmax, ymin, (ymin+ymax)/2, mapfile );
        PutPointsInArea(lvl+1, 4*index+3, xmin, (xmin+xmax)/2, (ymin+ymax)/2, ymax, mapfile );
        PutPointsInArea(lvl+1, 4*index+4, (xmin+xmax)/2, xmax, (ymin+ymax)/2, ymax, mapfile );
    }

}


void SelfSimMapGenerator::PutSetOnMap(char* mapfile)
{
    ofstream* mfile=new ofstream(mapfile);

    PutPointsInArea(0, 0, Xmin, Xmax, Ymin, Ymax, mfile );

    mfile->close();

}

//------------------------------------ √енераци€ дл€ случа€ одной карты ----------------------------------------

int putSetOnSingleMap(int totalPoints = 6000)
{
    cout << "Hello world!" << endl;

    SelfSimMapGenerator h("bounds.bnd");
    h.MakeSelfSimSet("variances.txt", totalPoints);
    h.PutSetOnMap("artifical.wpt");

    cout << endl << "Complete!" << endl;
    return 0;
}

//------------------------------------ √енераци€ дл€ случа€ набора кластеров ------------------------------------

char* buildFullName(char* buffer, char* dir, char* fileName)
{
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}


class HotSpotInfo {
public:
    char* hotSpotName;
    char* fullHotSpotName;
    double square;
    double points;

public:
    HotSpotInfo(char* fullName, char* name) {
        fullHotSpotName = new char[256];
        strcpy(fullHotSpotName, fullName);

        hotSpotName = new char[256];
        strcpy(hotSpotName, name);

        double Xmin, Xmax, Ymin, Ymax;
        ifstream* hotSpot = new ifstream(fullName);
        if(hotSpot == NULL) { cout<<"No hotSpot file\n"; exit(2); }
        (*hotSpot)>>Xmin>>Xmax>>Ymin>>Ymax;
        cout<<"\tHotSpot bounds:"<<Xmin<<"\t"<<Xmax<<"\t"<<Ymin<<"\t"<<Ymax<<endl;

        square = (Xmax - Xmin)*(Ymax - Ymin);
        cout<<"\tHotSpot square:"<<square<<endl;
        points = -1;

        hotSpot->close();
        delete hotSpot;
    }

    char* buildWayPointFileName(char* dir)
    {
        char* wayPointFileName = new char[256];
        wayPointFileName = buildFullName(wayPointFileName, dir, hotSpotName);
        wayPointFileName = strcat(wayPointFileName, ".wpt");

        cout << "wayPointFileName: " << wayPointFileName << endl;
        return wayPointFileName;
    }

    //¬ысчитывает доли точек на каждый кластер
    static void distributePoints(vector<HotSpotInfo*>* infos, int totalPoints)
    {
        double sum = 0;
        for (int i=0; i<infos->size(); i++) sum += (*infos)[i]->square;

        int remainPoints = totalPoints;
        for (int i=0; i<infos->size()-1; i++)
        {
            (*infos)[i]->points = floor(((*infos)[i]->square / sum) * totalPoints);
            //если ноль тогда сколько точек ???
            //if((*infos)[i]->points == 0){
            //    (*infos)[i]->points = 1;
            //}
            remainPoints -= (*infos)[i]->points;
        }
        (*infos)[infos->size()-1]->points = remainPoints;

        //for debug
        int checkPoints = 0;
        for (int i=0; i<infos->size(); i++) checkPoints += (*infos)[i]->points;
        if (checkPoints != totalPoints) {cout << "checkPoints=" << checkPoints << endl; exit(-3);}
    }

    static vector<HotSpotInfo*>* loadAllHotSpotInfo(char* hotSpotDir)
    {
        vector<HotSpotInfo*>* infos = new vector<HotSpotInfo*>();

        WIN32_FIND_DATA f;

        char hotspotFileNamePattern[256];
        buildFullName(hotspotFileNamePattern, hotSpotDir, "*.hts");

        HANDLE h = FindFirstFile(hotspotFileNamePattern, &f);
        if(h != INVALID_HANDLE_VALUE)
        {
            do
            {
                char inputFileName[256];
                buildFullName(inputFileName, hotSpotDir, f.cFileName);
                cout << "inputFileName: " << inputFileName << endl;

                HotSpotInfo* info = new HotSpotInfo(inputFileName, f.cFileName);
                infos->push_back(info);
            }
            while(FindNextFile(h, &f));
        }
        else
        {
            fprintf(stderr, "Directory or files not found\n");
        }

        return infos;
    }
};


int putSetOnHotspots(int argc, char** argv, int totalPoints = 6000)
{
    cout << "Hello world!" << endl;

    char* hotspotsDir;
    char* varfile;
    int points = totalPoints;
    switch(argc)
    {
    case 1 :
        hotspotsDir = "./hotspotfiles";
        varfile = "variances.txt";
        break;

    case 2 :
        hotspotsDir = argv[1];
        varfile = "variances.txt";
        break;

    case 3 :
        hotspotsDir = argv[1];
        varfile = argv[2];
        break;

    case 4:
    default:
        hotspotsDir = argv[1];
        varfile = argv[2];
        points = atoi(argv[3]);
        break;
    }

    WIN32_FIND_DATA f;
    if (FindFirstFile("./waypointfiles", &f) == INVALID_HANDLE_VALUE)
    {
        if (CreateDirectory("./waypointfiles", NULL)) cout << "create output directory " << endl;
        else cout << "error create output directory" << endl;
    }

    vector<HotSpotInfo*>* infos = HotSpotInfo::loadAllHotSpotInfo(hotspotsDir);
    HotSpotInfo::distributePoints(infos, points);
    cout << endl;

    for (int i=0; i<infos->size(); i++)
    {
        cout << "(*infos)[i]->hotSpotName = " << (*infos)[i]->hotSpotName << endl;
        cout << "(*infos)[i]->fullHotSpotName = " << (*infos)[i]->fullHotSpotName << endl;
        cout << "(*infos)[i]->square = " << (*infos)[i]->square << endl;
        cout << "(*infos)[i]->points = " << (*infos)[i]->points << endl << endl;
        SelfSimMapGenerator* h = new SelfSimMapGenerator((*infos)[i]->fullHotSpotName);
        h->MakeSelfSimSet("variances.txt", (*infos)[i]->points);
        h->PutSetOnMap((*infos)[i]->buildWayPointFileName("./waypointfiles"));
        delete h;
    }

    cout << endl << "Complete!" << endl;
    return 0;
}


int main(int argc, char** argv)
{
    //putSetOnSingleMap();
    putSetOnHotspots(argc, argv, 20000);
}
