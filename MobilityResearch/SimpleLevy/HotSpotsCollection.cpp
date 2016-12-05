#include "HotSpotsCollection.h"


bool HotSpotsCollection::isHotSpotsCollectionReady=false;
vector<HotSpotData> HotSpotsCollection::HSData;


void HotSpotsCollection::readHotSpotsInfo(char* TracesDir, double& minX, double& maxX, double& minY, double& maxY)
{
   double Xmin, Xmax, Ymin, Ymax;
   if(!isHotSpotsCollectionReady) {
        char* spotInfoFileName = buildFullName(TracesDir, LOC_FILE);
        ifstream* spotInfoFile = new ifstream(spotInfoFileName);
        if (!spotInfoFile) { cout << " No spotInfoFile file: " << spotInfoFileName << endl;  exit(104); }

        char lastAddedName[256];

        while(!spotInfoFile->eof()) {
            char hotSpotName[256];
            double sumTime;
            unsigned int waypointNum;
            (*spotInfoFile) >> hotSpotName >> sumTime >> waypointNum >> Xmin >> Xmax >> Ymin >> Ymax;
            if (strcmp(lastAddedName, hotSpotName)!=0) {
                HotSpotData hsi(hotSpotName, Xmin, Xmax, Ymin, Ymax, sumTime, waypointNum);
                HSData.push_back(hsi);
                strcpy(lastAddedName, hotSpotName);
                hsi.print();
            }
        }

        char* spotCountFileName = buildFullName(TracesDir, SPC_FILE);
        ifstream* spotCountFile = new ifstream(spotCountFileName);
        if (!spotCountFile) { cout << " No spotCountFile file: " << spotCountFileName << endl; exit(104); }

        while(!spotCountFile->eof()) {
            char hotSpotName[256];
            int counter;
            (*spotCountFile) >> hotSpotName >> counter;
            for(unsigned int i=0; i<HSData.size(); i++)
                if( strcmp( (HSData[i]).hotSpotName, hotSpotName)==0 ) { (HSData[i]).counter=counter; break; }
        }

        spotInfoFile->close();
        spotCountFile->close();
        delete spotInfoFile;
        delete spotCountFile;

        isHotSpotsCollectionReady=true;
        print();
   }

   Xmin=(HSData[0]).Xmin; Xmax=(HSData[0]).Xmax;
   Ymin=(HSData[0]).Ymin; Ymax=(HSData[0]).Ymax;
   for(unsigned int i=1; i<HSData.size(); i++) {
      if((HSData[i]).Xmin < Xmin ) Xmin = (HSData[i]).Xmin;
      if((HSData[i]).Xmax > Xmax ) Xmax = (HSData[i]).Xmax;
      if((HSData[i]).Ymin < Ymin ) Xmin = (HSData[i]).Ymin;
      if((HSData[i]).Ymax > Ymax ) Ymax = (HSData[i]).Ymax;
   }
   minX = Xmin;
   maxX = Xmax;
   minY = Ymin;
   maxY = Ymax;
}


void HotSpotsCollection::print()
{
    for (unsigned int i=0; i < HSData.size(); i++) {
        HotSpotData info = HSData[i];
        cout << info.hotSpotName<<": xMin=" << info.Xmin << " yMin=" << info.Ymin
                << " xMax=" << info.Xmax << " yMax=" << info.Ymax << endl;
    }
    cout<<endl;
}


HotSpotData* HotSpotsCollection::findHotSpotbyName(char* HotSpotName, int& HotSpotNum)
{
    for(unsigned int i=0; i<HSData.size(); i++)
        if( strcmp(HSData[i].hotSpotName, HotSpotName) == 0 ) {
            HotSpotNum=i;
            return &HSData[i];
        }
    return NULL;
}


HotSpotData* HotSpotsCollection::randomRemove(vector<HotSpotData*>* hotSpots, int& HotSpotNum)
{
    HotSpotNum = (int) round(uniform(0, (double) (hotSpots->size() - 1)));
    HotSpotData* removedItem = hotSpots->at(HotSpotNum);

    ASSERT(HotSpotNum >= 0 && HotSpotNum < hotSpots->size() && removedItem != NULL);

    hotSpots->erase(hotSpots->begin() + HotSpotNum);

    return removedItem;
}



bool HSDistanceMatrix::isMatrixReady = false;
vector<double>* HSDistanceMatrix::DistanceMatrix;


void HSDistanceMatrix::makeDistanceMatrix()
{
    if(HotSpotsCollection::isHotSpotsCollectionReady && !isMatrixReady) {
        DistanceMatrix = new vector<double>[HotSpotsCollection::HSData.size()];
        for(unsigned int i=0; i<HotSpotsCollection::HSData.size(); i++) {
            DistanceMatrix[i].clear();
            DistanceMatrix[i].push_back(0);
            for(unsigned int j=i+1; j<HotSpotsCollection::HSData.size(); j++) {
                double d2=((HotSpotsCollection::HSData[i]).Xcenter-(HotSpotsCollection::HSData[j]).Xcenter)*
                          ((HotSpotsCollection::HSData[i]).Xcenter-(HotSpotsCollection::HSData[j]).Xcenter)+
                          ((HotSpotsCollection::HSData[i]).Ycenter-(HotSpotsCollection::HSData[j]).Ycenter)*
                          ((HotSpotsCollection::HSData[i]).Ycenter-(HotSpotsCollection::HSData[j]).Ycenter);
                DistanceMatrix[i].push_back(sqrt( d2 ));
            }
        }
        isMatrixReady=true;
    }
}


double HSDistanceMatrix::getDistance(unsigned int i, unsigned int j)
{
    if(i<=j) return (DistanceMatrix[i]).at(j-i);
    else return (DistanceMatrix[j]).at(i-j);
}


bool HSDistanceMatrix::isProbabilityReady = false;
double**  HSDistanceMatrix::ProbabilityMatrix;


void HSDistanceMatrix::makeProbabilityMatrix(double powA)
{
    if(isMatrixReady && !isProbabilityReady) {
        ProbabilityMatrix = new double*[HotSpotsCollection::HSData.size()];
        for(unsigned int i=0; i<HotSpotsCollection::HSData.size(); i++) {
            ProbabilityMatrix[i]= new double[HotSpotsCollection::HSData.size()];
            double h=0;
            for(unsigned int j=0; j<HotSpotsCollection::HSData.size(); j++) {
                if(i!=j) h += ProbabilityMatrix[i][j] = pow(1/getDistance(i,j), powA);
                else ProbabilityMatrix[i][j]=0;
            }
            for(unsigned int j=0; j<HotSpotsCollection::HSData.size(); j++) ProbabilityMatrix[i][j]/=h;
        }
        isProbabilityReady=true;
    }
}
