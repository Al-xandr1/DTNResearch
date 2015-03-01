#ifndef HOTSPOT_H_INCLUDED
#define HOTSPOT_H_INCLUDED

#include <vector>

using namespace std;


struct Waypoint{
    double X,Y,Tb,Te;
    char* traceName;
};


struct HotSpot {
     double Xmin, Xmax, Ymin, Ymax, Xcenter, Ycenter;
     Coord vertixesByQudrant[4]; // содержит врешины прямоугольника, противополжные квадрантам, номер которых соответствует числу index+1
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

         this->vertixesByQudrant[0].x = Xmin;   //противоположная вершина для первого квадранта
         this->vertixesByQudrant[0].y = Ymin;   //противоположная вершина для первого квадранта

         this->vertixesByQudrant[1].x = Xmax;   //противоположная вершина для второго квадранта
         this->vertixesByQudrant[1].y = Ymin;   //противоположная вершина для второго квадранта

         this->vertixesByQudrant[2].x = Xmax;   //противоположная вершина для третьего квадранта
         this->vertixesByQudrant[2].y = Ymax;   //противоположная вершина для третьего квадранта

         this->vertixesByQudrant[3].x = Xmin;   //противоположная вершина для четвёртого квадранта
         this->vertixesByQudrant[3].y = Ymax;   //противоположная вершина для четвёртого квадранта
     }

     void print()
     {
         cout << "\tXmin=" << Xmin << "\t Xmax=" << Xmax << "\n" << "\tYmin=" << Ymin << "\t Ymax=" << Ymax << endl << endl;
     }

     bool isPointBelong(Coord& point)
     {
         return (Xmin <= point.x && point.x <= Xmax) && (Ymin <= point.y && point.y <= Ymax);
     }

private:
     // Получение номера квадранты в системе осей относительно центра данного HotSpot (нумерация против часовой стрелки)
     vector<int> getQuadrants(Coord& point)
     {
         vector<int> quadrants;
         if (point.x > Xcenter) {

             if (point.y > Ycenter) {
                 quadrants.push_back(1);

             } else if (point.y < Ycenter) {
                 quadrants.push_back(4);

             } else {//на границе между 1ым и 4ым
                 quadrants.push_back(1);
//                 quadrants.push_back(4);
             }
         }
         else if (point.x < Xcenter) {

             if (point.y > Ycenter) {
                 quadrants.push_back(2);

             } else if (point.y < Ycenter) {
                 quadrants.push_back(3);

             } else {
                 //на границе между 2ым и 3ым
                 quadrants.push_back(2);
//                 quadrants.push_back(3);
             }

         } else {

             if (point.y > Ycenter) {
                 //на границе между 1ым и 2ым
                 quadrants.push_back(1);
//                 quadrants.push_back(2);
             } else if (point.y < Ycenter) {
                 //на границе между 3ым и 4ым
                 quadrants.push_back(3);
//                 quadrants.push_back(4);
             } else {
                 //на границе между 1ым, 2ым, 3им и 4ым
                 quadrants.push_back(1);
//                 quadrants.push_back(2);
//                 quadrants.push_back(3);
//                 quadrants.push_back(4);
             }
         }

         return quadrants;
     }

public:
     // Получение вершины прямоугольной горячей точки, максимально удалённой от указанной точки point
     Coord getFarthestVertix(Coord& point) {
         vector<int> quadrants = getQuadrants(point);
         if (quadrants.size() < 1 || quadrants.size() > 4) {exit(-159);}

         int quadrant = -1;
         if (quadrants.size() == 1) {
             //можно ожднозначно выбрать противоположную вершину
             quadrant = quadrants[0];
         } else {
             //выбирать случайным образом, из нескольких противоположных вершин
             int index = rint(uniform(0, quadrants.size() - 1));
             quadrant = quadrants[index];
             exit(-666);
         }
         if (quadrant < 1 || quadrant > 4) {exit(-195);}

         return vertixesByQudrant[quadrant - 1];
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

        double Xmin, Xmax, Ymin, Ymax;
        (*hotSpotFile) >> Xmin >> Xmax >> Ymin >> Ymax;
        hotSpotFile->close();
        delete hotSpotFile;

        return HotSpot(Xmin, Xmax, Ymin, Ymax);
    }
};

#endif // HOTSPOT_H_INCLUDED
