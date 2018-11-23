#include <iostream>
#include <fstream>
#include <windows.h>
/**
 * Use this library http://www.boost.org
 * The description of usage is here http://www.boost.org/doc/libs/1_55_0/more/getting_started/windows.html
 * With IDE CLion it works this hint with https://stackoverflow.com/questions/3897839/how-to-link-c-program-with-boost-using-cmake
*/
#include "boost/multiprecision/float128.hpp"

using namespace std;
using namespace boost::multiprecision;

#define ASSERT_1(trueVal, errorCode) if(!(trueVal)){exit(errorCode);}
#define ASSERT_2(trueVal, errorCode, message) if(!(trueVal)){cout<<message<<endl;exit(errorCode);}

class SelfSimCalculator {
protected:
    int levels;
    long int arraySize;
    double Xmin, Xmax, Ymin, Ymax;
    unsigned int *pointsInArea;
    double *variance;
public:
    SelfSimCalculator(char *boundFile, int lvl);

    ~SelfSimCalculator();

    void loadWaypoints(char *WaypointFile);

    void loadAllDir(char *WaypoitDir);

    void calculateVariances();

    void showArray(int level);
};


SelfSimCalculator::SelfSimCalculator(char *boundFile, int lvl = 9) {
    int i;
    long int h4;

    ifstream *bfile = new ifstream(boundFile);
    if (bfile == NULL) {
        cout << "No bound file\n";
        exit(1);
    }
    (*bfile) >> Xmin >> Xmax >> Ymin >> Ymax;
    cout << "Area bounds:" << Xmin << "\t" << Xmax << "\t" << Ymin << "\t" << Ymax << endl;
    bfile->close();

    levels = lvl;
    for (i = 1, h4 = 1; i <= levels; i++) h4 *= 4;
    arraySize = (4 * h4 - 1) / 3;

    pointsInArea = new unsigned int[arraySize];
    if (pointsInArea == NULL) {
        cout << "out of memory\n";
        exit(3);
    }
    for (long int i = 0; i < arraySize; i++) pointsInArea[i] = 0;
    variance = new double[levels + 1];
}

SelfSimCalculator::~SelfSimCalculator() {
    delete[] pointsInArea;
    delete[] variance;
}


void SelfSimCalculator::loadWaypoints(char *WaypointFile) {
    double xcoord, ycoord, tb, te, xmin, xmax, ymin, ymax, hx, hy;
    int lvl;
    long int index;

    char file[256];
    strcpy(file, WaypointFile);

    ifstream *wfile = new ifstream(file);
    if (wfile == NULL) {
        cout << "No waypoint file\n";
        exit(2);
    }

    while (!wfile->eof()) {

        xmin = Xmin;
        xmax = Xmax;
        ymin = Ymin;
        ymax = Ymax;
        (*wfile) >> xcoord >> ycoord >> tb >> te;
        pointsInArea[0]++;

        for (index = 0, lvl = 1; lvl <= levels; lvl++) {
            if (xcoord <= (hx = (xmin + xmax) / 2)) {
                xmax = hx;
                if (ycoord <= (hy = (ymin + ymax) / 2)) {
                    ymax = hy;
                    index = 4 * index + 1;
                } else {
                    ymin = hy;
                    index = 4 * index + 3;
                }
            } else {
                xmin = hx;
                if (ycoord <= (hy = (ymin + ymax) / 2)) {
                    ymax = hy;
                    index = 4 * index + 2;
                } else {
                    ymin = hy;
                    index = 4 * index + 4;
                }
            }
            pointsInArea[index]++;
        }
    }
    wfile->close();
}


char *buildFullName(char *buffer, char *dir, char *fileName) {
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}


void SelfSimCalculator::loadAllDir(char *WaypoitDir) {
    WIN32_FIND_DATA f;

    char waypointFileNamePattern[256];
    buildFullName(waypointFileNamePattern, WaypoitDir, "*.wpt");

    HANDLE h = FindFirstFile(waypointFileNamePattern, &f);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            char inputFileName[256];
            buildFullName(inputFileName, WaypoitDir, f.cFileName);
            cout << "inputFileName: " << inputFileName << endl;

            this->loadWaypoints(inputFileName);
        } while (FindNextFile(h, &f));
    } else {
        fprintf(stderr, "Directory or files not found\n");
    }
}


void SelfSimCalculator::calculateVariances() {
    int lvl;
    int index, hsize;
    float128 MX2, MX;
    double MX2_Test, MX_Test;
    const double initialSquare = (Xmax - Xmin) * (Ymax - Ymin);

    for (lvl = 0, hsize = 1, index = 0; lvl <= levels; lvl++) {
        MX2 = MX = 0;
        MX2_Test = MX_Test = 0;
        for (long int i = 0; i < hsize; i++) {
            double points = pointsInArea[index + i] / (initialSquare / hsize);
            MX2 += float128(points) * float128(points);
            MX += float128(points);
            MX2_Test += points * points;
            MX_Test += points;
        }

        //region For debug
//        double MX2toDbl = double(MX2);
//        double MXMXtoDbl = double(MX * MX);
//        printf("%d\t TotalSquare= %12.0f,\t hsize = %d,\t SquareForLevel = %12.0f\n", lvl, initialSquare, hsize, (initialSquare / hsize));
//        printf("%d\t MX2      = %12.0f,\t hsize = %d,\t MX*MX          = %12.0f\n", lvl, MX2toDbl, hsize, MXMXtoDbl);
//        printf("%d\t MX2_Test = %12.0f,\t hsize = %d,\t MX_Test*MX_Test= %12.0f\n", lvl, MX2_Test, hsize, (MX_Test * MX_Test));
//        printf("%d\t delta_MX2= %12.0f,\t hsize = %d,\t delta_MX_Test= %12.0f\n", lvl, MX2toDbl - MX2_Test, hsize, MXMXtoDbl - (MX_Test * MX_Test));
//        ASSERT_1((MX2toDbl - MX2_Test) == 0, -123);
//        ASSERT_1((MXMXtoDbl - (MX_Test * MX_Test)) == 0, -124);
//        ASSERT_1((double(MX2 * float128(hsize)) - MX2_Test*hsize) == 0, -125);
        //endregion

        variance[lvl] = double(MX2 * float128(hsize) / (MX * MX)) - 1;
        cout << lvl << "\t" << variance[lvl] << endl;
        index += hsize;
        hsize *= 4;
    }

    double Mxy = 0, Mx = 0, My = 0, Mx2 = 0;
    for (lvl = 1; lvl <= levels; lvl++) {
        Mx += 2 * lvl;
        My += log2(variance[lvl]);
        Mx2 += 4 * lvl * lvl;
        Mxy += log2(variance[lvl]) * 2 * lvl;
    }
    Mx /= levels - 1;
    My /= levels - 1;
    Mx2 /= levels - 1;
    Mxy /= levels - 1;
    double c, b, H;
    b = (Mxy - Mx * My) / (Mx2 - Mx * Mx);
    c = My - b * Mx;
    H = 1 - fabs(b) / 2;

    cout << "b=" << b << "\t c=" << c << "\t H=" << H << endl;
    ofstream *file1 = new ofstream("herst.txt");
    (*file1) << "b=" << b << "\t c=" << c << "\t H=" << H << endl;
    ofstream *file2 = new ofstream("variances.txt");
    for (int i = 1; i <= levels; i++) (*file2) << i << "\t" << variance[i] << endl;
    file1->close();
    file2->close();
}

void SelfSimCalculator::showArray(int level) {
    long int hsize = 1, index = 0;
    for (int i = 0; i <= level; i++) {
        for (long int j = index; j < index + hsize; j++) cout << pointsInArea[j] << " ";
        index += hsize;
        hsize *= 4;
        cout << endl;
    }
}


int main(int argc, char **argv) {
    cout << "Hello world!" << endl;

    char *WaypointDir = "./waypointfiles";
    char *boundFile = "bounds.bnd";
    int lvl = 9;
    switch (argc) {
        case 1 :
            // оставляем значения по умолчанию
            break;
        case 2 :
            WaypointDir = argv[1];
            break;
        case 3 :
            WaypointDir = argv[1];
            boundFile = argv[2];
            break;
        case 4:
        default:
            WaypointDir = argv[1];
            boundFile = argv[2];
            lvl = atoi(argv[3]);
            break;
    }

    SelfSimCalculator h(boundFile, lvl);
    h.loadAllDir(WaypointDir);
    h.calculateVariances();

    return 0;
}
