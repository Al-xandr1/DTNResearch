#include <RootsPersistenceAndStatistics.h>

Define_Module(RootsPersistenceAndStatistics);

void RootsPersistenceAndStatistics::initialize() {
    ASSERT(persistenceFromMassCenter == -1);
    ASSERT(!summarizedIndicatorVector);
    ASSERT(!rootsDimensionHistogram);
    ASSERT(!summarizedRoot);

    cXMLElement *rootStatistics = par("rootsPersistenceAndStatistics");

    cXMLElement* persistence = rootStatistics->getChildrenByTagName("PERSISTENCE").at(0);
    cXMLElement* massCenter = persistence->getChildrenByTagName("MASS-CENTER").at(0);
    cXMLElement* coef = massCenter->getChildrenByTagName("COEF").at(0);
    persistenceFromMassCenter = atof(coef->getNodeValue());


    cXMLElement* summarizedIndicatorVector = rootStatistics->getChildrenByTagName("SUMMARIZED-INDICATOR-VECTOR").at(0);
    cXMLElement* summarizedIndicatorVectorVals = summarizedIndicatorVector->getChildrenByTagName("VALS").at(0);
    cStringTokenizer summarizedIndicatorVectorTok(summarizedIndicatorVectorVals->getNodeValue());
    vector<int> summarizedIndicatorVectorVec = summarizedIndicatorVectorTok.asIntVector();
    this->summarizedIndicatorVector = new vector<int>(summarizedIndicatorVectorVec);


    cXMLElement* rootsDimensionHistogram = rootStatistics->getChildrenByTagName("ROOTS-DIMENSION-HISTOGRAM").at(0);
    cXMLElement* rootsDimensionHistogramVals = rootsDimensionHistogram->getChildrenByTagName("VALS").at(0);
    cStringTokenizer rootsDimensionHistogramTok(rootsDimensionHistogramVals->getNodeValue());
    vector<int> rootsDimensionHistogramVec = rootsDimensionHistogramTok.asIntVector();
    this->rootsDimensionHistogram = new vector<int>(rootsDimensionHistogramVec);


    cXMLElement* summarizedRoot = rootStatistics->getChildrenByTagName("SUMMARIZED-ROOT").at(0);
    cXMLElement* summarizedRootVals = summarizedRoot->getChildrenByTagName("VALS").at(0);
    cStringTokenizer summarizedRootTok(summarizedRootVals->getNodeValue());
    vector<int> summarizedRootVec = summarizedRootTok.asIntVector();
    this->summarizedRoot = new vector<int>(summarizedRootVec);


    print();
}

void RootsPersistenceAndStatistics::print() {
    cout << "persistenceFromMassCenter: " << persistenceFromMassCenter << endl << endl;
    if (summarizedIndicatorVector) {
        cout << "summarizedIndicatorVector: " << endl;
        for (unsigned int i = 0; i < summarizedIndicatorVector->size(); i++) {
            cout << summarizedIndicatorVector->at(i) << "  ";
        }
    } else {
        cout << "summarizedIndicatorVector: null" << endl;
    }
    cout << endl << endl;
    if (rootsDimensionHistogram) {
        cout << "rootsDimensionHistogram: " << endl;
        for (unsigned int i = 0; i < rootsDimensionHistogram->size(); i++) {
            cout << rootsDimensionHistogram->at(i) << "  ";
        }
    } else {
        cout << "rootsDimensionHistogram: null" << endl;
    }
    cout << endl << endl;
    if (summarizedRoot) {
        cout << "summarizedRoot: " << endl;
        for (unsigned int i = 0; i < summarizedRoot->size(); i++) {
            cout << summarizedRoot->at(i) << "  ";
        }
    } else {
        cout << "summarizedRoot: null" << endl;
    }
    cout << endl << endl;
}
