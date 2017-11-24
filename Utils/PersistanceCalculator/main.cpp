#include <iostream>
#include <fstream>
#include <vector>
#include <windows.h>
#include <limits>
#include <algorithm>

using namespace std;

// ��������� ���������� ��������� �� �� � ���� ����� ������� ������� ��� ���� �� �������, �������� ����. �����������
#define SAVE_MASS_CENTER_IN_ROOT false

#define ASSERT_1(trueVal, errorCode) if(!(trueVal)){exit(errorCode);}
#define ASSERT_2(trueVal, errorCode, message) if(!(trueVal)){cout<<message<<endl;exit(errorCode);}

#define myDelete(obj)      if(obj){delete   obj; obj = NULL;}
#define myDeleteArray(obj) if(obj){delete[] obj; obj = NULL;}

struct HotSpot {
    HotSpot(bool isHome, long counter, long double sumTime, long long int totalPoints)
            : isHome(isHome), counter(counter), sumTime(sumTime), totalPoints(totalPoints) {}

    bool isHome;            // ���� ����, ��� ������� ������� ��������
    long counter;           // ��������� ������ ������� � ������ �������� ��������
    long double sumTime;    // ����� ����� �������, ���������� � ������� � ������ ������� ��������
    long long totalPoints;  // ����� ���������� ������� ����� ��������� � ������ ������� � ������ ������� ��������
};

struct Histogram {
    Histogram(vector<int> *data, int countOfVals, double min, double max, double average)
            : data(data), countOfVals(countOfVals), max(max), min(min), average(average) {}

    vector<int> *data;      // �������� �����������
    int countOfVals;        // ���������� ������� �������� �����������
    double max;             // ������������ ��������
    double min;             // ����������� ��������
    double average;         // ������� ��������
};

/**
 * ��������, ����������� ���. �������� � ��������� �����
 */
struct MxDx {
    MxDx(double MX, double DX) : MX(MX), DX(DX) {}

    MxDx(const MxDx &anotherMxDx) : MX(anotherMxDx.MX), DX(anotherMxDx.DX) {}

    double MX;
    double DX;
};

char *buildFullName(char *buffer, char *dir, const char *fileName) {
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}


class PersistenceCalculator {
protected:
    vector<char *> spotNames;            // ��� ������� (����� ��������������� ������)
    vector<vector<HotSpot *> *> roots;   // ��� ��������, ��������� �� ������� HotSpot
    vector<string> rootFullFileNames;    // ����� ���� ���������

public:
    PersistenceCalculator(char *SpotDir, char *RootDi);

    ~PersistenceCalculator();

    MxDx calculateNewPersistence(unsigned int etalonRootNum);

    MxDx calculatePersistence(unsigned int etalonRootNum);

    double calculatePersistence(vector<HotSpot *> *etalonRoot, char *rootName);

    double coefficientOfSimilarity(vector<HotSpot *> *root1, vector<HotSpot *> *root2);

    vector<HotSpot *> *getMassCenter();

    vector<int> *getSummarizedIndicatorVector();

    vector<int> *getIndicationVector(vector<HotSpot *> *root);

    Histogram *getRootsDimensionsHistogram();

    Histogram *getRootsLengthHistogram();

    vector<int> *getHomeHotspotHistogram();

    vector<double> *getAverageCounterVector(vector<int> *summarizedIndicatorVector);

    void save(char *RootDir, char *SpotDir);

    const string getFilePrefix(char *RootDir);

    /**
     * ��������� ����� ��������� �������
    */
    template<typename T>
    inline T getSum(const std::vector<T> &vector) {
        T sum = 0;
        for (unsigned int i = 0; i < vector.size(); i++) sum += vector.at(i);
        return sum;
    }

    void saveAggregatedRotFile(char *SpotDir, const string &prefix, vector<HotSpot *> *vec, double coef);

    void saveOriginRotFile(const string &prefix, unsigned int rootNum, double coef);
};

PersistenceCalculator::PersistenceCalculator(char *SpotDir, char *RootDir) {
    //��������� �������
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

    //��������� ��������
    char RootNamePattern[256];
    buildFullName(RootNamePattern, RootDir, "*.rot");

    char hotSpotName[256];
    long double time = 0;
    long long points = 0;
    long double Xmin, Xmax, Ymin, Ymax;

    WIN32_FIND_DATA f2;
    HANDLE h2 = FindFirstFile(RootNamePattern, &f2);
    if (h2 != INVALID_HANDLE_VALUE) {
        do {
            char *fileName = new char[256];
            fileName = buildFullName(fileName, RootDir, f2.cFileName);
            ifstream *rfile = new ifstream(fileName);
            vector<HotSpot *> *root = new vector<HotSpot *>();
            for (unsigned int i = 0; i < spotNames.size(); i++) root->push_back(new HotSpot(false, 0, 0, 0));
            char *lastRedHotSpotName = NULL;
            bool foundHome = false;
            while (!rfile->eof()) {
                std::fill(&hotSpotName[0], &hotSpotName[0] + sizeof(hotSpotName), 0);
                (*rfile) >> hotSpotName >> Xmin >> Xmax >> Ymin >> Ymax >> time >> points;
                // ��-�� ��������������� ��������� ASSERT_2(time >= 0 && points >= 0, -1235, "time or points are wrong");

                //�������� ��������� ������: ��� ������ ���������� ������� ���� �� �����
                if (lastRedHotSpotName) {
                    bool nextIter = false;
                    // ���� true - ������ ���������� �� ����� ��������� ������ (��� � ����� ������ ������ �� ����� ������)
                    if (strcmp(lastRedHotSpotName, hotSpotName) == 0) nextIter = true;
                    myDeleteArray(lastRedHotSpotName);
                    if (nextIter) continue;
                }
                lastRedHotSpotName = strcpy(new char[256], hotSpotName);

                //����������� ������� �� �������� ������. ���������� ��������
                bool foundSpot = false;
                for (unsigned int i = 0; i < spotNames.size(); i++) {
                    if (strcmp(spotNames[i], hotSpotName) == 0) {
                        // ������ ������ � ����� - ��� �������� ������� - ���� � ��������� ����������� �� ��������
                        if (!foundHome) {
                            root->at(i)->isHome = true;
                            foundHome = true;
                        };
                        root->at(i)->counter++;
                        root->at(i)->sumTime += time;
                        root->at(i)->totalPoints += points;
                        foundSpot = true;
                        break;
                    }
                }
                ASSERT_2(foundSpot || (string(hotSpotName).length() == 0), -1234,
                         string("hotSpotName ='") + string(hotSpotName) + string("'"));
            }
            roots.push_back(root);
            rootFullFileNames.push_back(string(fileName));
            rfile->close();
            myDelete(rfile);
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
 * ������ ������ ������������ ��������������� ������������ ������ ���� �������� �� ��������������� ������:
 *
 * � ���� ��������� �������� - ��� ��������� ���������� ����������� ������� (�� ������������� �������,
 * �� ���� ��� ����� ��������� ���������), � ����� (� ���������� �������, ����� ��� ����� ��������� ���������).
 * �� � ����� ��� �������� ������ �������, � � ��� ���������� ��� ���������, � ���������� ������������ ���������
*/
MxDx PersistenceCalculator::calculateNewPersistence(unsigned int etalonRootNum) {
    ASSERT_1(etalonRootNum >= 0 && etalonRootNum < roots.size(), -1101);

    vector<int> *etalonIndicatorVector = getIndicationVector(roots.at(etalonRootNum));
    int etalonIndicatorVectorWeight = getSum(*etalonIndicatorVector);
    double coef = 0.0;
    double coef2 = 0.0;
    for (unsigned int i = 0; i < roots.size(); i++)
        if (i != etalonRootNum) {
            vector<int> *someIndicatorVector = getIndicationVector(roots.at(i));
            ASSERT_1(etalonIndicatorVector->size() == someIndicatorVector->size(), -1102);
            double intersectWeight = 0.0;
            for (unsigned int j = 0; j < etalonIndicatorVector->size(); j++) {
                intersectWeight += (((etalonIndicatorVector->at(j) == 1) && (someIndicatorVector->at(j) == 1)) ? 1 : 0);
            }
            intersectWeight /= (1.0 * std::max(etalonIndicatorVectorWeight, getSum(*someIndicatorVector)));
            myDelete(someIndicatorVector);

            cout << "New K(" << etalonRootNum << "," << i << ")=" << intersectWeight << endl;
            coef += intersectWeight;
            coef2 += (intersectWeight * intersectWeight);
        }
    myDelete(etalonIndicatorVector);

    // MX - ��� ���.�������� ������������ ������� ��� - ����������� ���������������
    double MX = coef / (roots.size() - 1); // roots.size() = L
    // DX - ��������� ������������ �������
    double DX = (coef2 / (roots.size() - 1)) - (MX * MX);

    return MxDx(MX, DX);
}

/**
 * ������ ������������ ��������������� ������������ ������ ���� �������� �� ��������������� ������
*/
MxDx PersistenceCalculator::calculatePersistence(unsigned int etalonRootNum) {
    ASSERT_1(etalonRootNum >= 0 && etalonRootNum < roots.size(), -110);

    vector<HotSpot *> *etalonRoot = roots.at(etalonRootNum);
    double coef = 0.0;
    double coef2 = 0.0;
    for (unsigned int i = 0; i < roots.size(); i++)
        if (i != etalonRootNum) {
            double k = coefficientOfSimilarity(etalonRoot, roots.at(i));
            cout << "K(" << etalonRootNum << "," << i << ")=" << k << endl;
            coef += k;
            coef2 += (k * k);
        }

    // MX - ��� ���.�������� ������������ ������� ��� - ����������� ���������������
    double MX = coef / (roots.size() - 1); // roots.size() = L
    // DX - ��������� ������������ �������
    double DX = (coef2 / (roots.size() - 1)) - (MX * MX);

    return MxDx(MX, DX);
}

/**
 * ������ ������������ ��������������� ������������ �������� �� �� ��������������� ������
*/
double PersistenceCalculator::calculatePersistence(vector<HotSpot *> *etalonRoot, char *rootName) {
    double coef = 0.0;
    for (unsigned int i = 0; i < roots.size(); i++) {
        double k = coefficientOfSimilarity(etalonRoot, roots.at(i));
        cout << "K(" << rootName << "," << i << ")=" << k << endl;
        coef += k;
    }

    return coef / (roots.size() - 1); // roots.size() = L
}

#define MY_MAX(a, b) ((a>b)?a:b)

double PersistenceCalculator::coefficientOfSimilarity(vector<HotSpot *> *root1, vector<HotSpot *> *root2) {
    ASSERT_1(root1->size() == root2->size(), -222);

    double sumDiff = 0, sumOfMaxComponents = 0, k = 0;
    bool homeFoundInFirst = false;
    bool homeFoundInSecond = false;
    for (unsigned int i = 0; i < root1->size(); i++) {
        long counter1 = root1->at(i)->counter;
        if (root1->at(i)->isHome) {
            ASSERT_1(!homeFoundInFirst, -111);
            counter1--;
            homeFoundInFirst = true;
        }
        ASSERT_1(counter1 >= 0, -112);

        long counter2 = root2->at(i)->counter;
        if (root2->at(i)->isHome) {
            ASSERT_1(!homeFoundInSecond, -113);
            counter2--;
            homeFoundInSecond = true;
        }
        ASSERT_1(counter2 >= 0, -114);

        sumDiff += abs(counter1 - counter2);
        sumOfMaxComponents += MY_MAX(counter1, counter2);
    }

    k = 1 - sumDiff / sumOfMaxComponents;

    if (k > 1) { // for debug
        cout << "sumDiff=" << sumDiff << endl;
        cout << "sumOfMaxComponents=" << sumOfMaxComponents << endl;
        cout << "sumDiff / sumOfMaxComponents=" << (sumDiff / sumOfMaxComponents) << endl;
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

        ASSERT_1(false, -333);
    };
    return k;
}

/**
 * ������ ������ ���� � ����������� ������������, ����������� spotNames.size()
 * � ��������� roots. ������������ ����������� �����!!!
*/
vector<HotSpot *> *PersistenceCalculator::getMassCenter() {
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
        // ��-�� ��������������� ��������� ASSERT_1(averageCounter >= 0 && averageSumTime >= 0 && averageTotalPoints >= 0, -321);

        //TODO ����������� ��������� �������� �������.... ��� Ũ ����������??? ������ ������ ������� � ��������� ���������� ����������� ��������
        massCenter->push_back(
                new HotSpot(false, long(averageCounter + 0.5), averageSumTime, long(averageTotalPoints + 0.5)));
    }

    return massCenter;
}

/**
 * ��������� ������������ ������� �� ���� ������� ��������� � �������� ����������� ������� ��������� ������� �� ���� �������� �������.
 * �.�. � ���������� ������ ��������� ����� ����� ���������� ����, � ������� ������������ ���� �� ��� ������� � �������.
 */
vector<int> *PersistenceCalculator::getSummarizedIndicatorVector() {
    vector<int> *summarizedIndicatorVector = new vector<int>(spotNames.size());
    for (unsigned int i = 0; i < summarizedIndicatorVector->size(); i++) summarizedIndicatorVector->at(i) = 0;

    for (unsigned int i = 0; i < roots.size(); i++) {
        vector<int> *indicatorVector = getIndicationVector(roots.at(i));
        ASSERT_1(summarizedIndicatorVector->size() == indicatorVector->size(), -4321);
        for (unsigned int j = 0; j < indicatorVector->size(); j++) {
            int indicator = indicatorVector->at(j);
            ASSERT_1(indicator == 0 || indicator == 1, -4432);
            summarizedIndicatorVector->at(j) += indicator;
        }
        delete indicatorVector;
    }

    return summarizedIndicatorVector;
}

/**
 * ����� �������� ����������� ����������� ���������
 * (���, �����, ����������� ����� �������� � ����������� ������������ ���������)
 */
Histogram *PersistenceCalculator::getRootsDimensionsHistogram() {
    vector<int> *data = new vector<int>(spotNames.size() + 1);
    // +1 � ������� �.�. ���� �������� ���������� �� 0 �� spotNames.size()
    for (unsigned int i = 0; i < data->size(); i++) data->at(i) = 0;

    double min = std::numeric_limits<double>::max(); // maximum value int;
    double max = std::numeric_limits<double>::min(); // minimum value int;
    double average = 0.0;

    for (unsigned int i = 0; i < roots.size(); i++) {
        vector<int> *indicatorVector = getIndicationVector(roots.at(i));
        ASSERT_1(data->size() == (indicatorVector->size() + 1), -4322);
        int weight = getSum(*indicatorVector);
        ASSERT_1(weight >= 0 && weight <= spotNames.size(), -4434);

        data->at((unsigned int) weight)++;
        average += weight;
        min = std::min(min, (double) weight);
        max = std::max(max, (double) weight);

        myDelete(indicatorVector);
    }

    return new Histogram(data, roots.size(), min, max, average / (1.0 * roots.size()));
}

/**
 * ����� �������� �����������, ������������ � ������� ����� ��������� (� ������ ����������)
 * ��� ���ר�� ����������� (���� � ��������� Histogram == NULL)
 */
Histogram *PersistenceCalculator::getRootsLengthHistogram() {
    double min = std::numeric_limits<double>::max(); // maximum value int;
    double max = std::numeric_limits<double>::min(); // minimum value int;
    double average = 0.0;

    for (unsigned int i = 0; i < roots.size(); i++) {
        vector<HotSpot *> *root = roots.at(i);
        double length = 0.0;
        for (unsigned int j = 0; j < root->size(); j++) {
            length += root->at(j)->counter;
        }

        average += length;
        min = std::min(min, length);
        max = std::max(max, length);
    }

    return new Histogram(NULL, roots.size(), min, max, average / (1.0 * roots.size()));
}

/**
 * �� ������� �������� �������� ������ ������������ ��������� (�� ��������� 0 � 1),
 * ������������ ���������� �� ������ ������� � ��������������� ����.
 *
 * @param root - �������, ��� �������� ����� �������� ������ ������
 */
vector<int> *PersistenceCalculator::getIndicationVector(vector<HotSpot *> *root) {
    vector<int> *indicatorVector = new vector<int>(root->size());
    for (unsigned int i = 0; i < indicatorVector->size(); i++) {
        ASSERT_1(root->at(i), -3211);
        indicatorVector->at(i) = (root->at(i)->counter != 0) ? 1 : 0;
    }
    return indicatorVector;
}

/**
 * ��������� ����������� �� ������ ���� �������� (������) ������� �� ���� �����.
 */
vector<int> *PersistenceCalculator::getHomeHotspotHistogram() {
    vector<int> *data = new vector<int>(spotNames.size());
    for (unsigned int i = 0; i < data->size(); i++) data->at(i) = 0;

    for (unsigned int i = 0; i < roots.size(); i++) {
        vector<HotSpot *> *root = roots.at(i);
        ASSERT_1(data->size() == root->size(), -4325);
        for (unsigned int j = 0; j < root->size(); j++) {
            if (root->at(j)->isHome) {
                // ��������� ������ ������� � ����������� � ������� �� �����
                data->at(j) += 1;
                break;
            }
        }
    }

    int checkSum = getSum(*data);
    ASSERT_1(checkSum <= roots.size(), -4326); // < ��-�� �������� � ����� ������ ������ ��������

    return data;
}

/**
 * ����� �������� ���������������� ������ �� ���� ���������, ��������� �� ��������� ������������ ������.
 * ����� ������� � ������ ���������� ���������� ������� ���������� ��������� �� �� ���,
 * � ������� ���� ���� �� ���� ���������.
 */
vector<double> *PersistenceCalculator::getAverageCounterVector(vector<int> *summarizedIndicatorVector) {
    vector<double> *averageCounterVector = new vector<double>(spotNames.size());
    for (unsigned int i = 0; i < averageCounterVector->size(); i++) averageCounterVector->at(i) = 0;

    for (unsigned int i = 0; i < roots.size(); i++) {
        vector<HotSpot *> *root = roots.at(i);
        ASSERT_1(averageCounterVector->size() == root->size(), -4332);
        for (unsigned int j = 0; j < root->size(); j++) {
            HotSpot *hotSpot = root->at(j);
            ASSERT_1(hotSpot->counter >= 0, -4432);
            averageCounterVector->at(j) += hotSpot->counter;
        }
    }

    ASSERT_1(averageCounterVector->size() == summarizedIndicatorVector->size(), -4132);
    for (unsigned int i = 0; i < averageCounterVector->size(); i++)
        if (summarizedIndicatorVector->at(i) != 0)
            averageCounterVector->at(i) = averageCounterVector->at(i) / (1.0 * summarizedIndicatorVector->at(i));
        else {
            // ���� ���������� ������������� ������� ����� ����, �� � ���������� ����� ���� ������ ���� �������
            ASSERT_1(averageCounterVector->at(i) == 0, -4430);
        }

    return averageCounterVector;
}

/**
 * ���������� ����������� � �����.
 */
void PersistenceCalculator::save(char *RootDir, char *SpotDir) {
    const string prefix = getFilePrefix(RootDir);


    /**
     * ������������ ����� ����������
     */
    ofstream out(prefix + string("_roots_persistence_statistics.pst"));
    out << "<?xml version=\"1.0\" ?>" << endl << endl;
    out << "<ROOT-STATISTICS info=\"Collected statistics for set of root files\">" << endl;

    //region PERSISTENCE
    out << "    <PERSISTENCE info=\"Data about persistence for all root files\">" << endl;
    out << "        <COEFFICIENTS info=\"1: ethalon root num; 2: coefficient of persistence (MX of coefficient of similarity); "
        << "3: Normalized standard deviation of coefficient of similarity\">" << endl;
    double averagePersistence = 0.0;
    for (unsigned int i = 0; i < roots.size(); i++) {
        MxDx mxdx = calculatePersistence(i);
        out << "            <COEF> " << i << "\t" << mxdx.MX << "\t" << (sqrt(mxdx.DX) / mxdx.MX) << " </COEF>" << endl;
        averagePersistence += mxdx.DX;
    }
    out << "        </COEFFICIENTS>" << endl;
    out << "        <AVERAGE-PERSISTENCE info=\"Average coefficient for persistence (based on the tag COEFFICIENTS)\"> "
        << (averagePersistence /= (1.0 * roots.size())) << " </AVERAGE-PERSISTENCE>" << endl;
    out << "        <MASS-CENTER info=\"Mass center for set of input vectors (roots)\"> " << endl;
    vector<HotSpot *> *massCenter = getMassCenter();
    const double massCenterPersistence = calculatePersistence(massCenter, (char *) "massCenter");
    out << "            <COEF info=\"Coefficient of persistence for mass center marked as ethalon\">"
        << massCenterPersistence << " </COEF>" << endl;
    out << "            <VALS info=\"Components of the mass center (counters for corresponding HOT-SPOTS)\"> " << endl;
    for (unsigned int i = 0; i < massCenter->size(); i++) out << massCenter->at(i)->counter << "  ";
    out << endl << "            </VALS> ";
    out << endl << "        </MASS-CENTER> ";
    out << endl << "    </PERSISTENCE>" << endl;
    //endregion


    //region NEW PERSISTENCE
    out << "    <NEW-PERSISTENCE info=\"Data about new persistence for all root files\">" << endl;
    out << "        <COEFFICIENTS info=\"1: ethalon root num; 2: coefficient of persistence (MX of coefficient of similarity); "
        << "3: Normalized standard deviation of coefficient of similarity\">" << endl;
    MxDx maxMx(0, 0);
    vector<unsigned int> *rootsWithMaxMx = new vector<unsigned int>();
    rootsWithMaxMx->push_back(roots.size() - 1); // �������� �� ���������, ���� ����� ���� ������ �� �����
    double averageNewPersistence = 0.0;
    for (unsigned int i = 0; i < roots.size(); i++) {
        MxDx mxdx = calculateNewPersistence(i);
        out << "            <COEF> " << i << "\t" << mxdx.MX << "\t" << (sqrt(mxdx.DX) / mxdx.MX) << " </COEF>" << endl;
        averageNewPersistence += mxdx.MX;
        if (mxdx.MX > maxMx.MX) {
            // ���� ��������� ������� ��� ������ ����������� ���������������, �� ...
            // ... ������� ��� ��������, � ������� ������������� ��������������� ...
            rootsWithMaxMx->clear();
            // ... � ���������� ���� "�������" ������� c ������� ������������� ���������������
            rootsWithMaxMx->push_back(i);
            maxMx = mxdx;
        } else if (mxdx.MX == maxMx.MX) {
            // ���� ��������� ������� ��� ����� �� ����������� ���������������, �� ���������� ���
            rootsWithMaxMx->push_back(i);
        }
    }
    ASSERT_1(rootsWithMaxMx->size() > 0, -4137);
    int ethalonRootNum = -1;
    // ������ �� ���� ��������� � ������������ ������������� ��������������� ����� ������� ����
    int maxDimension = -1;
    for (unsigned int i = 0; i < rootsWithMaxMx->size(); i++) {
        vector<int> *iv = getIndicationVector(roots.at(rootsWithMaxMx->at(i)));
        if (getSum(*iv) > maxDimension) {
            maxDimension = getSum(*iv);
            ethalonRootNum = rootsWithMaxMx->at(i);
        }
        myDelete(iv);
    }
    ASSERT_1(ethalonRootNum >= 0 && ethalonRootNum < roots.size(), -4138);
    vector<HotSpot *> *ethalonRoot = roots.at((unsigned int) ethalonRootNum);
    out << "        </COEFFICIENTS>" << endl;
    out << "        <AVERAGE-NEW-PERSISTENCE info=\"Average coefficient for new persistence (based on the tag COEFFICIENTS)\"> "
        << (averageNewPersistence /= (1.0 * roots.size())) << " </AVERAGE-NEW-PERSISTENCE>" << endl;
    out << "        <ETHALON-ROOT info=\"One root selected as ethalon from input roots\"> " << endl;
    out << "            <ROOT-NUM info=\"Number of root marked as ethalon\"> " << ethalonRootNum << " </ROOT-NUM> " << endl;
    out << "            <COEF info=\"Coefficient of persistence for root marked as ethalon\">" << maxMx.MX << " </COEF>" << endl;
    out << "            <VALS info=\"Components of the ethalon root (counters for corresponding HOT-SPOTS)\"> " << endl;
    for (unsigned int i = 0; i < ethalonRoot->size(); i++) out << ethalonRoot->at(i)->counter << "  ";
    out << endl << "            </VALS> ";
    out << endl << "        </ETHALON-ROOT> ";
    out << endl << "    </NEW-PERSISTENCE>" << endl;
    //endregion


    //region ROOT LENGTH HISTOGRAM
    Histogram *lengthHistogram = getRootsLengthHistogram();
    out << "    <ROOT-LENGTH-HISTOGRAM info=\"Statistics about root's length\">" << endl;
    out << "        <MIN> " << lengthHistogram->min << " </MIN>" << endl;
    out << "        <MAX> " << lengthHistogram->max << " </MAX>" << endl;
    out << "        <AVERAGE> " << lengthHistogram->average << " </AVERAGE>";
    out << endl << "    </ROOT-LENGTH-HISTOGRAM>" << endl;
    myDelete(lengthHistogram);
    //endregion


    //region ROOTS DIMENSIONS HISTOGRAM
    Histogram *dimensionsHistogram = getRootsDimensionsHistogram();
    out << "    <ROOTS-DIMENSION-HISTOGRAM info=\"Histogram of root's dimensions (sum of non zero root's components)\">" << endl;
    out << "        <MIN> " << dimensionsHistogram->min << " </MIN>" << endl;
    out << "        <MAX> " << dimensionsHistogram->max << " </MAX>" << endl;
    out << "        <AVERAGE> " << dimensionsHistogram->average << " </AVERAGE>" << endl;
    out << "        <COUNT-OF-VALS> " << dimensionsHistogram->countOfVals << " </COUNT-OF-VALS>" << endl;
    out << "        <VALS> " << endl;
    for (unsigned int i = 0; i < dimensionsHistogram->data->size(); i++) out << dimensionsHistogram->data->at(i) << "  ";
    out << endl << "        </VALS> " << endl;
    out << "    </ROOTS-DIMENSION-HISTOGRAM>" << endl;
    myDelete(dimensionsHistogram);
    //endregion


    //region SUMMARIZED INDICATOR VECTOR (HISTOGRAM)
    vector<int> *summarizedIndicatorVector = getSummarizedIndicatorVector();
    out << "    <SUMMARIZED-INDICATOR-VECTOR info=\"Each component of this vector - sum of the roots' components\">" << endl;
    out << "        <VALS> " << endl;
    for (unsigned int i = 0; i < summarizedIndicatorVector->size(); i++) out << summarizedIndicatorVector->at(i) << "  ";
    out << endl << "        </VALS> " << endl;
    out << "    </SUMMARIZED-INDICATOR-VECTOR>" << endl;
    //endregion


    //region AVERAGE COUNTER VECTOR
    vector<double> *averageCounterVector = getAverageCounterVector(summarizedIndicatorVector);
    out << "    <AVERAGE-COUNTER-VECTOR info=\"Calculated as: averageCounterVector[i] / summarizedIndicatorVector[i]\">" << endl;
    out << "        <VALS> " << endl;
    for (unsigned int i = 0; i < averageCounterVector->size(); i++) out << averageCounterVector->at(i) << "  ";
    out << endl << "        </VALS> " << endl;
    out << "    </AVERAGE-COUNTER-VECTOR>" << endl;
    //endregion


    //region HOME HOTSPOT HISTOGRAM
    vector<int> *homeHotspotHistogram = getHomeHotspotHistogram();
    out << "    <HOME-HOTSPOT-HISTOGRAM info=\"Histogram of home hotspots (made from all roots)\">" << endl;
    out << "        <VALS> " << endl;
    for (unsigned int i = 0; i < homeHotspotHistogram->size(); i++) out << homeHotspotHistogram->at(i) << "  ";
    out << endl << "        </VALS> " << endl;
    out << "    </HOME-HOTSPOT-HISTOGRAM>" << endl;
    //endregion


    //region HOT SPOTS (FOR DEBUG)
    out << "    <HOT-SPOTS info=\"Vector of corresponding hotspots\">" << endl;
    out << "        <VALS> " << endl;
    for (unsigned int i = 0; i < spotNames.size(); i++) out << spotNames.at(i) << "  ";
    out << endl << "        </VALS> " << endl;
    out << "    </HOT-SPOTS>" << endl;
    //endregion

    out << "</ROOT-STATISTICS>" << endl;
    cout << endl << "Persistence & PDFs are made!" << endl;


    /**
    * ������������ ����� *.rot � ��������� ���������
    */
    if (SAVE_MASS_CENTER_IN_ROOT)
        saveAggregatedRotFile(SpotDir, prefix, massCenter, massCenterPersistence);
    else
        saveOriginRotFile(prefix, (unsigned int) ethalonRootNum, maxMx.MX);
}

/**
 * ��������� �������������� ������� � ���� *.rot (�� ���������� ��������� sumTime & totalPoints �� ���������)
 */
void PersistenceCalculator::saveAggregatedRotFile(char *SpotDir, const string &prefix, vector<HotSpot *> *vec, double coef) {
    ofstream file(prefix + string("_persistence=") + string(to_string(coef).c_str()) + string("_.rot"));
    char SpotNamePattern[256];
    char buff[50];
    for (unsigned int i = 0; i < vec->size(); i++) {
        for (unsigned int k = 0; k < vec->at(i)->counter; k++) {
            buildFullName(SpotNamePattern, SpotDir, spotNames[i]);
            ifstream fin(SpotNamePattern);
            file << spotNames[i] << " ";
            for (int j = 0; j < 4; j++) {
                fin >> buff;
                file << buff << " ";
            }
            file << (vec->at(i)->sumTime / vec->at(i)->counter) << " " << (vec->at(i)->totalPoints / vec->at(i)->counter) << endl;
            fin.close();
        }
    }
    file.close();
    cout << endl << "rot file is generated!" << endl;
}

/**
 * ���������� ������������� �������� (������������ �� �����) ��� ����� ������.
 */
void PersistenceCalculator::saveOriginRotFile(const string &prefix, unsigned int rootNum, double coef) {
    ofstream outFile(prefix + string("_persistence=") + string(to_string(coef).c_str()) + string("_.rot"));
    ifstream rfile(rootFullFileNames.at(rootNum));
    string line;
    while (std::getline(rfile, line)) {
        outFile << line << endl;
    }
    rfile.close();
    outFile.close();
    cout << endl << "rot file with ORIGIN root is generated!" << endl;
}

const string PersistenceCalculator::getFilePrefix(char *RootDir) {
    char RootNamePattern[256];
    buildFullName(RootNamePattern, RootDir, "*.rot");
    WIN32_FIND_DATA f0;
    FindFirstFile(RootNamePattern, &f0);

    string fileName(f0.cFileName);
    std::size_t found = fileName.find("_id=");
    cout << "!!! fileName = '" << fileName << "'" << endl;
    ASSERT_1(found != std::string::npos, -115);
    string fileNamePrefix = fileName.substr(0, (found + 7));
    cout << "!!! fileNamePrefix = '" << fileNamePrefix << "'" << endl;
    return fileNamePrefix;
}

int main(int argc, char **argv) {
    char *rootFilesDir;        //full path name of root files directory
    char *hotspotFilesDir;     //full path name of hot spot files directory
    switch (argc) {
        case 1 :
            rootFilesDir = (char *) "./rootfiles";
            hotspotFilesDir = (char *) "./hotspotfiles";
            break;
        case 2 :
            rootFilesDir = argv[1];
            hotspotFilesDir = (char *) "./hotspotfiles";
            break;
        case 3 :
        default:
            rootFilesDir = argv[1];
            hotspotFilesDir = argv[2];
            break;
    }

    PersistenceCalculator calc(hotspotFilesDir, rootFilesDir);
    calc.save(rootFilesDir, hotspotFilesDir);
    return 0;
}
