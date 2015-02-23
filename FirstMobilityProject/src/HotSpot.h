#ifndef HOTSPOT_H_INCLUDED
#define HOTSPOT_H_INCLUDED

#include <vector>

using namespace std;


struct Waypoint{
    double X,Y,Tb,Te;
    char* traceName;
};


struct HotSpot {
     double Xmin, Xmax, Ymin, Ymax;
     double sumTime;
     vector<Waypoint> waypoint;

     void print()
     {
         cout << "\tXmin=" << Xmin << "\t Xmax=" << Xmax << "\n" << "\tYmin=" << Ymin << "\t Ymax=" << Ymax << endl << endl;
     }

     bool isPointBelong(double x, double y)
     {
         return (Xmin <= x && x <= Xmax) && (Ymin <= y && y <= Ymax);
     }
};


class HotSpotReader
{
public:
    vector<HotSpot>* readAllHotSpots(char* hotSpotDir)
    {
        vector<HotSpot>* hotSpots = new vector<HotSpot>();

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

                HotSpot hotSpot = readHotSpot(hsInputFileName);
                hotSpot.print();
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
    HotSpot readHotSpot(char* fileName)
    {
        ifstream* hotSpotFile = new ifstream(fileName);
        if (!hotSpotFile)
        {
            cout << " No hotSpotFile file: " << fileName << endl;
            exit(102);
        }
        HotSpot hotSpot;
        (*hotSpotFile) >> hotSpot.Xmin >> hotSpot.Xmax >> hotSpot.Ymin >> hotSpot.Ymax;
        hotSpot.sumTime = 0;
        hotSpotFile->close();
        delete hotSpotFile;

        return hotSpot;
    }
};

#endif // HOTSPOT_H_INCLUDED
