#ifndef HOTSPOT_H_INCLUDED
#define HOTSPOT_H_INCLUDED

#include <vector>
#include <fstream>
#include "DevelopmentHelper.h"

using namespace std;


struct Waypoint{
    double X,Y,Tb,Te;
    char* traceName;
};


struct HotSpotAvailability {
    char* hotSpotName;
    int count;

    HotSpotAvailability(char* hotSpotName, int count)
    {
        strcpy(this->hotSpotName = new char[256], hotSpotName);
        this->count = count;
    }

    ~HotSpotAvailability()
    {
        if (this->hotSpotName) delete this->hotSpotName;
    }
};


struct HotSpot {
     double Xmin, Xmax, Ymin, Ymax, Xcenter, Ycenter;
     double sumTime;
     vector<Waypoint> waypoint;
     char* hotSpotName;

     HotSpot(char* hotSpotName, double Xmin, double Xmax, double Ymin, double Ymax)
     {
         strcpy(this->hotSpotName = new char[256], hotSpotName);
         this->Xmin = Xmin;
         this->Xmax = Xmax;
         this->Ymin = Ymin;
         this->Ymax = Ymax;
         this->Xcenter = (Xmin + Xmax) / 2;
         this->Ycenter = (Ymin + Ymax) / 2;
         this->sumTime = 0;
     }

     void print()
     {
         cout << "\tXmin=" << Xmin << "\t Xmax=" << Xmax << "\n" << "\tYmin=" << Ymin << "\t Ymax=" << Ymax << endl << endl;
     }

     bool isPointBelong(Coord& point)
     {
         return (Xmin <= point.x && point.x <= Xmax) && (Ymin <= point.y && point.y <= Ymax);
     }

public:
     double getDistance(HotSpot* hotSpot) {
         return sqrt((this->Xcenter - hotSpot->Xcenter) * (this->Xcenter - hotSpot->Xcenter)
                   + (this->Ycenter - hotSpot->Ycenter) * (this->Ycenter - hotSpot->Ycenter));
     }
};



class HotSpotReader
{
public:
    vector<HotSpot*>* readAllHotSpots(char* hotSpotDir)
    {
        vector<HotSpot*>* hotSpots = new vector<HotSpot*>();

        WIN32_FIND_DATA f;
        if (FindFirstFile(hotSpotDir, &f) == INVALID_HANDLE_VALUE)
        {
            fprintf(stderr, "File directory with hotSpots not found.\n");
            exit(-765);
        }

        char* hotSpotFileNamePattern = buildFullName(hotSpotDir, "*.hts");
        cout << "   hotSpotFileNamePattern: " << hotSpotFileNamePattern << endl << endl;

        HANDLE h = FindFirstFile(hotSpotFileNamePattern, &f);
        if(h != INVALID_HANDLE_VALUE)
        {
            do
            {
                char* hsInputFileName = buildFullName(hotSpotDir, f.cFileName);
                cout << "       hsInputFileName: " << hsInputFileName << endl;

                HotSpot* hotSpot = readHotSpot(hsInputFileName, f.cFileName);
                hotSpot->print();
                hotSpots->push_back(hotSpot);

                delete hsInputFileName;
            }
            while(FindNextFile(h, &f));
        }
        else
        {
            fprintf(stderr, "HotSpotReader: Directory or files not found\n");
        }
        delete hotSpotFileNamePattern;

        return hotSpots;
    }

    vector<HotSpotAvailability*>* readHotSpotsAvailabilities(char* hotSpotDir)
    {
        vector<HotSpotAvailability*>* hotSpotsAvailabilities = new vector<HotSpotAvailability*>();

        char* spotCountFileName = buildFullName(hotSpotDir, "spotcount.cnt");
        ifstream* spotCountFile = new ifstream(spotCountFileName);
        if (!spotCountFile)
        {
            cout << " No spotCountFile file: " << spotCountFileName << endl;
            exit(104);
        }

        while(!spotCountFile->eof()) {
            char hotSpotName[256];
            int count = -1;
            (*spotCountFile) >> hotSpotName >> count;
            if (count != -1) {
                hotSpotsAvailabilities->push_back(new HotSpotAvailability(hotSpotName, count));
            }
        }

        spotCountFile->close();
        delete spotCountFile;

        return hotSpotsAvailabilities;
    }

private:
    HotSpot* readHotSpot(char* fullFileName, char* simpleFileName)
    {
        ifstream* hotSpotFile = new ifstream(fullFileName);
        if (!hotSpotFile)
        {
            cout << " No hotSpotFile file: " << fullFileName << endl;
            exit(102);
        }

        double Xmin, Xmax, Ymin, Ymax;
        (*hotSpotFile) >> Xmin >> Xmax >> Ymin >> Ymax;
        hotSpotFile->close();
        delete hotSpotFile;

        return new HotSpot(simpleFileName, Xmin, Xmax, Ymin, Ymax);
    }
};

#endif // HOTSPOT_H_INCLUDED
