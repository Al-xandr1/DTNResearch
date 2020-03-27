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

struct Waypoint {
    double X, Y, Tb, Te;
    char *traceName;
};

struct Cluster {
    double Xmin, Xmax, Ymin, Ymax;
    double sumTime;
    vector<Waypoint> waypoint;
};

struct Location {
    double Xmin, Xmax, Ymin, Ymax;
    double sumTime;
    unsigned long int wPoints;
    char name[256];
};

struct OptLoc {              // !!!
    double angle;
    double Xmin, Xmax, Ymin, Ymax;
    double sumTime;
    unsigned long int wPoints;
    char name[256];
};


char *buildFullName(char *buffer, char *dir, char *fileName) {
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}

class HotSpotFinder {
protected:

    double Delta, R2;  // main Cluster parameter (Delta=70m for R=100m )

    double Xmin, Xmax, Ymin, Ymax;
    int Xsize, Ysize;
    vector<Waypoint> **clusterWaypoint;

    bool skipEmptyHotSpots; // параметр, показывающий пропускать пустые локации или нет

    int NumOfHotSpots;
    vector<Location> Locs;
    vector<OptLoc> OptLocs;       // !!!

public:
    HotSpotFinder(char *boundFile, bool skipEmptyHotSpots, double delta, double r);

    void LoadWaypointsFromDir(char *waypointDir);

    bool isMergeable(vector<Waypoint> &wp1, vector<Waypoint> &wp2);

    void mergeClusterToHotSpot(Cluster &hotSpot, vector<Waypoint> &clW);

    void writeHotSpot(Cluster &cls, char *SpotFileName);

    void printHotSpot(Cluster &HotSpot);

    void makeAllSpots(char *hotSpotFilesDir);

    void optimizeHotSpotAngle(Cluster& HotSpot, OptLoc& optLc);   // !!!
};

HotSpotFinder::HotSpotFinder(char *boundFile, bool skipEmptyHotSpots = false, double delta = 70, double r = 100) {
    Delta = delta;
    R2 = r * r;

    NumOfHotSpots = 0;
    this->skipEmptyHotSpots = skipEmptyHotSpots;

    ifstream *bf = new ifstream(boundFile);
    if (bf == NULL) {
        cout << " No bound file\n";
        exit(1);
    }
    (*bf) >> Xmin >> Xmax >> Ymin >> Ymax;
    bf->close();
    cout << "Read from bound file:\n";
    cout << "Xmin=" << Xmin << "\t Xmax=" << Xmax << "\n" << "Ymin=" << Ymin << "\t Ymax=" << Ymax << endl << endl;

    Xmin = floor(Xmin / Delta) * Delta;
    Xmax = ceil(Xmax / Delta) * Delta;
    Xsize = (int) ceil((Xmax - Xmin) / Delta);
    Ymin = floor(Ymin / Delta) * Delta;
    Ymax = ceil(Ymax / Delta) * Delta;
    Ysize = (int) ceil((Ymax - Ymin) / Delta);
    cout << "Adjusted to Delta grid:\n";
    cout << "Xmin=" << Xmin << "\t Xmax=" << Xmax << "\n" << "Ymin=" << Ymin << "\t Ymax=" << Ymax << endl << endl;
    cout << "Xsize=" << Xsize << "\t Ysize=" << Ysize << endl;

    clusterWaypoint = new vector<Waypoint> *[Ysize];
    for (int i = 0; i < Ysize; i++) clusterWaypoint[i] = new vector<Waypoint>[Xsize];
}


void HotSpotFinder::LoadWaypointsFromDir(char *waypointDir) {
    char waypointFileNamePattern[256];
    buildFullName(waypointFileNamePattern, waypointDir, "*.wpt");
    cout << "   traceFileNamePattern: " << waypointFileNamePattern << endl << endl;

    Waypoint wp;
    int i, k, counter = 0;

    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(waypointFileNamePattern, &f);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            char inputFileName[256];
            buildFullName(inputFileName, waypointDir, f.cFileName);

            ifstream *file = new ifstream(inputFileName);
            wp.traceName = new char[strlen(f.cFileName) + 1];
            strcpy(wp.traceName, f.cFileName);

            while (!file->eof()) {
                (*file) >> wp.X >> wp.Y >> wp.Tb >> wp.Te;
                i = (int) floor((wp.Y - Ymin) / Delta);
                k = (int) floor((wp.X - Xmin) / Delta);
                clusterWaypoint[i][k].push_back(wp);
                counter++;
            }
            cout << endl << counter << " waypoints were loaded from file " << f.cFileName << endl;
            file->close();
        } while (FindNextFile(h, &f));
    } else {
        fprintf(stderr, "Directory or files not found\n");
    }
}


bool HotSpotFinder::isMergeable(vector<Waypoint> &wp1, vector<Waypoint> &wp2) {
    double dx, dy;
    unsigned int i, j;

    if (wp1.size() == 0 || wp2.size() == 0) return true;

    for (i = 0; i < wp1.size(); i++)
        for (j = 0; j < wp2.size(); j++) {
            dx = (wp1[i]).X - (wp2[j]).X;
            dy = (wp1[i]).Y - (wp2[j]).Y;
            if (dx * dx + dy * dy <= R2) return true;
        }
    return false;
}

void HotSpotFinder::mergeClusterToHotSpot(Cluster &hotSpot, vector<Waypoint> &clW) {
    Waypoint h;

    if (clW.size() == 0) return;

    h = clW[0];
    if ((hotSpot.waypoint).size() == 0) {
        hotSpot.Xmin = hotSpot.Xmax = h.X;
        hotSpot.Ymin = hotSpot.Ymax = h.Y;
        hotSpot.sumTime = h.Te - h.Tb;
        (hotSpot.waypoint).push_back(h);
    } else {
        if (h.X > hotSpot.Xmax) hotSpot.Xmax = h.X;
        if (h.X < hotSpot.Xmin) hotSpot.Xmin = h.X;
        if (h.Y > hotSpot.Ymax) hotSpot.Ymax = h.Y;
        if (h.Y < hotSpot.Ymin) hotSpot.Ymin = h.Y;
        hotSpot.sumTime += h.Te - h.Tb;
        (hotSpot.waypoint).push_back(h);
    }

    for (unsigned int i = 1; i < clW.size(); i++) {
        h = clW[i];
        if (h.X > hotSpot.Xmax) hotSpot.Xmax = h.X;
        if (h.X < hotSpot.Xmin) hotSpot.Xmin = h.X;
        if (h.Y > hotSpot.Ymax) hotSpot.Ymax = h.Y;
        if (h.Y < hotSpot.Ymin) hotSpot.Ymin = h.Y;
        hotSpot.sumTime += h.Te - h.Tb;
        (hotSpot.waypoint).push_back(h);
    }
}


void HotSpotFinder::makeAllSpots(char *hotSpotFilesDir) {
    int i, k;
    unsigned int j;
    vector<int> iq, kq;
    Cluster HotSpot;

    WIN32_FIND_DATA f;
    if (FindFirstFile(hotSpotFilesDir, &f) == INVALID_HANDLE_VALUE) {
        if (CreateDirectory(hotSpotFilesDir, NULL))
            cout << "create output directory " << endl;
        else
            cout << "error create output directory" << endl;
    }

    for (i = 0; i < Ysize; i++)
        for (k = 0; k < Xsize; k++)
            if ((clusterWaypoint[i][k]).size() > 0) {
                iq.push_back(i);
                kq.push_back(k);
            };

    while (iq.size() > 0) {
        (HotSpot.waypoint).clear();
        mergeClusterToHotSpot(HotSpot, clusterWaypoint[iq[0]][kq[0]]);
        iq.erase(iq.begin());
        kq.erase(kq.begin());
        j = 0;
        while (j < iq.size()) {
            if (isMergeable(HotSpot.waypoint, clusterWaypoint[iq[j]][kq[j]])) {
                mergeClusterToHotSpot(HotSpot, clusterWaypoint[iq[j]][kq[j]]);
                iq.erase(iq.begin() + j);
                kq.erase(kq.begin() + j);
            } else j++;
        }
        if (skipEmptyHotSpots && (HotSpot.Xmax == HotSpot.Xmin || HotSpot.Ymax == HotSpot.Ymin)) {
            continue;
        }
        NumOfHotSpots++;
        cout << "Cluster number " << NumOfHotSpots << endl;
        printHotSpot(HotSpot);

        char num[5];
        char name[128];
        char buffer[256];
        sprintf(num,"%d",NumOfHotSpots);
        strcpy(name, "hotSpot");
        strcat(name, num);
        strcat(name, ".hts");
        buildFullName(buffer, hotSpotFilesDir, name);
        writeHotSpot(HotSpot, buffer);

        Location lc;
        lc.Xmin = HotSpot.Xmin;
        lc.Xmax = HotSpot.Xmax;
        lc.Ymin = HotSpot.Ymin;
        lc.Ymax = HotSpot.Ymax;
        lc.sumTime = HotSpot.sumTime;
        lc.wPoints = (HotSpot.waypoint).size();
        strcpy(lc.name, name);
        Locs.push_back(lc);

        OptLoc optLc;     // !!!
        optimizeHotSpotAngle(HotSpot, optLc);
        strcpy(optLc.name, name);
        OptLocs.push_back(optLc);

    }

    ofstream lcfile("locations.loc");
    for (unsigned int i = 0; i < Locs.size(); i++) {
        lcfile << "0\t";
        lcfile << (Locs[i]).name << "\t" << (Locs[i]).sumTime << "\t" << "\t" << (Locs[i]).wPoints << "\t" << "\t";
        lcfile << (Locs[i]).Xmin << "\t" << (Locs[i]).Xmax << "\t";
        lcfile << (Locs[i]).Ymin << "\t" << (Locs[i]).Ymax << endl;
    }
    lcfile.close();

    ofstream opfile("optlocs.loc");
    for(unsigned int i=0; i<OptLocs.size(); i++) {
        opfile<<(OptLocs[i]).angle<<"\t";
        opfile<<(OptLocs[i]).name<<"\t"<<(OptLocs[i]).sumTime<<"\t"<<"\t"<<(OptLocs[i]).wPoints<<"\t"<<"\t";
        opfile<<(OptLocs[i]).Xmin<<"\t"<<(OptLocs[i]).Xmax<<"\t";
        opfile<<(OptLocs[i]).Ymin<<"\t"<<(OptLocs[i]).Ymax<<endl;
    }
    opfile.close();
}


// !!!!!!!!!!!!!!!!!!!!!!!!
void HotSpotFinder::optimizeHotSpotAngle(Cluster& HotSpot, OptLoc& optLc)
{
    double AngleStep = 5*2*3.1415926/360; // шаг 5 градусов
    int S=(int)ceil((3.1415926/2-AngleStep)/AngleStep);
    double x_min[S], x_max[S], y_min[S], y_max[S], min_square, opt_angle;
    int opt_index;

    for(int i=0; i<S; i++) {
        x_min[i] = x_max[i]=cos(i*AngleStep)*(HotSpot.waypoint)[0].X - sin(i*AngleStep)*(HotSpot.waypoint)[0].Y;
        y_min[i] = y_max[i]=sin(i*AngleStep)*(HotSpot.waypoint)[0].X + cos(i*AngleStep)*(HotSpot.waypoint)[0].Y;
    }

    for(unsigned int w=1; w<(HotSpot.waypoint).size(); w++){
        for(int i=0; i<S; i++) {
             double x,y;
             x = cos(i*AngleStep)*(HotSpot.waypoint)[w].X - sin(i*AngleStep)*(HotSpot.waypoint)[w].Y;
             y = sin(i*AngleStep)*(HotSpot.waypoint)[w].X + cos(i*AngleStep)*(HotSpot.waypoint)[w].Y;
             if(x>x_max[i]) x_max[i]=x;
             if(x<x_min[i]) x_min[i]=x;
             if(y>y_max[i]) y_max[i]=y;
             if(y<y_min[i]) y_min[i]=y;
        }
    }

    opt_index=0;
    opt_angle=0;
    min_square=(x_max[0]-x_min[0])*(y_max[0]-y_min[0]);
    for(int i=1; i<S; i++)
        if(min_square>(x_max[i]-x_min[i])*(y_max[i]-y_min[i])) {
             min_square=(x_max[i]-x_min[i])*(y_max[i]-y_min[i]);
             opt_index=i;
             opt_angle=i*AngleStep;
        }

    optLc.angle=opt_angle;
    optLc.Xmin=x_min[opt_index]; optLc.Xmax=x_max[opt_index]; optLc.Ymin=y_min[opt_index]; optLc.Ymax=y_max[opt_index];
    optLc.sumTime=HotSpot.sumTime; optLc.wPoints=(HotSpot.waypoint).size();
}

void HotSpotFinder::writeHotSpot(Cluster &HotSpot, char *SpotFileName) {
    ofstream file(SpotFileName);
    file << HotSpot.Xmin << "\t" << HotSpot.Xmax << endl;
    file << HotSpot.Ymin << "\t" << HotSpot.Ymax << endl;
    file << HotSpot.sumTime << "\t" << (HotSpot.waypoint).size() << endl;
    for (unsigned int i = 0; i < (HotSpot.waypoint).size(); i++)
        file << (HotSpot.waypoint[i]).X << "\t" << (HotSpot.waypoint[i]).Y << "\t"
             << (HotSpot.waypoint[i]).Tb << "\t" << (HotSpot.waypoint[i]).Te << "\t"
             << (HotSpot.waypoint[i]).traceName << endl;
    file.close();
}

void HotSpotFinder::printHotSpot(Cluster &HotSpot) {
    cout << "Xmin=" << HotSpot.Xmin << "\t Xmax=" << HotSpot.Xmax << endl;
    cout << "Ymin=" << HotSpot.Ymin << "\t Ymax=" << HotSpot.Ymax << endl;
    cout << "Total time in cluster:" << HotSpot.sumTime << "sec \t Number of waypoints:" << (HotSpot.waypoint).size()
         << endl;
    for (unsigned int i = 0; i < (HotSpot.waypoint).size(); i++)
        cout << (HotSpot.waypoint[i]).X << "\t" << (HotSpot.waypoint[i]).Y << "\t"
             << (HotSpot.waypoint[i]).Tb << "\t" << (HotSpot.waypoint[i]).Te << "\t"
             << (HotSpot.waypoint[i]).traceName << endl;
}


int main(int argc, char **argv) {
    cout << "Hello world!" << endl;

    char *wayPointFilesDir;    //full path name of directory
    char *hotSpotFilesDir; //full path name of directory
    switch (argc) {
        case 1 :
            wayPointFilesDir = "./waypointfiles";
            hotSpotFilesDir = "./hotspotfiles";
            break;
        case 2 :
            wayPointFilesDir = argv[1];
            hotSpotFilesDir = "./hotspotfiles";
            break;
        case 3 :
        default:
            wayPointFilesDir = argv[1];
            hotSpotFilesDir = argv[2];
            break;
    }

    HotSpotFinder HSFinder("bounds.bnd", true);

    HSFinder.LoadWaypointsFromDir(wayPointFilesDir);

    HSFinder.makeAllSpots(hotSpotFilesDir);

    return 0;
}
