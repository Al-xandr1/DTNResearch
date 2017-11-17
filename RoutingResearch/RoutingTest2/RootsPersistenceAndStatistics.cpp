#include <RootsPersistenceAndStatistics.h>


//---------------------------------- RootsPersistenceAndStatistics -----------------------------------------------


void RootsPersistenceAndStatistics::initialize() {
    ASSERT(persistenceFromMassCenter == -1);
    ASSERT(!rootsDimensionHistogram);
    ASSERT(!rootsDimensionHistogramPDF);
    ASSERT(!summarizedIndicatorVector);
    ASSERT(!summarizedIndicatorVectorPDF);
    ASSERT(!averageCounterVector);
    ASSERT(!hotSpots);

    // Чтение данных из файла
    xml_document doc;
    xml_parse_result result = doc.load_file(xmlFileName->c_str());
    cout << endl << "Result of loading rootStatistics (" << xmlFileName->c_str() << "): " << result.description() << endl;

    xml_node rootStatistics = doc.child("ROOT-STATISTICS");

    const char* coef = rootStatistics.child("PERSISTENCE").child("MASS-CENTER").child("COEF").child_value();
    persistenceFromMassCenter = atof(coef);

    const char* rootsDimensionHistogramStr = rootStatistics.child("ROOTS-DIMENSION-HISTOGRAM").child("VALS").child_value();
    parseData(rootsDimensionHistogramStr, rootsDimensionHistogram, rootsDimensionHistogramPDF);

    const char* summarizedIndicatorVectorStr = rootStatistics.child("SUMMARIZED-INDICATOR-VECTOR").child("VALS").child_value();
    parseData(summarizedIndicatorVectorStr, summarizedIndicatorVector, summarizedIndicatorVectorPDF);

    const char* averageCounterVectorStr = rootStatistics.child("AVERAGE-COUNTER-VECTOR").child("VALS").child_value();
    parseData(averageCounterVectorStr, averageCounterVector);

    const char* hotSpotsStr = rootStatistics.child("HOT-SPOTS").child("VALS").child_value();
    parseData(hotSpotsStr, hotSpots);

    // проверяем загрузку данных и согласованность размеров всех структур
    ASSERT(persistenceFromMassCenter != -1);
    ASSERT(rootsDimensionHistogram);
    ASSERT(rootsDimensionHistogramPDF);
    ASSERT(summarizedIndicatorVector);
    ASSERT(summarizedIndicatorVectorPDF);
    ASSERT(averageCounterVector);
    ASSERT(hotSpots);
    // в rootsDimensionHistogramPDF на одно значение больше (из-за размерности 0)
    ASSERT((hotSpots->size() + 1) == rootsDimensionHistogramPDF->size());
    ASSERT(hotSpots->size() == summarizedIndicatorVectorPDF->size());
    ASSERT(hotSpots->size() == averageCounterVector->size());
}

void RootsPersistenceAndStatistics::parseData(const char* histogramStr, vector<int>*& histogram, vector<double>*& histogramPDF) {
    cStringTokenizer histogramTok(histogramStr);
    histogram = new vector<int>(histogramTok.asIntVector());

    double histogramSum = 0;
    for (unsigned int i = 0; i < histogram->size(); i++) histogramSum += histogram->at(i);
    histogramPDF = new vector<double>(histogram->size());
    for (unsigned int i = 0; i < histogramPDF->size(); i++) histogramPDF->at(i) = (1.0 * histogram->at(i)) / histogramSum;
}

void RootsPersistenceAndStatistics::parseData(const char* vecStr, vector<double>*& vec) {
    cStringTokenizer vecTok(vecStr);
    vec = new vector<double>(vecTok.asDoubleVector());
}

void RootsPersistenceAndStatistics::parseData(const char* hotSpotsStr, vector<const char*>*& hotSpots) {
    hotSpots = new vector<const char*>();

    string tmp(hotSpotsStr);
    tmp = fullTrim(tmp);
    cStringTokenizer vecTok(tmp.c_str());
    vecTok.setDelimiter("  ");
    vector<string> hotSpotNamesVec = vecTok.asVector();
    for (unsigned int i = 0; i < hotSpotNamesVec.size(); i++) {
        string hotSpotNameStr = hotSpotNamesVec.at(i);
        char* newStr = new char[256];
        newStr = strcpy(newStr, hotSpotNameStr.c_str());
        hotSpots->push_back(newStr);
    }
}

unsigned int RootsPersistenceAndStatistics::findHotSpotIndexByName(const char* hotSpotName) {
    int index = -1;
    for(unsigned int i = 0; i < hotSpots->size(); i++)
        if (strcmp(hotSpots->at(i), hotSpotName) == 0) {
            index = i;
            break;
        }
    ASSERT(index != -1); //обязательно должны найти! Иначе это неконсистентные данные
    return index;
}

void RootsPersistenceAndStatistics::print() {
    cout << endl << "RootsPersistenceAndStatistics::print: START" << endl;
    cout << "xmlFileName: " << xmlFileName->c_str() << endl;

    cout << "persistenceFromMassCenter: " << persistenceFromMassCenter << endl;

    print("rootsDimensionHistogram", rootsDimensionHistogram);
    print("rootsDimensionHistogramPDF", rootsDimensionHistogramPDF);
    cout << "rootsDimensionHistogramPDFSum=" << getSum(*rootsDimensionHistogramPDF) << endl ;

    print("summarizedIndicatorVector", summarizedIndicatorVector);
    print("summarizedIndicatorVectorPDF", summarizedIndicatorVectorPDF);
    cout << "summarizedIndicatorVectorPDFSum=" << getSum(*summarizedIndicatorVectorPDF) << endl;

    print("averageCounterVector", averageCounterVector);
    print("hotSpots", hotSpots);

    if (hotSpots) cout << "hotSpots->size() = " << hotSpots->size() << endl;
    if (rootsDimensionHistogramPDF) cout << "rootsDimensionHistogramPDF->size() = " << rootsDimensionHistogramPDF->size() << endl;
    if (summarizedIndicatorVectorPDF) cout << "summarizedIndicatorVectorPDF->size() = " << summarizedIndicatorVectorPDF->size() << endl;
    if (averageCounterVector) cout << "averageCounterVector->size() = " << averageCounterVector->size() << endl;
    cout << "RootsPersistenceAndStatistics::print: END" << endl << endl;
}


//----------------------------- RootsPersistenceAndStatisticsCollection --------------------------------------


RootsPersistenceAndStatisticsCollection* RootsPersistenceAndStatisticsCollection::instance = NULL;   // указатель на singleton объект


RootsPersistenceAndStatisticsCollection* RootsPersistenceAndStatisticsCollection::getInstance() {
    if (!instance) instance = new RootsPersistenceAndStatisticsCollection();
    return instance;
}

void RootsPersistenceAndStatisticsCollection::readRootStatistics(const char* commonRootStatisticsFileName, const char* personalRootStatisticsDir) {
    cout << endl << "Initializing of RootsPersistenceAndStatisticsCollection..." << endl;

    ASSERT(!commonRootStatistics);
    ASSERT(!personalRootStatisticsList);

    // Чтение общей статистики
    const char* commonRootStatisticsFileFullName = buildFullName(DEF_TR_DIR, commonRootStatisticsFileName);
    ifstream* commonRootStatisticsFile = new ifstream(commonRootStatisticsFileFullName);
    if (!commonRootStatisticsFile) { cout << " No commonRootStatisticsFile file: " << commonRootStatisticsFileName << endl;  exit(-140); }
    commonRootStatisticsFile->close();
    delete commonRootStatisticsFile;
    commonRootStatistics = new RootsPersistenceAndStatistics(commonRootStatisticsFileFullName);

    // Реализовать Чтение персональных статистик
    personalRootStatisticsList = new vector<RootsPersistenceAndStatistics*>();
    const char* personalRootStatisticsNamePattern = buildFullName(personalRootStatisticsDir, PST_PATTERT);

    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(personalRootStatisticsNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            const char* fileName = buildFullName(personalRootStatisticsDir, f.cFileName);
            RootsPersistenceAndStatistics* rootsPersistenceAndStatistics = new RootsPersistenceAndStatistics(fileName);
            personalRootStatisticsList->push_back(rootsPersistenceAndStatistics);
        }
        while(FindNextFile(h, &f));
    } else cout << "Directory or files not found\n";

    ASSERT(commonRootStatistics);
    ASSERT(personalRootStatisticsList);

    cout << endl <<  "RootsPersistenceAndStatisticsCollection is initialized." << endl << endl;
}

void RootsPersistenceAndStatisticsCollection::print() {
    cout << endl << "------------------------ CommonRootStatistics START -------------------------" << endl;
    commonRootStatistics->print();
    cout << "------------------------ CommonRootStatistics END ---------------------------" << endl << endl;

    cout << "\t---------------------- PersonalRootStatisticsList START -------------------------" << endl;
    for (unsigned int i = 0; i < personalRootStatisticsList->size(); i++) {
        cout << "\t---------------------- PersonalRootStatistics START (" << i << ")-------------------------" << endl;
        personalRootStatisticsList->at(i)->print();
        cout << "\t---------------------- PersonalRootStatistics END ---------------------------" << endl << endl;
    }
    cout << "\t---------------------- PersonalRootStatisticsList END----------------------------" << endl << endl;
}
