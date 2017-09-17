#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>

using namespace std;

struct HotSpot {
    HotSpot(long counter, long double sumTime, long long int totalPoints)
            : counter(counter), sumTime(sumTime), totalPoints(totalPoints) {}

    long counter;           // кратность данной локации в рамках текущего маршрута
    long double sumTime;    // общая сумма времене, проведённая в локации в рамках данного маршрута
    long long totalPoints;  // общее количество путевых точек сделанных в данной локации в рамках данного маршрута
};


char *buildFullName(char *buffer, char *dir, const char *fileName) {
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}


class PersistenceCalculator {
protected:
    vector<char *> spotNames;            // все локации (имена соответствующих файлов)
    vector<vector<HotSpot *> *> roots;   // все маршруты, состоящие из наборов HotSpot

public:
    PersistenceCalculator(char *SpotDir, char *RootDi);

    ~PersistenceCalculator();

    double CalculatePersistence(unsigned int etalonRootNum);

    double CalculatePersistence(vector<HotSpot *> *etalonRoot, char *rootName);

    double CoefficientOfSimilarity(vector<HotSpot *> *root1, vector<HotSpot *> *root2);

    vector<HotSpot *> *GetMassCenter();

    void CalcAllAndSave();

    void GenerateRotFile(char *RootDir, char *SpotDir);
};

PersistenceCalculator::PersistenceCalculator(char *SpotDir, char *RootDir) {
    //загружаем локации
    char SpotNamePattern[256];
    buildFullName(SpotNamePattern, SpotDir, "*.hts");

    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(SpotNamePattern, &f);
    if (h != INVALID_HANDLE_VALUE) {
        do {
            char *sname = new char[256];
            strcpy(sname, f.cFileName);
            spotNames.push_back(sname);
        } while (FindNextFile(h, &f));
    }

    //загружаем маршруты
    char RootNamePattern[256];
    buildFullName(RootNamePattern, RootDir, "*.rot");

    char spot[256];
    long double time = 0;
    long long points = 0;
    long double Xmin, Xmax, Ymin, Ymax;

    WIN32_FIND_DATA f2;
    HANDLE h2 = FindFirstFile(RootNamePattern, &f2);
    if (h2 != INVALID_HANDLE_VALUE) {
        do {
            char *name = new char[256];
            name = buildFullName(name, RootDir, f2.cFileName);
            ifstream *rfile = new ifstream(name);
            vector<HotSpot *> *root = new vector<HotSpot *>();
            for (unsigned int i = 0; i < spotNames.size(); i++) root->push_back(new HotSpot(0, 0, 0));
            while (!rfile->eof()) {
                (*rfile) >> spot >> Xmin >> Xmax >> Ymin >> Ymax >> time >> points;
                for (unsigned int i = 0; i < spotNames.size(); i++)
                    if (strcmp(spotNames[i], spot) == 0) {
                        root->at(i)->counter++;
                        root->at(i)->sumTime += time;
                        root->at(i)->totalPoints += points;
                        break;
                    }
            }
            roots.push_back(root);
            rfile->close();
        } while (FindNextFile(h2, &f2));
    }
}

PersistenceCalculator::~PersistenceCalculator() {
    for (unsigned int i = 0; i < spotNames.size(); i++) delete[] spotNames[i];
    for (unsigned int i = 0; i < roots.size(); i++) {
        vector<HotSpot *> *root = roots.at(i);
        for (unsigned int j = 0; j < root->size(); j++) delete root->at(j);
        delete root;
    }
}

/**
    Расчёт коэффициента персистентности относительно какого либо маршрута из первоначального набора
*/
double PersistenceCalculator::CalculatePersistence(unsigned int etalonRootNum) {
    if (etalonRootNum < 0 && etalonRootNum >= roots.size()) exit(-111);

    vector<HotSpot *> *etalonRoot = roots.at(etalonRootNum);
    double coef = 0.0;
    for (unsigned int i = 0; i < roots.size(); i++)
        if (i != etalonRootNum) {
            double k = CoefficientOfSimilarity(etalonRoot, roots.at(i));
            cout << "K(" << etalonRootNum << "," << i << ")=" << k << endl;
            coef += k;
        }

    return coef / (roots.size() - 1); // roots.size() = L
}

/**
    Расчёт коэффициента персистентности относительно маршрута НЕ из первоначального набора
*/
double PersistenceCalculator::CalculatePersistence(vector<HotSpot *> *etalonRoot, char *rootName) {
    double coef = 0.0;
    for (unsigned int i = 0; i < roots.size(); i++) {
        double k = CoefficientOfSimilarity(etalonRoot, roots.at(i));
        cout << "K(" << rootName << "," << i << ")=" << k << endl;
        coef += k;
    }

    return coef / (roots.size() - 1); // roots.size() = L
}

#define MY_MAX(a, b) ((a>b)?a:b)

double PersistenceCalculator::CoefficientOfSimilarity(vector<HotSpot *> *root1, vector<HotSpot *> *root2) {
    if (root1->size() != root2->size()) exit(-222);

    double sumDiff = 0, sumOfMaxComponetns = 0, k = 0;
    for (unsigned int i = 0; i < root1->size(); i++) {
        sumDiff += abs(root1->at(i)->counter - root2->at(i)->counter);
    }
    for (unsigned int i = 0; i < root1->size(); i++) {
        sumOfMaxComponetns += MY_MAX(root1->at(i)->counter, root2->at(i)->counter);
    }

    k = 1 - sumDiff / sumOfMaxComponetns;

    if (k > 1) { // for debug
        cout << "sumDiff=" << sumDiff << endl;
        cout << "sumOfMaxComponetns=" << sumOfMaxComponetns << endl;
        cout << "sumDiff / sumOfMaxComponetns=" << (sumDiff / sumOfMaxComponetns) << endl;
        cout << endl;
        cout << "root1: " << endl;
        for (unsigned int i = 0; i < root1->size(); i++) {
            cout << root1->at(i)->counter << ", ";
            if ((i + 1) % 35 == 0) cout << endl;
        }
        cout << endl;
        cout << "root2: " << endl;
        for (unsigned int i = 0; i < root2->size(); i++) {
            cout << root2->at(i)->counter << ", ";
            if ((i + 1) % 35 == 0) cout << endl;
        }
        cout << endl;

        exit(-333);
    };
    return k;
}

/**
    Расчёт центра масс с многомерном пространстве, размерности spotNames.size()
    и векторами roots. Пространство натуральных чисел!!!
*/
vector<HotSpot *> *PersistenceCalculator::GetMassCenter() {
    vector<HotSpot *> *massCenter = new vector<HotSpot *>();
    for (unsigned int i = 0; i < roots.at(0)->size(); i++) {
        long double averageCounter = 0.0;
        long double averageSumTime = 0.0;
        long double averageTotalPoints = 0;
        for (unsigned int j = 0; j < roots.size(); j++) {
            vector<HotSpot *> *root = roots.at(j);
            averageCounter += root->at(i)->counter;
            averageSumTime += root->at(i)->sumTime;
            averageTotalPoints += root->at(i)->totalPoints;
        }
        averageCounter /= (1.0 * roots.size());
        averageSumTime /= (1.0 * roots.size());
        averageTotalPoints /= (1.0 * roots.size());
        if (averageCounter < 0 || averageSumTime < 0 || averageTotalPoints < 0) exit(-321);

        massCenter->push_back(new HotSpot(long(averageCounter + 0.5), averageSumTime, long(averageTotalPoints + 0.5)));
    }

    return massCenter;
}

/**
    Расчёт коэффициентов персистентности относительно всех маршрутов
    и сохранение результатов
*/
void PersistenceCalculator::CalcAllAndSave() {
    ofstream file("persistence.pst");
    cout << "Coefficients: " << endl;

    double averagePersistence = 0.0;
    for (unsigned int i = 0; i < roots.size(); i++) {
        double persistence = CalculatePersistence(i);
        file << i << "\t" << persistence << endl;
        cout << "\t" << i << "\t" << persistence << endl << endl;
        averagePersistence += persistence;
    }

    averagePersistence /= roots.size();
    file << "averagePersis" << "\t" << averagePersistence << endl;
    cout << "averagePersis" << "\t" << averagePersistence << endl << endl;

    vector<HotSpot *> *massCenter = GetMassCenter();
    double persistence = CalculatePersistence(massCenter, "massCenter");
    file << "massCenter" << "\t" << persistence << endl;
    cout << "\t" << "massCenter" << "\t" << persistence << endl << endl;
    file << "massCenterVector" << "\t";
    cout << "massCenterVector" << "\t";
    for (unsigned int i = 0; i < massCenter->size(); i++) {
        file << massCenter->at(i)->counter << " ";
        cout << massCenter->at(i)->counter << " ";
    }
    file.close();
    cout << endl << endl << endl;

    // for debug
    //for(unsigned int i=0; i<roots.size(); i++) {
    //    cout<<"root "<<i<<": ";
    //    for(unsigned int j=0; j<roots.at(i)->size(); j++)
    //    cout<<roots.at(i)->at(j)<<", ";
    //    cout<<endl;
    //}
    //cout<<"massCenter: ";
    //for(unsigned int j=0; j<massCenter->size(); j++)
    //cout<<massCenter->at(j)<<", ";
    //cout<<endl;
}

/**
   формирование файла *.rot со средним маршрутом
*/
void PersistenceCalculator::GenerateRotFile(char *RootDir, char *SpotDir) {
    char RootNamePattern[256];
    char SpotNamePattern[256];
    buildFullName(RootNamePattern, RootDir, "*.rot");
    WIN32_FIND_DATA f0;
    HANDLE h = FindFirstFile(RootNamePattern, &f0);
    char *sname0 = new char[256];
    char *sname01 = new char[256];
    strcpy(sname0, f0.cFileName);
    int i = 0;
    while (*(sname0 + i) != '_') {
        *(sname01 + i) = *(sname0 + i);
        i++;
    }
    strcat(sname01, "_persistence=");
    char str[80];
    char buff[50];
    vector<HotSpot *> *massCenter = GetMassCenter();
    double persistence = CalculatePersistence(massCenter, "massCenter");
    sprintf(str, "%f", persistence); //конвертация double в char
    strcat(sname01, str);
    strcat(sname01, "_.rot");
    ofstream file(sname01);
    for (unsigned int i = 0; i < massCenter->size(); i++) {
        for (unsigned int k = 0; k < massCenter->at(i)->counter; k++) {
            buildFullName(SpotNamePattern, SpotDir, spotNames[i]);
            ifstream fin(SpotNamePattern);
            file << spotNames[i] << " ";
            for (int j = 0; j < 4; j++) {
                fin >> buff;
                file << buff << " ";
            }
            file << (massCenter->at(i)->sumTime / massCenter->at(i)->counter)
                 << " " << (massCenter->at(i)->totalPoints / massCenter->at(i)->counter) << endl;
            fin.close();
        }
    }
    file.close();
}

int main(int argc, char **argv) {
    char *rootFilesDir;        //full path name of root files directory
    char *hotspotFilesDir;     //full path name of hot spot files directory
    switch (argc) {
        case 1 :
            rootFilesDir = "./rootfiles";
            hotspotFilesDir = "./hotspotfiles";
            break;
        case 2 :
            rootFilesDir = argv[1];
            hotspotFilesDir = "./hotspotfiles";
            break;
        case 3 :
        default:
            rootFilesDir = argv[1];
            hotspotFilesDir = argv[2];
            break;
    }

    PersistenceCalculator calc(hotspotFilesDir, rootFilesDir);
    calc.CalcAllAndSave();
    calc.GenerateRotFile(rootFilesDir, hotspotFilesDir);
    cout << endl << "Hello world!" << endl;
    return 0;
}
