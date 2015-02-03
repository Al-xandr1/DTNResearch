#ifndef AREA_H_INCLUDED
#define AREA_H_INCLUDED

using namespace std;

#define LEVELS 9
#define SUB_AREAS_COUNT 4
#define PI 3.1415926


class Area {

private:
    int n;              //count of point in this area
    double EX;
    double DX;
    Bounds* bound;

    Area** subAreas;

public:
    Area(Bounds* bound)
    {
        this->n = 0;
        this->EX = 0;
        this->DX = 0;
        this->bound = new Bounds(bound);
        this->subAreas = NULL;
    }

    Area(double XMin, double YMin, double XMax, double YMax)
    {
        this->n = 0;
        this->EX = 0;
        this->DX = 0;
        this->bound = new Bounds(XMin, YMin, XMax, YMax);
        this->subAreas = NULL;
    }

    ~Area()
    {
        if (this->bound) delete this->bound;
        if (this->subAreas)
        {
            for (int i=0; i<SUB_AREAS_COUNT; i++)
                if (this->subAreas[i])
                    delete this->subAreas[i];

            delete this->subAreas;
        }
    }

    void setN(int n) {this->n = n;}
    int getN() {return this->n;}
    double getEX() {return this->EX;}
    double getDX() {return this->DX;}
    Bounds* getBounds() {return this->bound;}

    bool isInArea(Point* point)
    {
        return (this->bound->getXMin() <= point->x && point->x <= this->bound->getXMax())
            && (this->bound->getYMin() <= point->y && point->y <= this->bound->getYMax());
    }

    bool putInArea(Point* point)
    {
        if (!isInArea(point)) {
            return false;
        }
        n++;

        if (subAreas) {
            for(int i=0; i<SUB_AREAS_COUNT; i++) {
                bool isPutted = subAreas[i]->putInArea(point);
                if (isPutted) {
                    return true;
                }
            }
            //unreachable statement
            exit(-111);
        }

        return true;
    }

    void distributePoints(int n, int level)
    {
        this->setN(n);
        if (this->subAreas)
        {
            double n[4];
            if (!Area::GVrand(1.0, n[0], n[1], n[2], n[3]))                //todo брать очередной R
            {
                cout << endl << "Negative value!" << endl;
                cout << n[0] << "\t" << n[1] << "\t" << n[2] << "\t" << n[3] << "\t" << n[0]*n[0]+n[1]*n[1]+n[2]*n[2]+n[3]*n[3] << endl;
                exit(234);
            }

            //из-за округления сумма не получается равной начальному значению
            int remaining = this->getN();
            for(int i = 0; i < SUB_AREAS_COUNT-1; i++)
            {
                int subN = round(this->getN() * n[i]);                     //todo изменить округление
                remaining -= subN;
                while (remaining < 0) {subN--; remaining++;}
                this->subAreas[i]->setN(subN);
            }
            this->subAreas[3]->setN(remaining);

            if (!this->validDistributionOfN())
            {
                cout << endl << "Incorrect distribution of N!" << endl;
                cout << this->getN()*n[0] << "\t" << this->getN()*n[1] << "\t" << this->getN()*n[2] << "\t" <<
                        this->getN()*n[3] << "\tN=" << this->getN() << endl;
                exit(235);
            }
        }
    }

    bool validDistributionOfN()
    {
        if (this->getN() < 0) return false;
        if (this->subAreas)
        {
            int checkSum = 0;
            for (int i = 0; i < SUB_AREAS_COUNT; i++)
            {
                if (this->subAreas[i]->getN() < 0) return false;
                checkSum += this->subAreas[i]->getN();
            }
            if (checkSum != this->getN()) return false;
        }
        return true;
    }

    void computeLocalExDx()
    {
        if (subAreas) {
            this->EX = 0;
            double ex2 = 0;
            for(int i=0; i<SUB_AREAS_COUNT; i++) {
                this->EX += subAreas[i]->n;
                ex2 += (subAreas[i]->n * subAreas[i]->n);
            }
            this->EX /= SUB_AREAS_COUNT;
            this->DX = (ex2 / SUB_AREAS_COUNT) - (this->EX * this->EX);
        }
    }

    static Area* createTreeStructure(Bounds* bounds)
    {
        Area* initialArea = new Area(bounds);

        queue<Area*> areasForProcess;
        areasForProcess.push(initialArea);

        for (int l=0; l<LEVELS; l++)
        {
            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty())
            {
                Area* area = areasPerLevel.front();
                areasPerLevel.pop();

                area->subAreas = new Area*[SUB_AREAS_COUNT];

                double middleX = (area->bound->getXMin() + area->bound->getXMax()) / 2;
                double middleY = (area->bound->getYMin() + area->bound->getYMax()) / 2;

                area->subAreas[0] = new Area(area->bound->getXMin(), middleY, middleX, area->bound->getYMax());//S1
                area->subAreas[1] = new Area(middleX, middleY, area->bound->getXMax(), area->bound->getYMax());//S2
                area->subAreas[2] = new Area(area->bound->getXMin(), area->bound->getYMin(), middleX, middleY);//S3
                area->subAreas[3] = new Area(middleX, area->bound->getYMin(), area->bound->getXMax(), middleY);//S4

                if (l != LEVELS - 1) {
                    for(int i=0; i<SUB_AREAS_COUNT; i++) areasForProcess.push(area->subAreas[i]);
                }
            }
        }

        return initialArea;
    }

    static double** computeExDx(Area* rootArea)
    {
        double* ExPerLevel = new double[LEVELS];
        double* DxPerLevel = new double[LEVELS];

        queue<Area*> areasForProcess;
        areasForProcess.push(rootArea);
        double areasCount = SUB_AREAS_COUNT;

        for (int l=0; l<LEVELS; l++)
        {
            ExPerLevel[l] = DxPerLevel[l] = 0;

            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty())
            {
                Area* area = areasPerLevel.front();
                areasPerLevel.pop();

                if (area->subAreas != NULL) {
                    for(int i=0; i<SUB_AREAS_COUNT; i++) {
                        ExPerLevel[l] += area->subAreas[i]->n;
                        DxPerLevel[l] += (area->subAreas[i]->n * area->subAreas[i]->n);

                        areasForProcess.push(area->subAreas[i]);
                    }
                }
            }

            ExPerLevel[l] /= areasCount;

            DxPerLevel[l] /= areasCount;
            DxPerLevel[l] /= ExPerLevel[l] * ExPerLevel[l];
            DxPerLevel[l] -= 1;

            areasCount *= SUB_AREAS_COUNT;
        }

        return new double*[2]{ExPerLevel, DxPerLevel};
    }

    static void writeStatistics(ofstream* out, Area* rootArea)
    {
        double** ExDxPerLevel = Area::computeExDx(rootArea);
        double areasCount = SUB_AREAS_COUNT;
        cout << endl << endl;
        cout << "\t<EX-DX-STAT>" << endl;
        *out << "  <EX-DX-STAT>" << endl;
        *out << "    <BASE>" << SUB_AREAS_COUNT << "</BASE>" << endl;
        *out << "    <LEVELS>" << LEVELS << "</LEVELS>" << endl;
        *out << "    <EX>";
        for(int l = 0; l < LEVELS; l++)
        {
            cout << "\t" << "Level= " << (l+1) << "  areas= " << areasCount << "\tEX=" << ExDxPerLevel[0][l] << "\tDX=" << ExDxPerLevel[1][l] << endl;
            areasCount *= SUB_AREAS_COUNT;
            *out << ExDxPerLevel[0][l];
            if (l != LEVELS-1) *out << "  ";
        }
        *out << "</EX>" << endl;
        *out << "    <DX>";
        for(int l = 0; l < LEVELS; l++)
        {
            *out << ExDxPerLevel[1][l];
            if (l != LEVELS-1) *out << "  ";
        }
        *out << "</DX>" << endl;
        *out << "  </EX-DX-STAT>" << endl;
        cout << "\t</EX-DX-STAT>" << endl;
        delete ExDxPerLevel[0];
        delete ExDxPerLevel[1];
        delete ExDxPerLevel;
    }

    //----------------------------------------need to replace this logic for generating points ----------------------------------------

    //Распихать метод по классу distributePoints(int n) & writeStatistics(ofstream* waypointFile)
    static void generateNperAreaAndSave(Area* rootArea, int initialN, ofstream* waypointFile)
    {
        bool firstRow = true;
        rootArea->setN(initialN);

        queue<Area*> areasForProcess;
        areasForProcess.push(rootArea);

        while (!areasForProcess.empty())
        {
            queue<Area*> areasPerLevel(areasForProcess);
            while(!areasForProcess.empty()) areasForProcess.pop();

            while(!areasPerLevel.empty())
            {
                Area* area = areasPerLevel.front();
                areasPerLevel.pop();

                if (area->subAreas)
                {
                    double n[4];
                    if (!Area::GVrand(1.0, n[0], n[1], n[2], n[3]))                //todo брать очередной R
                    {
                        cout << endl << "Negative value!" << endl;
                        cout << n[0] << "\t" << n[1] << "\t" << n[2] << "\t" << n[3] << "\t" << n[0]*n[0]+n[1]*n[1]+n[2]*n[2]+n[3]*n[3] << endl;
                        exit(234);
                    }

                    int remaining = area->getN();
                    for(int i=0; i<SUB_AREAS_COUNT-1; i++)
                    {
                        int subN = round(area->getN() * n[i]);                     //todo изменить округление
                        remaining -= subN;
                        while (remaining < 0) {subN--; remaining++;}
                        area->subAreas[i]->setN(subN);
                        areasForProcess.push(area->subAreas[i]);
                    }
                    //из-за округления сумма не получается равной начальному значению, поэтому последнюю часть рассчитыаем так
                    area->subAreas[3]->setN(remaining);
                    areasForProcess.push(area->subAreas[3]);

                    if (!area->validDistributionOfN())
                    {
                        cout << endl << "Incorrect distribution of N!" << endl;
                        cout << area->getN()*n[0] << "\t" << area->getN()*n[1] << "\t" << area->getN()*n[2] << "\t" <<
                                area->getN()*n[3] << "\tN=" << area->getN() << endl;
                        exit(235);
                    }
                } else
                {
                    for (int i = 0; i < area->getN(); i++)
                    {
                        if (firstRow) firstRow = false;
                        else (*waypointFile) << endl;
                        Point* randPoint = area->bound->generateRandomPoint();
                        (*waypointFile) << randPoint->x << "\t" << randPoint->y << "\t" << -1.0 << "\t" << -1.0; // todo отсутствие времени обработать
                        delete randPoint;
                    }
                }
            }
        }
    }

    static bool GVrand(double R, double& q1, double& q2, double& q3, double& q4)
    {
        double r,phi,theta,z1,x2,x3;

        r=sqrt(R)/2;
        for(int i=0; i<1000; i++)
        {
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
        return false;
    }

    static int testDVrand()
    {
        double q1,q2,q3,q4, R=1.0;
        for(int i=0; i<1000; i++){
            if (GVrand(R, q1, q2, q3, q4))
                cout<<q1<<"\t"<<q2<<"\t"<<q3<<"\t"<<q4<<"\t"<<q1*q1+q2*q2+q3*q3+q4*q4<<endl;
            else { cout<<"Negative value!"<<endl; exit(1); }
        }
        cout << "Hello world!" << endl;
        return 0;
    }
};

#endif // AREA_H_INCLUDED
