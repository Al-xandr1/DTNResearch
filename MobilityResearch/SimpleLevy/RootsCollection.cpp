#include "RootsCollection.h"


RootsCollection* RootsCollection::instance = NULL;            // ��������� �� singleton ������


RootsCollection* RootsCollection::getInstance()
{
    if (!instance) instance = new RootsCollection();
    return instance;
}


void RootsCollection::readRootsData(char* TracesDir, char* allRootsFile, char* rootsDir, char* filePattern)
{
    // ������������� ��������� RootsData �� ��������� ������ ������ *.rot �� rootfiles
    cout << "Initializing of RootsData..." << endl;
    ASSERT(!RootsData);
    RootsData = new vector<vector<HotSpotDataRoot>*>();

    const char* rootFileNamePattern = buildFullName(rootsDir, filePattern);
    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(rootFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            double* persistence = extractDoubleParameter(f.cFileName, PERSISTENCE);
            //���� ����������� ��������������� ����, �� �� ������ ���� ������ ����
            if (persistence != NULL) ASSERT(*(persistence) > 0);
            //todo  ���� ���??

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

                if (lastRedHotSpotName) {
                    //��� ������ ���������� �� ����� ��������� ������ (��� � ����� ������ ������ �� ����� ������)
                    bool nextIter = false;
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
        }
        while(FindNextFile(h, &f));
    } else cout << "Directory or files not found\n";
    cout << "RootsData is initialized." << endl;


    // ������������� ��������� RootsDataShort �� ��������� ����� allroots.roo (���� �� ����) ��� �� ������ RootsData (���� ����� ���)
    cout << "Initializing of RootsDataShort..." << endl;
    //todo �������� ����������� allroots.roo � ��������� �������� ��������
    ASSERT(!RootsDataShort);
    RootsDataShort = new vector<RootDataShort>();
    ifstream rfile(buildFullName(TracesDir, allRootsFile));
    while(!rfile.eof()) {
        string rootinfo;
        getline(rfile, rootinfo);
        RootsDataShort->push_back(RootDataShort(rootinfo));
    }
    RootsDataShort->pop_back(); // ������� ����� � �����
    cout << "RootsDataShort is initialized." << endl;


    // ������������� ��������� generatedRootsData ��� �������� ������������ ��������� ��� ������� ���� �� ����
    cout << "Initializing of generatedRootsData..." << endl;
    ASSERT(!generatedRootsData);
    generatedRootsData = new vector<vector<vector<HotSpotDataRoot*> *> *>();
    for (unsigned int i=0; i<RootsData->size(); i++) {
        generatedRootsData->push_back(new vector<vector<HotSpotDataRoot*>*>());
    }
    cout << "generatedRootsData is initialized." << endl;


    // �������� ��������������� �������� RootsDataShort & RootsData & generatedRootsData
    cout << "Checking of consistency..." << endl;
    ASSERT(RootsDataShort->size() == RootsData->size());
    ASSERT(RootsDataShort->size() == generatedRootsData->size());
    for (unsigned int i=1; i<RootsDataShort->size(); i++) {
        ASSERT(RootsDataShort->at(i).length == RootsData->at(i)->size());
        for (unsigned  int j=0; j<RootsDataShort->at(i).length; j++) {
            ASSERT(strcmp(RootsDataShort->at(i).hotSpot[j], RootsData->at(i)->at(j).hotSpotName) == 0);
        }
    }
    cout << "Consistency is checked." << endl;
}


void RootsCollection::collectRoot(vector<HotSpotData*>* root, vector<unsigned int>* rootSnumber, vector<int>* rootCounter, unsigned int nodeId, unsigned int day)
{
    ASSERT(generatedRootsData);
    ASSERT(root->size() == rootSnumber->size() && root->size() == rootCounter->size());
    ASSERT(0 <= nodeId && nodeId < generatedRootsData->size());
    // ������ ���� � ������� ������ � 1
    ASSERT(1 <= day);

    vector<HotSpotDataRoot*>* rootForHistory = new vector<HotSpotDataRoot*>();
    for (unsigned int i=0; i<root->size(); i++) {
        HotSpotDataRoot* data = new HotSpotDataRoot(*(root->at(i)));
        // ����������� ���������� ���������
        data->counter = rootCounter->at(i);
        rootForHistory->push_back(data);
    }

    //�������� �������� ���������� ���� �� ���� ��������� ����
    vector<vector<HotSpotDataRoot*>*>* rootsByDays = generatedRootsData->at(nodeId);
    if ((day-1) == rootsByDays->size()) {
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
