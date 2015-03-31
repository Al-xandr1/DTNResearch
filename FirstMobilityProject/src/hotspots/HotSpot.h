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


struct HotSpot {
     double Xmin, Xmax, Ymin, Ymax, Xcenter, Ycenter;
     double sumTime;
     vector<Waypoint> waypoint;

     HotSpot(double Xmin, double Xmax, double Ymin, double Ymax)
     {
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

                HotSpot* hotSpot = readHotSpot(hsInputFileName);
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

private:
    HotSpot* readHotSpot(char* fileName)
    {
        ifstream* hotSpotFile = new ifstream(fileName);
        if (!hotSpotFile)
        {
            cout << " No hotSpotFile file: " << fileName << endl;
            exit(102);
        }

        double Xmin, Xmax, Ymin, Ymax;
        (*hotSpotFile) >> Xmin >> Xmax >> Ymin >> Ymax;
        hotSpotFile->close();
        delete hotSpotFile;

        return new HotSpot(Xmin, Xmax, Ymin, Ymax);
    }
};

#endif // HOTSPOT_H_INCLUDED
