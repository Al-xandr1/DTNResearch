#include "HotSpotsCollection.h"


HotSpotsCollection* HotSpotsCollection::instance = NULL;   // указатель на singleton объект


HotSpotsCollection* HotSpotsCollection::getInstance() {
    if (!instance) instance = new HotSpotsCollection();
    return instance;
}


void HotSpotsCollection::readHotSpotsInfo(char* TracesDir, char* spotcountfile)
{
    ASSERT(!HSData);

    HSData = new vector<HotSpotData>();

    double angle, Xmin, Xmax, Ymin, Ymax;

    const char* spotInfoFileName = buildFullName(TracesDir, LOC_FILE);
    ifstream* spotInfoFile = new ifstream(spotInfoFileName);
    if (!spotInfoFile) { cout << " No spotInfoFile file: " << spotInfoFileName << endl;  exit(104); }

    char lastAddedName[256];

    while(!spotInfoFile->eof()) {
        char hotSpotName[256];
        double sumTime;
        unsigned int waypointNum;
        (*spotInfoFile) >> angle >> hotSpotName >> sumTime >> waypointNum >> Xmin >> Xmax >> Ymin >> Ymax;
        if (strcmp(lastAddedName, hotSpotName)!=0) {
            HotSpotData hsi(hotSpotName, Xmin, Xmax, Ymin, Ymax, sumTime, waypointNum, angle);
            HSData->push_back(hsi);
            strcpy(lastAddedName, hotSpotName);
            hsi.print();
        }
    }

    const char* spotCountFileName = buildFullName(TracesDir, spotcountfile);
    ifstream* spotCountFile = new ifstream(spotCountFileName);
    if (!spotCountFile) { cout << " No spotCountFile file: " << spotCountFileName << endl; exit(104); }

    while(!spotCountFile->eof()) {
        char hotSpotName[256];
        int counter;
        (*spotCountFile) >> hotSpotName >> counter;
        for(unsigned int i=0; i<HSData->size(); i++)
            if( strcmp( (HSData->at(i)).hotSpotName, hotSpotName)==0 ) { HSData->at(i).counter=counter; break; }
    }

    spotInfoFile->close();
    spotCountFile->close();
    delete spotInfoFile;
    delete spotCountFile;
}


void HotSpotsCollection::getTotalSize(double& minX, double& maxX, double& minY, double& maxY)
{
    double Xmin, Xmax, Ymin, Ymax;
    Xmin =  HSData->at(0).Xmin * cos(HSData->at(0).angle) + HSData->at(0).Ymin * sin(HSData->at(0).angle);
    Xmax =  HSData->at(0).Xmax * cos(HSData->at(0).angle) + HSData->at(0).Ymax * sin(HSData->at(0).angle);
    Ymin = -HSData->at(0).Xmax * sin(HSData->at(0).angle) + HSData->at(0).Ymin * cos(HSData->at(0).angle);
    Ymax = -HSData->at(0).Xmin * sin(HSData->at(0).angle) + HSData->at(0).Ymax * cos(HSData->at(0).angle);
    for(unsigned int i=1; i<HSData->size(); i++) {
        double xmin_i, xmax_i, ymin_i, ymax_i;
        xmin_i =  HSData->at(i).Xmin * cos(HSData->at(i).angle) + HSData->at(i).Ymin * sin(HSData->at(i).angle);
        xmax_i =  HSData->at(i).Xmax * cos(HSData->at(i).angle) + HSData->at(i).Ymax * sin(HSData->at(i).angle);
        ymin_i = -HSData->at(i).Xmax * sin(HSData->at(i).angle) + HSData->at(i).Ymin * cos(HSData->at(i).angle);
        ymax_i = -HSData->at(i).Xmin * sin(HSData->at(i).angle) + HSData->at(i).Ymax * cos(HSData->at(i).angle);
        if(xmin_i < Xmin ) Xmin = xmin_i;
        if(xmax_i > Xmax ) Xmax = xmax_i;
        if(ymin_i < Ymin ) Xmin = ymin_i;
        if(ymax_i > Ymax ) Ymax = ymax_i;
    }
    minX = Xmin;
    maxX = Xmax;
    minY = Ymin;
    maxY = Ymax;
}


void HotSpotsCollection::print()
{
    for (unsigned int i=0; i < HSData->size(); i++) {
        HotSpotData info = HSData->at(i);
        cout << info.hotSpotName<<": angle=" << info.angle << " xMin=" << info.Xmin << " yMin=" << info.Ymin
                << " xMax=" << info.Xmax << " yMax=" << info.Ymax << endl;
    }
    cout<<endl;
}


void HotSpotsCollection::saveHotSpots(const char *hotSpotDirName)
{
    for (unsigned int i = 0; i < HSData->size(); i++) {
        const char* fullNameHS = buildFullName(hotSpotDirName, HSData->at(i).hotSpotName);
        ofstream* hsFile = new ofstream(fullNameHS);
        (*hsFile) << HSData->at(i).angle << "\t";
        (*hsFile) << HSData->at(i).Xmin << "\t" << HSData->at(i).Xmax << endl;
        (*hsFile) << HSData->at(i).Ymin << "\t" << HSData->at(i).Ymax << endl;
        (*hsFile) << HSData->at(i).generatedSumTime << "\t"<< HSData->at(i).generatedWaypointNum << endl;

        for(unsigned int j = 0; j < HSData->at(i).waypoints.size(); j++)
            (*hsFile) << HSData->at(i).waypoints[j].X  << "\t" << HSData->at(i).waypoints[j].Y  << "\t"
                      << HSData->at(i).waypoints[j].Tb << "\t" << HSData->at(i).waypoints[j].Te << "\t"
                      << HSData->at(i).waypoints[j].traceName << endl;

        hsFile->close();
        delete hsFile;
    }
}


void HotSpotsCollection::saveLocationsFile(const char *locationsFileName)
{
    ofstream lcfile(locationsFileName);
    for(unsigned int i = 0; i < HSData->size(); i++) {
        lcfile << HSData->at(i).angle << "\t" << HSData->at(i).hotSpotName << "\t" << HSData->at(i).generatedSumTime << "\t" << "\t";
        lcfile << HSData->at(i).generatedWaypointNum << "\t" << "\t";
        lcfile << HSData->at(i).Xmin << "\t"<< HSData->at(i).Xmax << "\t";
        lcfile << HSData->at(i).Ymin << "\t"<< HSData->at(i).Ymax << endl;
    }
    lcfile.close();
}


HotSpotData* HotSpotsCollection::findHotSpotbyName(const char* HotSpotName, int& HotSpotNum)
{
    for(unsigned int i=0; i<HSData->size(); i++)
        if( strcmp(HSData->at(i).hotSpotName, HotSpotName) == 0 ) {
            HotSpotNum=i;
            return &HSData->at(i);
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


// ----------------------------------- HSDistanceMatrix --------------------------------------------


HSDistanceMatrix* HSDistanceMatrix::instance = NULL;     // указатель на singleton объект


HSDistanceMatrix* HSDistanceMatrix::getInstance(double powA) {
    if (!instance) instance = new HSDistanceMatrix(powA);
    return instance;
}

void HSDistanceMatrix::makeDistanceMatrix()
{
    ASSERT(!DistanceMatrix);
    HotSpotsCollection* hsc = HotSpotsCollection::getInstance();
    DistanceMatrix = new vector<double>[hsc->getHSData()->size()];
    for(unsigned int i=0; i<hsc->getHSData()->size(); i++) {
        DistanceMatrix[i].clear();
        DistanceMatrix[i].push_back(0);
        for(unsigned int j=i+1; j<hsc->getHSData()->size(); j++) {
            double d2=(hsc->getHSData()->at(i).Xcenter - hsc->getHSData()->at(j).Xcenter)*
                (hsc->getHSData()->at(i).Xcenter - hsc->getHSData()->at(j).Xcenter)+
                (hsc->getHSData()->at(i).Ycenter - hsc->getHSData()->at(j).Ycenter)*
                (hsc->getHSData()->at(i).Ycenter - hsc->getHSData()->at(j).Ycenter);
            DistanceMatrix[i].push_back(sqrt( d2 ));
        }
    }
}


double HSDistanceMatrix::getDistance(unsigned int i, unsigned int j)
{
    if(i<=j) return (DistanceMatrix[i]).at(j-i);
    else return (DistanceMatrix[j]).at(i-j);
}


void HSDistanceMatrix::makeProbabilityMatrix(double powA)
{
    ASSERT(DistanceMatrix);
    ASSERT(!ProbabilityMatrix);
    HotSpotsCollection* hsc = HotSpotsCollection::getInstance();
    ProbabilityMatrix = new double*[hsc->getHSData()->size()];
    for(unsigned int i=0; i<hsc->getHSData()->size(); i++) {
        ProbabilityMatrix[i]= new double[hsc->getHSData()->size()];
        double h=0;
        for(unsigned int j=0; j<hsc->getHSData()->size(); j++) {
            if(i!=j) h += ProbabilityMatrix[i][j] = pow(1/getDistance(i,j), powA);
            else ProbabilityMatrix[i][j]=0;
        }
        for(unsigned int j=0; j<hsc->getHSData()->size(); j++) ProbabilityMatrix[i][j]/=h;
    }
}
