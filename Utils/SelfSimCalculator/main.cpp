#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <math.h>

using namespace std;

class SelfSimCalculator {
protected:
    int levels;
    long int arraySize;
    double Xmin, Xmax, Ymin, Ymax;
    unsigned int* pointsInArea;
    double* variance;
public:
    SelfSimCalculator(char* boundFile, int lvl);
    ~SelfSimCalculator();
    void loadWaypoints(char* WaypointFile);
    void loadAllDir(char* WaypoitDir);
    void calculateVariances();
    void showArray(int level);
};


SelfSimCalculator::SelfSimCalculator(char* boundFile, int lvl=9)
{
    int i;
    long int h4;

    ifstream* bfile= new ifstream(boundFile);
    if(bfile==NULL) {cout<<"No bound file\n"; exit(1); }
    (*bfile)>>Xmin>>Xmax>>Ymin>>Ymax;
    cout<<"Area bounds:"<<Xmin<<"\t"<<Xmax<<"\t"<<Ymin<<"\t"<<Ymax<<endl;
    bfile->close();

    levels=lvl;
    for(i=1, h4=1; i<=levels; i++) h4*=4;
    arraySize=(4*h4-1)/3;

    pointsInArea=new unsigned int[arraySize];
    if(pointsInArea==NULL) {cout<<"out of memory\n"; exit(3);}
    for(long int i=0; i<arraySize; i++) pointsInArea[i]=0;
    variance=new double[levels];
}

SelfSimCalculator::~SelfSimCalculator()
{
    delete[] pointsInArea;
    delete[] variance;
}


void SelfSimCalculator::loadWaypoints(char* WaypointFile)
{
    double xcoord, ycoord, tb, te, xmin, xmax, ymin, ymax, hx, hy;
    int lvl;
    long int index;

    char file[256];
    strcpy(file, WaypointFile);

    ifstream* wfile= new ifstream(file);
    if(wfile==NULL) { cout<<"No waypoint file\n"; exit(2); }

    while( !wfile->eof() ) {

        xmin=Xmin; xmax=Xmax; ymin=Ymin; ymax=Ymax;
        (*wfile)>>xcoord>>ycoord>>tb>>te;
        pointsInArea[0]++;

        for( index=0, lvl=1; lvl<levels; lvl++) {
            if( xcoord <= (hx=(xmin+xmax)/2) ) {
                xmax=hx;
                if( ycoord <= (hy=(ymin+ymax)/2) ) { ymax=hy; index=4*index+1;}
                else { ymin=hy; index=4*index+3; }
            }
            else {
                xmin=hx;
                if( ycoord <= (hy=(ymin+ymax)/2) ) { ymax=hy; index=4*index+2;}
                else { ymin=hy; index=4*index+4; }
            }
            pointsInArea[index]++;
        }
    }
    wfile->close();
}



char* buildFullName(char* buffer, char* dir, char* fileName)
{
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}


void SelfSimCalculator::loadAllDir(char* WaypoitDir)
{
WIN32_FIND_DATA f;

    char waypointFileNamePattern[256];
    buildFullName(waypointFileNamePattern, WaypoitDir, "*.wpt");

    HANDLE h = FindFirstFile(waypointFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE)
    {
        do
        {
            char inputFileName[256];
            buildFullName(inputFileName, WaypoitDir, f.cFileName);
            cout << "inputFileName: " << inputFileName << endl;

            this->loadWaypoints(inputFileName);
        }
        while(FindNextFile(h, &f));
    }
    else
    {
        fprintf(stderr, "Directory or files not found\n");
    }
}



void SelfSimCalculator::calculateVariances()
{
    int lvl;
    long int index, hsize;
    double MX2, MX;

    for(lvl=0, hsize=1, index=0; lvl<levels; lvl++) {
        MX2=0; MX=0;
        for(long int i=0; i<hsize; i++) { MX2+=pointsInArea[index+i]*pointsInArea[index+i]; MX+=pointsInArea[index+i]; }
        // variance[lvl]=MX2/hsize-(MX/hsize)*(MX/hsize);
        variance[lvl]=MX2*hsize/(MX*MX)-1;
        cout<<lvl<<"\t"<<variance[lvl]<<endl;
        index+=hsize; hsize*=4;
    }
    double Mxy=0, Mx=0, My=0, Mx2=0;
    for(int i=1; i<levels; i++) {
        Mx += 2*i;
        My += log2(variance[i]);
        Mx2+= 4*i*i;
        Mxy+= log2(variance[i])*2*i;
    }
    Mx/=levels-1; My/=levels-1; Mx2/=levels-1; Mxy/=levels-1;
    double c,b,H;
    b=(Mxy-Mx*My)/(Mx2-Mx*Mx);
    c=My-b*Mx;
    H=1-fabs(b)/2;
    cout<<"b="<<b<<"\t c="<<c<<"\t H="<<H<<endl;
    ofstream *file1=new ofstream("herst.txt");
    (*file1)<<"b="<<b<<"\t c="<<c<<"\t H="<<H<<endl;
    ofstream *file2=new ofstream("variances.txt");
    for(int i=1; i<levels; i++) (*file2)<<i<<"\t"<<variance[i]<<endl;
    file1->close();
    file2->close();
}

void SelfSimCalculator::showArray(int level)
{
    long int hsize=1, index=0;
    for(int i=0; i<=level; i++) {
        for(long int j=index; j<index+hsize; j++)  cout<<pointsInArea[j]<<" ";
        index+=hsize; hsize*=4;
        cout<<endl;
    }
}


int main(int argc, char** argv)
{
    cout << "Hello world!" << endl;

    char* WaypointDir;
    char* boundFile;
    int lvl;
    switch(argc)
    {
    case 1 :
        WaypointDir="./waypointfiles";
        boundFile="bounds.bnd";
        lvl=9;
        break;
    case 2 :
        WaypointDir=argv[1];
        boundFile="bounds.bnd";
        lvl=9;
        break;
    case 3 :
        WaypointDir=argv[1];
        boundFile=argv[2];
        lvl=9;
        break;
    case 4:
    default:
        WaypointDir=argv[1];
        boundFile=argv[2];
        lvl=atoi(argv[3]);
        break;
    }

    SelfSimCalculator h(boundFile, lvl);
    h.loadAllDir(WaypointDir);
    h.calculateVariances();

    return 0;
}
