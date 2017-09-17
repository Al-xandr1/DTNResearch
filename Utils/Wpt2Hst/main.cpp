#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <math.h>
#include <vector>
#include <windows.h>

using namespace std;

ifstream* wpFile;
vector<double> pauseTime, WPdistance, travelTime;
double maxDistance=0, minDistance=1.0e10, maxPause=0, minPause=1.0e10;

double tDelta, dDelta;
int distBaskets, timeBaskets, *distSamples, *timeSamples;
double *distBounds, *pauseBounds;
int distOutOfBound, timeOutOfBound;

void WPfileProcessing(char* wpFileName)
{
   double x1,x2,y1,y2,Tb1,Te1,Tb2,Te2,time,dist;

   wpFile = new ifstream(wpFileName);
   if(wpFile == NULL){
       cout<<"Input wayPoint file "<<wpFileName<<" is not found."<<endl;
       exit(1);
   }

   if( !wpFile->eof() ) {
       (*wpFile) >> x1 >> y1 >> Tb1 >> Te1;

       time=Te1-Tb1;
       if(maxPause<time) maxPause=time;
       if(minPause>time) minPause=time;
       pauseTime.push_back(time);
   }
   while( !wpFile->eof() ) {
       (*wpFile) >> x2 >> y2 >> Tb2 >> Te2;

       dist=sqrt( (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1) );
       if(maxDistance<dist) maxDistance=dist;
       if(minDistance>dist) minDistance=dist;
       WPdistance.push_back(dist);
       travelTime.push_back(Tb2-Te1);

       time=Te2-Tb2;
       if(maxPause<time) maxPause=time;
       if(minPause>time) minPause=time;
       pauseTime.push_back(time);

       x1=x2; y1=y2; Tb1=Tb2; Te1=Te2;
   }

   wpFile->close();
   cout<<"\t"<<pauseTime.size()<<" waypoints were processed"<<endl;
}

void HistCreate(int timeBas, double tD, int distBas, double dD)
{
    timeBaskets = timeBas;
    distBaskets = distBas;
    dDelta = dD;
    tDelta = tD;

    distBounds  = new double[distBaskets];
    pauseBounds = new double[timeBaskets];
    distSamples = new int[distBaskets];
    timeSamples = new int[timeBaskets];

    for(int i=0; i<distBaskets; i++) { distSamples[i]=0; distBounds[i] = (i+1)*dDelta; }
    for(int i=0; i<timeBaskets; i++) { timeSamples[i]=0; pauseBounds[i]= (i+1)*tDelta; }
    distOutOfBound = 0; timeOutOfBound = 0;

    for(int i=0; i<WPdistance.size(); i++){
        int j;
        for(j=0; WPdistance[i]>distBounds[j] && j<distBaskets; j++ );
        if(j<distBaskets) distSamples[j]++;
        else distOutOfBound++;
    }

    for(int i=0; i<pauseTime.size(); i++){
        int j;
        for(j=0; pauseTime[i]>pauseBounds[j] && j<timeBaskets; j++ );
        if(j<timeBaskets)timeSamples[j]++;
        else timeOutOfBound++;
    }
}

void HistSave(char *FileName, double MinVal, double MaxVal, int Samples, int Baskets, int Delta, int OutOfBound, int* HistData)
{
    ofstream *file=new ofstream(FileName);
    if(file==NULL) exit(1);

    (*file)<<MinVal<<"\t"<<MaxVal<<"\t"<<Samples<<"\t"<<Baskets<<"\t"<< Delta <<"\t"<<OutOfBound<<endl;
    for(int i=0; i<Baskets; i++) (*file)<<HistData[i]<<endl;

    file->close();
}

//-----------------------------------------------------------------------------------------------------------------------------

char* getWayPointFileName(char* wayPointFileName)
{
    return strcat(wayPointFileName,".wpt");
}

char* buildFullName(char* buffer, char* dir, char* fileName)
{
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}

int main(int argc, char** argv)
{
        cout << "Hello world!" << endl;

    char* wayPointFilesDir; //full path name of directory
    char* histogramFilesDir; // full path name of directory
    switch(argc)
    {
    case 1 :
        wayPointFilesDir  = "./waypointfiles";
        histogramFilesDir = "./histogramfiles";
        break;
    case 2 :
        wayPointFilesDir  = argv[1];
        histogramFilesDir = "./histogramfiles";
        break;
    case 3 :
    default:
        wayPointFilesDir  = argv[1];
        histogramFilesDir = argv[2];
        break;
    }

    cout << "Start histogram build..." << endl;

    WIN32_FIND_DATA f;
    if (FindFirstFile(histogramFilesDir, &f) == INVALID_HANDLE_VALUE)
    {
        if (CreateDirectory(histogramFilesDir, NULL))
            cout << "create output directory " << endl;
        else
            cout << "error create output directory" << endl;
    }

    char wayPointFileNamePattern[256];
    buildFullName(wayPointFileNamePattern, wayPointFilesDir, "*.wpt");
    cout << "   wayPointFileNamePattern: " << wayPointFileNamePattern << endl << endl;

    HANDLE h = FindFirstFile(wayPointFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE)
    {
        do
        {
            char inputFileName[256];
            buildFullName(inputFileName, wayPointFilesDir, f.cFileName);

            cout << "  inputFileName: " << inputFileName;
            WPfileProcessing(inputFileName);

        }
        while(FindNextFile(h, &f));
    }
    else
    {
        fprintf(stderr, "Directory or files not found\n");
    }

    HistCreate(256, 30., 256, 10.);

    char outPutFileName1[256];
    buildFullName(outPutFileName1, histogramFilesDir, "distances.hst");
    cout << "       outPutFileName1: " << outPutFileName1 << endl << endl;

    HistSave(outPutFileName1, minDistance, maxDistance, WPdistance.size(), distBaskets, dDelta, distOutOfBound, distSamples);

    char outPutFileName2[256];
    buildFullName(outPutFileName2, histogramFilesDir, "pauseTime.hst");
    cout << "       outPutFileName2: " << outPutFileName2 << endl << endl;

    HistSave(outPutFileName2, minPause, maxPause, pauseTime.size(), timeBaskets, tDelta, timeOutOfBound, timeSamples);

    char outPutFileName3[256];
    buildFullName(outPutFileName3, histogramFilesDir, "speed.dat");
    cout << "       outPutFileName: " << outPutFileName3 << endl << endl;

    ofstream *file=new ofstream(outPutFileName3);
    if(file==NULL) exit(1);
    for(int i=0; i<WPdistance.size(); i++) {
        (*file)<<WPdistance[i]<<"\t"<<travelTime[i]<<endl;
    }
    file->close();

    cout << "End." << endl;
    return 0;
}
