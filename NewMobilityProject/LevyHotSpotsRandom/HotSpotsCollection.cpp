#include <HotSpotsCollection.h>

bool HotSpotsCollection::isHSDataReady=false;
vector<HotSpotShortInfo> HotSpotsCollection::HSData;

void HotSpotsCollection::readHotSpotsInfo(char* TracesDir, double& minX, double& maxX, double& minY, double& maxY)
{
   double Xmin, Xmax, Ymin, Ymax;
   if(!isHSDataReady) {
        char* spotInfoFileName = buildFullName(TracesDir, "locations.loc");
        ifstream* spotInfoFile = new ifstream(spotInfoFileName);
        if (!spotInfoFile) { cout << " No spotInfoFile file: " << spotInfoFileName << endl;  exit(104); }

        while(!spotInfoFile->eof()) {
            char hotSpotName[256];
            double sumTime;
            unsigned int waypointNum;
            (*spotInfoFile) >> hotSpotName >> sumTime >> waypointNum >> Xmin >> Xmax >> Ymin >> Ymax;
            HotSpotShortInfo hsi(hotSpotName, Xmin, Xmax, Ymin, Ymax, sumTime, waypointNum);
            HSData.push_back( hsi );
        }

        char* spotCountFileName = buildFullName(TracesDir, "spotcount.cnt");
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

        isHSDataReady=true;
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

   return;
}
