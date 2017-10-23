#include <RootsPersistenceAndStatistics.h>

Define_Module(RootsPersistenceAndStatistics);

void RootsPersistenceAndStatistics::initialize() {
    ASSERT(persistenceFromMassCenter == -1);
    ASSERT(!rootsDimensionHistogramPDF);
    ASSERT(!summarizedIndicatorVectorPDF);
    ASSERT(!averageCounterVector);
    ASSERT(!hotSpots);

    // „тение данных из файла
    cXMLElement *rootStatistics = par("rootsPersistenceAndStatistics");


    cXMLElement* persistence = rootStatistics->getChildrenByTagName("PERSISTENCE").at(0);
    cXMLElement* massCenter = persistence->getChildrenByTagName("MASS-CENTER").at(0);
    cXMLElement* coef = massCenter->getChildrenByTagName("COEF").at(0);
    persistenceFromMassCenter = atof(coef->getNodeValue());

    vector<int>* rootsDimensionHistogram = NULL;
    loadData(rootStatistics, "ROOTS-DIMENSION-HISTOGRAM", "VALS", rootsDimensionHistogram, rootsDimensionHistogramPDF);
    ASSERT(rootsDimensionHistogram);
    ASSERT(rootsDimensionHistogramPDF);
    double rootsDimensionHistogramPDFSum = getSum(*rootsDimensionHistogramPDF);

    vector<int>* summarizedIndicatorVector = NULL;
    loadData(rootStatistics, "SUMMARIZED-INDICATOR-VECTOR", "VALS", summarizedIndicatorVector, summarizedIndicatorVectorPDF);
    ASSERT(summarizedIndicatorVector);
    ASSERT(summarizedIndicatorVectorPDF);
    double summarizedIndicatorVectorPDFSum = getSum(*summarizedIndicatorVectorPDF);

    loadData(rootStatistics, "AVERAGE-COUNTER-VECTOR", "VALS", averageCounterVector);
    ASSERT(averageCounterVector);

    loadData(rootStatistics, "HOT-SPOTS", "VALS", hotSpots);
    ASSERT(hotSpots);

    // ¬ывод на экран и проверка
    cout << "persistenceFromMassCenter: " << persistenceFromMassCenter << endl << endl;

    print("rootsDimensionHistogram", rootsDimensionHistogram);
    print("rootsDimensionHistogramPDF", rootsDimensionHistogramPDF);
    cout << "rootsDimensionHistogramPDFSum=" << rootsDimensionHistogramPDFSum << endl << endl;

    print("summarizedIndicatorVector", summarizedIndicatorVector);
    print("summarizedIndicatorVectorPDF", summarizedIndicatorVectorPDF);
    cout << "summarizedIndicatorVectorPDFSum=" << summarizedIndicatorVectorPDFSum << endl << endl;

    print("averageCounterVector", averageCounterVector);

    print("hotSpots", hotSpots);

    cout << "hotSpots->size() = " << hotSpots->size() << endl;
    cout << "rootsDimensionHistogramPDF->size() = " << rootsDimensionHistogramPDF->size() << endl;
    cout << "summarizedIndicatorVectorPDF->size() = " << summarizedIndicatorVectorPDF->size() << endl;
    cout << "averageCounterVector->size() = " << averageCounterVector->size() << endl << endl;

    // провер€ем согласованность размеров всех структур
    ASSERT((hotSpots->size() + 1) == rootsDimensionHistogramPDF->size()); // в rootsDimensionHistogramPDF на одно значение больше (из-за размерности 0)
    ASSERT(hotSpots->size() == summarizedIndicatorVectorPDF->size());
    ASSERT(hotSpots->size() == averageCounterVector->size());
}

void RootsPersistenceAndStatistics::loadData(cXMLElement *root, const char* histTag, const char* valsTag, vector<int>*& histogram, vector<double>*& histogramPDF) {
    cXMLElement* histogramXml = root->getChildrenByTagName(histTag).at(0);
    cXMLElement* histogramVals = histogramXml->getChildrenByTagName(valsTag).at(0);
    cStringTokenizer histogramTok(histogramVals->getNodeValue());
    histogram = new vector<int>(histogramTok.asIntVector());

    double histogramSum = 0;
    for (unsigned int i = 0; i < histogram->size(); i++) histogramSum += histogram->at(i);
    histogramPDF = new vector<double>(histogram->size());
    for (unsigned int i = 0; i < histogramPDF->size(); i++) histogramPDF->at(i) = (1.0 * histogram->at(i)) / histogramSum;
}

void RootsPersistenceAndStatistics::loadData(cXMLElement *root, const char* vecTag, const char* valsTag, vector<double>*& vec) {
    cXMLElement* vecXml = root->getChildrenByTagName(vecTag).at(0);
    cXMLElement* vecVals = vecXml->getChildrenByTagName(valsTag).at(0);
    cStringTokenizer vecTok(vecVals->getNodeValue());
    vec = new vector<double>(vecTok.asDoubleVector());
}

void RootsPersistenceAndStatistics::loadData(cXMLElement *root, const char* vecTag, const char* valsTag, vector<const char*>*& hotSpots) {
    cXMLElement* vecXml = root->getChildrenByTagName(vecTag).at(0);
    cXMLElement* vecVals = vecXml->getChildrenByTagName(valsTag).at(0);
    string tmp(vecVals->getNodeValue());

    // обрезаем символ перехода на новую строку в начале
    tmp = tmp.substr(1, tmp.size());

    cStringTokenizer vecTok(tmp.c_str());
    vecTok.setDelimiter("  ");

    hotSpots = new vector<const char*>();
    vector<string> hotSpotNamesVec = vecTok.asVector();
    for (unsigned int i = 0; i < hotSpotNamesVec.size(); i++) {
        string hotSpotNameStr = hotSpotNamesVec.at(i);
        if (!hotSpotNameStr.empty() && hotSpotNameStr.compare("\n") != 0) {
            const char* str = hotSpotNameStr.c_str();

            char* newStr = new char[256];
            newStr = strcpy(newStr, str);
            hotSpots->push_back(newStr);
        }
    }
}

unsigned int RootsPersistenceAndStatistics::findHotSpotIndexByName(const char* hotSpotName) {
    int index = -1;
    for(unsigned int i = 0; i < hotSpots->size(); i++)
        if (strcmp(hotSpots->at(i), hotSpotName) == 0) {
            index = i;
            break;
        }
    ASSERT(index != -1); //об€зательно должны найти! »наче это неконсистентные данные
    return index;
}
