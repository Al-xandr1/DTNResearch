#include <RootsPersistenceAndStatistics.h>

Define_Module(RootsPersistenceAndStatistics);

void RootsPersistenceAndStatistics::initialize() {
    ASSERT(persistenceFromMassCenter == -1);
    ASSERT(!rootsDimensionHistogramPDF);
    ASSERT(!summarizedIndicatorVectorPDF);
    ASSERT(!summarizedRootPDF);


    cXMLElement *rootStatistics = par("rootsPersistenceAndStatistics");


    cXMLElement* persistence = rootStatistics->getChildrenByTagName("PERSISTENCE").at(0);
    cXMLElement* massCenter = persistence->getChildrenByTagName("MASS-CENTER").at(0);
    cXMLElement* coef = massCenter->getChildrenByTagName("COEF").at(0);
    persistenceFromMassCenter = atof(coef->getNodeValue());

    vector<int>* rootsDimensionHistogram = NULL;
    loadData(rootStatistics, "ROOTS-DIMENSION-HISTOGRAM", "VALS", rootsDimensionHistogram, rootsDimensionHistogramPDF);
    ASSERT(rootsDimensionHistogram);
    ASSERT(rootsDimensionHistogramPDF);
    double rootsDimensionHistogramPDFSum = getSum(rootsDimensionHistogramPDF);
//    ASSERT(rootsDimensionHistogramPDFSum == 1.0);

    vector<int>* summarizedIndicatorVector = NULL;
    loadData(rootStatistics, "SUMMARIZED-INDICATOR-VECTOR", "VALS", summarizedIndicatorVector, summarizedIndicatorVectorPDF);
    ASSERT(summarizedIndicatorVector);
    ASSERT(summarizedIndicatorVectorPDF);
    double summarizedIndicatorVectorPDFSum = getSum(summarizedIndicatorVectorPDF);
//    ASSERT(summarizedIndicatorVectorPDFSum == 1.0);

    vector<int>* summarizedRoot = NULL;
    loadData(rootStatistics, "SUMMARIZED-ROOT", "VALS", summarizedRoot, summarizedRootPDF);
    ASSERT(summarizedRoot);
    ASSERT(summarizedRootPDF);
    double summarizedRootPDFSum = getSum(summarizedRootPDF);
//    ASSERT(summarizedRootPDFSum == 1.0);

    cout << "persistenceFromMassCenter: " << persistenceFromMassCenter << endl << endl;

    print("rootsDimensionHistogram", rootsDimensionHistogram);
    print("rootsDimensionHistogramPDF", rootsDimensionHistogramPDF);
    cout << "rootsDimensionHistogramPDFSum=" << rootsDimensionHistogramPDFSum << endl << endl;

    print("summarizedIndicatorVector", summarizedIndicatorVector);
    print("summarizedIndicatorVectorPDF", summarizedIndicatorVectorPDF);
    cout << "summarizedIndicatorVectorPDFSum=" << summarizedIndicatorVectorPDFSum << endl << endl;

    print("summarizedRoot", summarizedRoot);
    print("summarizedRootPDF", summarizedRootPDF);
    cout << "summarizedRootPDFSum=" << summarizedRootPDFSum << endl << endl;
}

void RootsPersistenceAndStatistics::loadData(cXMLElement *root, const char* histTag, const char* valsTag, vector<int>*& histogram, vector<double>*& histogramPDF) {
    cXMLElement* histogramXml = root->getChildrenByTagName(histTag).at(0);
    cXMLElement* histogramVals = histogramXml->getChildrenByTagName(valsTag).at(0);
    cStringTokenizer histogramTok(histogramVals->getNodeValue());
    histogram = new vector<int>(histogramTok.asIntVector());

    histogramPDF = new vector<double>(histogram->size());
    double histogramSum = 0;
    for (unsigned int i = 0; i < histogram->size(); i++) histogramSum += histogram->at(i);
    for (unsigned int i = 0; i < histogramPDF->size(); i++) histogramPDF->at(i) = (1.0 * histogram->at(i)) / histogramSum;
}

void RootsPersistenceAndStatistics::print(const char* histogramName, vector<int>*& histogram) {
    if (histogram) {
        cout << histogramName << ": " << endl;
        for (unsigned int i = 0; i < histogram->size(); i++) {
            cout << histogram->at(i) << "  ";
        }
    } else {
        cout << histogramName << ": null" << endl;
    }
    cout << endl << endl;
}

void RootsPersistenceAndStatistics::print(const char* histogramName, vector<double>*& histogram) {
    if (histogram) {
        cout << histogramName << ": " << endl;
        for (unsigned int i = 0; i < histogram->size(); i++) {
            cout << histogram->at(i) << "  ";
        }
    } else {
        cout << histogramName << ": null" << endl;
    }
    cout << endl << endl;
}

