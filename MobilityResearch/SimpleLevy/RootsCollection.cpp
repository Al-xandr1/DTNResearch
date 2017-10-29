#include "RootsCollection.h"


RootsCollection* RootsCollection::instance = NULL;            // указатель на singleton объект


RootsCollection* RootsCollection::getInstance()
{
    if (!instance) instance = new RootsCollection();
    return instance;
}


void RootsCollection::readRootsData(char* TracesDir, char* allRootsFile, char* rootsDir, char* filePattern)
{
    // Инициализация структуры RootsData на основании набора файлов *.rot из rootfiles
    cout << "Initializing of RootsData..." << endl;
    ASSERT(!RootsData);
    RootsData = new vector<vector<HotSpotDataRoot>*>();
    vector<string>* rootFileNames = new vector<string>();

    const char* rootFileNamePattern = buildFullName(rootsDir, filePattern);
    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(rootFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            const char* inputFileName = buildFullName(rootsDir, f.cFileName);
            ifstream* infile = new ifstream(inputFileName);
            vector<HotSpotDataRoot>* root = new vector<HotSpotDataRoot>;
            char* lastRedHotSpotName = NULL;
            while (!infile->eof()) {
                char hotSpotName[256];
                double Xmin, Xmax, Ymin, Ymax;
                double sumTime;
                unsigned int waypointNum;
                (*infile) >> hotSpotName >> Xmin >> Xmax >> Ymin >> Ymax >> sumTime >> waypointNum;
                ASSERT(sumTime >= 0 && waypointNum >= 0);

                //контроль появления дублей: две ПОДРЯД одинаковых локации идти не могут
                if (lastRedHotSpotName) {
                    bool nextIter = false;
                    // если true - значит наткнулись на дубль последней строки (или в общем случае вообще на дубль строки)
                    if (strcmp(lastRedHotSpotName, hotSpotName) == 0) nextIter = true;
                    delete[] lastRedHotSpotName;
                    lastRedHotSpotName = NULL;
                    if (nextIter) continue;
                }
                lastRedHotSpotName = new char[256];
                lastRedHotSpotName = strcpy(lastRedHotSpotName, hotSpotName);

                root->push_back(HotSpotDataRoot(hotSpotName, Xmin, Xmax, Ymin, Ymax, sumTime, waypointNum));
            }
            infile->close();
            delete infile;
            delete[] inputFileName;
            RootsData->push_back(root);
            rootFileNames->push_back(string(inputFileName));
        }
        while(FindNextFile(h, &f));
    } else cout << "Directory or files not found\n";
    cout << "RootsData is initialized." << endl << endl;


    // Инициализация структуры RootsDataShort на основании файла allroots.roo (если он есть) ИЛИ на основе RootsData (если файла нет)
    cout << "Initializing of RootsDataShort..." << endl;
    ASSERT(!RootsDataShort);
    RootsDataShort = new vector<RootDataShort>();
    ifstream rfile(buildFullName(TracesDir, allRootsFile));
    if (rfile.good()) {
        // Если файл allroots.roo существует - считываем из него
        while(!rfile.eof()) {
            string rootinfo;
            getline(rfile, rootinfo);
            RootsDataShort->push_back(RootDataShort(rootinfo));
        }
        RootsDataShort->pop_back(); // удаляем дубль в конце
    } else {
        // Генерируем RootsDataShort на основе RootsData
        ASSERT(rootFileNames->size() == RootsData->size());
        for (unsigned int i = 0; i < RootsData->size(); i++) {
            string rootinfo = rootFileNames->at(i);             // записываем имя файла
            rootinfo += string("\t");
            vector<HotSpotDataRoot> *root = RootsData->at(i);
            rootinfo += std::to_string(root->size());           // записываем длину маршрута
            rootinfo += string("\t");
            for (unsigned int j = 0; j < root->size(); j++) {   // записываем последовательность локаций
                rootinfo += string(root->at(j).hotSpotName);
                rootinfo += string("\t");
            }
            RootsDataShort->push_back(RootDataShort(rootinfo));
        }
    }
    delete rootFileNames;
    cout << "RootsDataShort is initialized." << endl << endl;


    // Инициализация структуры generatedRootsData для хранения генерируемых маршрутов для каждого узла по дням
    cout << "Initializing of generatedTheoryRootsData & generatedActualRootsData..." << endl;
    ASSERT(!generatedTheoryRootsData && !generatedActualRootsData);
    generatedTheoryRootsData = new vector<vector<vector<HotSpotDataRoot*> *> *>();
    generatedActualRootsData = new vector<vector<vector<HotSpotDataRoot*> *> *>();
    for (unsigned int i=0; i<RootsData->size(); i++) {
        generatedTheoryRootsData->push_back(new vector<vector<HotSpotDataRoot*>*>());
        generatedActualRootsData->push_back(new vector<vector<HotSpotDataRoot*>*>());
    }
    cout << "generatedTheoryRootsData & generatedActualRootsData are initialized." << endl;


    // Проверка согласованности структур RootsDataShort & RootsData & generatedTheoryRootsData & generatedActualRootsData
    cout << "Checking of consistency..." << endl;
    ASSERT(RootsDataShort->size() == RootsData->size()); // если падает - данные в allroots.roo & rootfiles/ не согласованы
    ASSERT(RootsDataShort->size() == generatedTheoryRootsData->size() && RootsDataShort->size() == generatedActualRootsData->size());
    for (unsigned int i=1; i<RootsDataShort->size(); i++) {
        ASSERT(RootsDataShort->at(i).length == RootsData->at(i)->size());
        for (unsigned  int j=0; j<RootsDataShort->at(i).length; j++) {
            ASSERT(strcmp(RootsDataShort->at(i).hotSpot[j], RootsData->at(i)->at(j).hotSpotName) == 0);
        }
    }
    cout << "Consistency is checked." << endl << endl;
}

void RootsCollection::collectTheoryRoot(vector<HotSpotData*>* root, vector<unsigned int>* rootSnumber, vector<int>* rootCounter, unsigned int nodeId, unsigned int day) {
    collectRoot(generatedTheoryRootsData, root, rootSnumber, rootCounter, nodeId, day);
}

void RootsCollection::collectActualRoot(vector<HotSpotData*>* root, vector<unsigned int>* rootSnumber, vector<int>* rootCounter, unsigned int nodeId, unsigned int day) {
    collectRoot(generatedActualRootsData, root, rootSnumber, rootCounter, nodeId, day);
}

void RootsCollection::collectRoot(vector<vector<vector<HotSpotDataRoot*> *> *> *generatedRootsData,
                                  vector<HotSpotData*>* root,
                                  vector<unsigned int>* rootSnumber,
                                  vector<int>* rootCounter,
                                  unsigned int nodeId,
                                  unsigned int day)
{
    ASSERT(generatedRootsData);
    ASSERT(root->size() == rootSnumber->size() && root->size() == rootCounter->size());
    ASSERT(0 <= nodeId && nodeId < generatedRootsData->size());
    // отсчёт дней в системе ведётся с 1
    ASSERT(1 <= day);

    vector<HotSpotDataRoot*>* rootForHistory = new vector<HotSpotDataRoot*>();
    for (unsigned int i=0; i<root->size(); i++) {
        HotSpotDataRoot* data = new HotSpotDataRoot();
        data->hotSpotName = new char[256];
        data->hotSpotName = strcpy(data->hotSpotName, root->at(i)->hotSpotName);
        data->Xmin = root->at(i)->Xmin;
        data->Xmax = root->at(i)->Xmax;
        data->Ymin = root->at(i)->Ymin;
        data->Ymax = root->at(i)->Ymax;
        //todo сделать запись фактических значений времени пребывания в локации и кот-ва путевых точек в локации (для узла в рамках одного маршрута)
        data->sumTime = -1;
        data->waypointNum = -1;
        // проставляем актуальную кратность
        data->counter = rootCounter->at(i);
        rootForHistory->push_back(data);
    }

    //получаем маршруты указанного узла по всем прошедшим дням
    vector<vector<HotSpotDataRoot*>*>* rootsByDays = generatedRootsData->at(nodeId);
    if ((day-1) == rootsByDays->size()) {
        //проверяем, что записали все предыдущие дни
        rootsByDays->push_back(rootForHistory);
    } else {
        cout << "Wrong day: nodeId = " << nodeId << ", day = " << day << ", rootsByDays->size() = " << rootsByDays->size() << endl;
        ASSERT(FALSE);
    }
}


void RootsCollection::printRootsDataShort()
{
    cout << "RootsCollection::printRootsDataShort RootsDataShort:" <<endl;
    for (unsigned int i=0; i<RootsDataShort->size(); i++) (*RootsDataShort)[i].print();
}


void RootsCollection::printRootsData()
{
    cout << "RootsCollection::printRootsDataShort RootsData:" <<endl;
    for(unsigned int i=0; i<RootsData->size(); i++) {
        cout << "Root " << i <<":" <<endl;
        for(unsigned int j=0; j<(*RootsData)[i]->size(); j++) (*RootsData)[i]->at(j).print();
    }
}
