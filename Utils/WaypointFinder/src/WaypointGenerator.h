#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <cmath>
#include <queue>
#include "Reader.h"
#include "Bounds.h"
#include "Area.h"

#ifndef WAYPOINTGENERATOR_INCLUDED
#define WAYPOINTGENERATOR_INCLUDED

using namespace std;


class WaypointGenerator {

private:
    Statistics* statistics; //Загруженная статистика из файла
    Bounds* bounds;         //граница генерации путевых точек
    Area* areaTree;         //дерево площадей для генерации

public:
    WaypointGenerator(char* boundsFileName, char* statFileName)
    {
        this->statistics = new Statistics(statFileName);
        this->bounds = new Bounds(boundsFileName);
        this->areaTree = Area::createTreeStructure(this->bounds);
    }

    ~WaypointGenerator()
    {
        if (this->statistics) delete this->statistics;
        if (this->bounds) delete this->bounds;
        if (this->areaTree) delete this->areaTree;
    }

    void generate(char* wayPointsFileName) {
        cout << "\t" << "WayPoints generating start..." << endl;
        generateNperArea();

        ofstream* wayPointsFile = new ofstream(wayPointsFileName);
        if (!wayPointsFile)
        {
            cout << endl << "\t" << "WaypointGenerator generate(): Output file " << wayPointsFileName << " opening failed." << endl;
            exit(344);
        }
        Area::writePoints(this->areaTree, wayPointsFile);
        wayPointsFile->close();
        delete wayPointsFile;
        cout << "\t" << "WayPoints generated!" << endl;
    }

private:
    void generateNperArea()
    {
        double totalSampledPoints = statistics->getSampledPoints();
        this->areaTree->setN(totalSampledPoints);

        queue<Area*> areasForProcess;
        areasForProcess.push(this->areaTree);

        for (int level = 0; level < Area::getLevels(); level++)
        {
            double dx = statistics->getRealDX(level); cout << "\t  Level = " << level << "  DX = " << dx << endl;

            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty())
            {
                Area* area = areasPerLevel.front();
                areasPerLevel.pop();

                if (area->getSubAreas())
                {
                    double n[4];
                    if (!GVrand(dx, n[0], n[1], n[2], n[3]))                       //todo брать из файла dx
                    {
                        cout << endl << "Negative value!" << endl;
                        cout << n[0] << "\t" << n[1] << "\t" << n[2] << "\t" << n[3] << "\t" << n[0]*n[0]+n[1]*n[1]+n[2]*n[2]+n[3]*n[3] << endl;
                        exit(234);
                    }

                    int remaining = area->getN();
                    for(int i = 0; i < Area::getSubAreasCount()-1; i++)
                    {
                        int subN = round(area->getN() * n[i]);                     //todo изменить округление
                        remaining -= subN;
                        while (remaining < 0) {subN--; remaining++;}
                        area->getSubAreas()[i]->setN(subN);
                        areasForProcess.push(area->getSubAreas()[i]);
                    }
                    //из-за округления сумма не получается равной начальному значению, поэтому последнюю часть рассчитыаем так
                    area->getSubAreas()[3]->setN(remaining);
                    areasForProcess.push(area->getSubAreas()[3]);

                    if (!area->validDistributionOfN())
                    {
                        cout << endl << "Incorrect distribution of N!" << endl;
                        cout << area->getN()*n[0] << "\t" << area->getN()*n[1] << "\t" << area->getN()*n[2] << "\t" <<
                                area->getN()*n[3] << "\tN=" << area->getN() << endl;
                        exit(235);
                    }
                }
            }
        }
    }

    bool GVrand(double R, double& q1, double& q2, double& q3, double& q4)
    {
        if( R<0 ) { cout<<"Wrong argument"<<endl; return false; }

        if( R>3 ) R=3;

        if( R>2 ) {
            double epsilon=(24-sqrt(24*24-4*39*(3-R)))/2/39;
            double q[]={epsilon, epsilon, epsilon, epsilon};
            q[rand()%4]=1-3*epsilon;
            q1=q[0]; q2=q[1]; q3=q[2]; q4=q[3];
            return true;
        }
        else {
            double r,phi,theta,z1,x2,x3;
            r=sqrt(R)/2;
            for(int i=0; i<1000; i++){
               phi=2*PI*(double)rand()/RAND_MAX;
               theta=PI*(double)rand()/RAND_MAX;

               z1=r*sin(theta)*cos(phi);
               x2=r*sin(theta)*sin(phi);
               x3=r*cos(theta);

               q1=z1/2/sqrt(3)-x3*2/sqrt(6)+0.25;
               q2=z1/2/sqrt(3)+x2/sqrt(2)+x3/sqrt(6)+0.25;
               q3=z1/2/sqrt(3)-x2/sqrt(2)+x3/sqrt(6)+0.25;
               q4=1-q1-q2-q3;

              if(q1>=0 && q2>=0 && q3>=0 && q4>=0 ) return true;
           }
        }
        return false;
    }

public:
    void testDVrand(double R = 1.0)
    {
        cout << "Test of generator..." << endl;
        double q1,q2,q3,q4;
        for(int i=0; i<1000; i++){
            if (GVrand(R, q1, q2, q3, q4))
                cout<<q1<<"\t"<<q2<<"\t"<<q3<<"\t"<<q4<<"\t"<<q1*q1+q2*q2+q3*q3+q4*q4<<endl;
            else
            {
                cout << endl << "Negative value!" << endl;
                cout<<q1<<"\t"<<q2<<"\t"<<q3<<"\t"<<q4<<"\t"<<q1*q1+q2*q2+q3*q3+q4*q4<<endl;
                exit(234);
            }
        }
        cout << "Test of generator finish!" << endl;
    }
};

#endif //WAYPOINTGENERATOR_INCLUDED
