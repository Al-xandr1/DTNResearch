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
                //ASSERT(sumTime >= 0 && waypointNum >= 0);

                //�������� ��������� ������: ��� ������ ���������� ������� ���� �� �����
                if (lastRedHotSpotName) {
                    bool nextIter = false;
                    // ���� true - ������ ���������� �� ����� ��������� ������ (��� � ����� ������ ������ �� ����� ������)
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


    // ������������� ��������� RootsDataShort �� ��������� ����� allroots.roo (���� �� ����) ��� �� ������ RootsData (���� ����� ���)
    cout << "Initializing of RootsDataShort..." << endl;
    ASSERT(!RootsDataShort);
    RootsDataShort = new vector<RootDataShort>();
    ifstream rfile(buildFullName(TracesDir, allRootsFile));
    if (rfile.good()) {
        // ���� ���� allroots.roo ���������� - ��������� �� ����
        while(!rfile.eof()) {
            string rootinfo;
            getline(rfile, rootinfo);
            RootsDataShort->push_back(RootDataShort(rootinfo));
        }
        RootsDataShort->pop_back(); // ������� ����� � �����
    } else {
        // ���������� RootsDataShort �� ������ RootsData
        ASSERT(rootFileNames->size() == RootsData->size());
        for (unsigned int i = 0; i < RootsData->size(); i++) {
            string rootinfo = rootFileNames->at(i);             // ���������� ��� �����
            rootinfo += string("\t");
            vector<HotSpotDataRoot> *root = RootsData->at(i);
            rootinfo += std::to_string(root->size());           // ���������� ����� ��������
            rootinfo += string("\t");
            for (unsigned int j = 0; j < root->size(); j++) {   // ���������� ������������������ �������
                rootinfo += string(root->at(j).hotSpotName);
                rootinfo += string("\t");
            }
            RootsDataShort->push_back(RootDataShort(rootinfo));
        }
    }
    delete rootFileNames;
    cout << "RootsDataShort is initialized." << endl << endl;


    // ������������� ��������� generatedRootsData ��� �������� ������������ ��������� ��� ������� ���� �� ����
    cout << "Initializing of generatedTheoryRootsData & generatedActualRootsData..." << endl;
    ASSERT(!generatedTheoryRootsData && !generatedActualRootsData);
    generatedTheoryRootsData = new vector<vector<vector<HotSpotDataRoot*> *> *>();
    generatedActualRootsData = new vector<vector<vector<HotSpotDataRoot*> *> *>();
    for (unsigned int i=0; i<RootsData->size(); i++) {
        generatedTheoryRootsData->push_back(new vector<vector<HotSpotDataRoot*>*>());
        generatedActualRootsData->push_back(new vector<vector<HotSpotDataRoot*>*>());
    }
    cout << "generatedTheoryRootsData & generatedActualRootsData are initialized." << endl;


    // �������� ��������������� �������� RootsDataShort & RootsData & generatedTheoryRootsData & generatedActualRootsData
    cout << "Checking of consistency..." << endl;
    ASSERT(RootsDataShort->size() == RootsData->size()); // ���� ������ - ������ � allroots.roo & rootfiles/ �� �����������
    ASSERT(RootsDataShort->size() == generatedTheoryRootsData->size() && RootsDataShort->size() == generatedActualRootsData->size());
    for (unsigned int i=1; i<RootsDataShort->size(); i++) {
        ASSERT(RootsDataShort->at(i).length == RootsData->at(i)->size());
        for (unsigned  int j=0; j<RootsDataShort->at(i).length; j++) {
            ASSERT(strcmp(RootsDataShort->at(i).hotSpot[j], RootsData->at(i)->at(j).hotSpotName) == 0);
        }
    }
    cout << "Consistency is checked." << endl << endl;
}

void RootsCollection::collectTheoryRoot(vector<HotSpotData*>* root,
                                        vector<unsigned int>* rootSnumber,
                                        vector<int>* rootCounter,
                                        unsigned int nodeId,
                                        unsigned int day) {
    collectRoot(generatedTheoryRootsData, root, rootSnumber, rootCounter, NULL, NULL, NULL, nodeId, day);
}

void RootsCollection::collectActualRoot(vector<HotSpotData*>* root,
                                        vector<unsigned int>* rootSnumber,
                                        vector<int>* rootCounter,
                                        vector<unsigned int>* rootTrack,
                                        vector<double>* rootTrackSumTime,
                                        vector<int>* rootTrackWaypointNum,
                                        unsigned int nodeId,
                                        unsigned int day) {
    collectRoot(generatedActualRootsData, root, rootSnumber, rootCounter, rootTrack, rootTrackSumTime, rootTrackWaypointNum, nodeId, day);
}

void RootsCollection::collectRoot(vector<vector<vector<HotSpotDataRoot*> *> *> *generatedRootsData,
                                  vector<HotSpotData*>* root,
                                  vector<unsigned int>* rootSnumber,
                                  vector<int>* rootCounter,
                                  vector<unsigned int>* rootTrack,
                                  vector<double>* rootTrackSumTime,
                                  vector<int>* rootTrackWaypointNum,
                                  unsigned int nodeId,
                                  unsigned int day)
{
    ASSERT(generatedRootsData);
    ASSERT(root->size() == rootSnumber->size() && root->size() == rootCounter->size());
    ASSERT(0 <= nodeId && nodeId < generatedRootsData->size());
    // ������ ���� � ������� ������ � 1
    ASSERT(1 <= day);

    vector<HotSpotDataRoot*>* rootForHistory = new vector<HotSpotDataRoot*>();

    if (rootTrack) {
        // ���� ���� ����, �� ���������� ������� �� ��������� ���!
        for (unsigned int i = 0; i < rootTrack->size(); i++) {
            ASSERT(rootTrack->at(i) >= 0 && rootTrack->at(i) < root->size());

            HotSpotDataRoot* data = new HotSpotDataRoot();
            data->hotSpotName = new char[256];
            data->hotSpotName = strcpy(data->hotSpotName, root->at(rootTrack->at(i))->hotSpotName);
            data->Xmin = root->at(rootTrack->at(i))->Xmin;
            data->Xmax = root->at(rootTrack->at(i))->Xmax;
            data->Ymin = root->at(rootTrack->at(i))->Ymin;
            data->Ymax = root->at(rootTrack->at(i))->Ymax;
            //������ ����������� �������� ������� ���������� � ������� � ���-�� ������� ����� � ������� (��� ���� � ������ ������ ��������)
            ASSERT(rootTrackSumTime->at(i) > 0);
            data->sumTime = rootTrackSumTime->at(i);
            ASSERT(rootTrackWaypointNum->at(i) > 0);
            data->waypointNum = rootTrackWaypointNum->at(i);
            data->counter = -1; // ����� ��� ���� �������������� �� ������!
            rootForHistory->push_back(data);
        }
    } else {
        // ���� ����� ���, �� ��������� ������� �� ��������� rootCounter->at(i) (������� ����� ���������������)
        for (unsigned int i = 0; i < root->size(); i++) {
            for (int count = 0; count < rootCounter->at(i); count++) {
                HotSpotDataRoot* data = new HotSpotDataRoot();
                data->hotSpotName = new char[256];
                data->hotSpotName = strcpy(data->hotSpotName, root->at(i)->hotSpotName);
                data->Xmin = root->at(i)->Xmin;
                data->Xmax = root->at(i)->Xmax;
                data->Ymin = root->at(i)->Ymin;
                data->Ymax = root->at(i)->Ymax;
                //����� ��� ������ � ������, ����� ������������ ������������� �������. � ���� ������ �� ��������� ���� sumTime & waypointNum!
                data->sumTime = -1;
                data->waypointNum = -1;
                data->counter = -1; // ����� ��� ���� �������������� �� ������!
                rootForHistory->push_back(data);
            }
        }
    }

    //�������� �������� ���������� ���� �� ���� ��������� ����
    vector<vector<HotSpotDataRoot*>*>* rootsByDays = generatedRootsData->at(nodeId);
    if ((day-1) == rootsByDays->size()) {
        //���������, ��� �������� ��� ���������� ���
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


void RootsCollection::saveRoots(const char *thRtDir, const char *acRtDir)
{
    ASSERT(RootsDataShort->size() == generatedTheoryRootsData->size() && RootsDataShort->size() == generatedActualRootsData->size());

    innerSaveRoots("Theory", thRtDir, generatedTheoryRootsData);
    innerSaveRoots("Actual", acRtDir, generatedActualRootsData);
}


void RootsCollection::innerSaveRoots(const char *logPrefix, const char *rtDir, vector<vector<vector<HotSpotDataRoot*> *> *> *generatedRootsData)
{
    // ��������� ������� ������ ���� ������ ������������� �� ��� ���
    for (unsigned int i = 0; i < generatedRootsData->size(); i++) {
        // ����� �� ������ ���� ����� � ��������� �� ��������

        vector<vector<HotSpotDataRoot*>*>* dailyRootsPerNode = generatedRootsData->at(i);
        for (unsigned int j = 0; j < dailyRootsPerNode->size(); j++) {

            // ��������� ��� ����� ��� �������� �������� j ...
            string filename("Gen_");
            string simpleName = extractSimpleName(RootsDataShort->at(i).RootName);
            std::size_t found;
            if ((found = simpleName.find("_id=")) != std::string::npos) {
                // �.�. � �������� ����� �� ����� ���� �������� ����� ��� (������ id - ��� The_dartmouth_cenceme_dataset_(v.2008-08-13))
                filename += (simpleName.substr(0, (found + 8)) + string(buildIntParameter("day", j+1, 3)) + simpleName.substr((found + 8), simpleName.size()));
            } else if ((found = simpleName.find("_30sec_")) != std::string::npos) {
                // �.�. � �������� ����� �� ����� ���� �������� ����� ��� (������ ����� ������� _30sec_ - ��� ����� KAIST, NCSU, NewYork, Orlando, Statefair)
                filename += (simpleName.substr(0, (found + 10)) + string(buildIntParameter("_day", j+1, 3)) + simpleName.substr((found + 10), simpleName.size()));
            } else {
                filename += (string(buildIntParameter("day", j+1, 3)) + extractSimpleName(RootsDataShort->at(i).RootName));
            }

            // ����� �� ��������� j �� ��� ��� �������� ���� i
            ofstream* rtFile = new ofstream(buildFullName(rtDir, filename.c_str()));
            vector<HotSpotDataRoot*>* dailyRoot = dailyRootsPerNode->at(j);
            for (unsigned int k = 0; k < dailyRoot->size(); k++) {
                HotSpotDataRoot* hs = dailyRoot->at(k);
                (*rtFile) << hs->hotSpotName << "\t" << hs->Xmin << "\t" << hs->Xmax << "\t" << hs->Ymin << "\t" << hs->Ymax
                          << "\t" << hs->sumTime << "\t" << hs->waypointNum << endl;
            }
            rtFile->close();
        }
        cout << "\t " << logPrefix << " roots per node " << i << " are collected!";
    }

    // ��������� �������� �� ���� (� ������)
    // �������� ����� ���� ����
    unsigned int days = generatedRootsData->at(0)->size();
    for (unsigned int day = 0; day < days; day++) {
        // ����� �� ���� ����

        // ������ ����� � ���������� �������� ���
        const char *dirForDayRoots = buildFullName(rtDir, buildIntParameter("_day", day+1, 3));
        if (CreateDirectory(dirForDayRoots, NULL)) cout << "create output directory: " << dirForDayRoots << endl;
        else cout << "error create output directory: " << dirForDayRoots << endl;

        for (unsigned int node = 0; node < generatedRootsData->size(); node++) {
            // ����� �� ���� ������������� (��� ������� ��� ���� �� ���� ������������� ��������)
            vector<HotSpotDataRoot*>* dailyRoot = generatedRootsData->at(node)->at(day);

            // ��������� ��� ����� ��� �������� �������� node ...
            string filename("Gen_");
            string simpleName = extractSimpleName(RootsDataShort->at(node).RootName);
            std::size_t found;
            if ((found = simpleName.find("_id=")) != std::string::npos) {
                // �.�. � �������� ����� �� ����� ���� �������� ����� ��� (������ id - ��� The_dartmouth_cenceme_dataset_(v.2008-08-13))
                filename += (simpleName.substr(0, (found + 8)) + string(buildIntParameter("day", day+1, 3)) + simpleName.substr((found + 8), simpleName.size()));
            } else if ((found = simpleName.find("_30sec_")) != std::string::npos) {
                // �.�. � �������� ����� �� ����� ���� �������� ����� ��� (������ ����� ������� _30sec_ - ��� ����� KAIST, NCSU, NewYork, Orlando, Statefair)
                filename += (simpleName.substr(0, (found + 10)) + string(buildIntParameter("_day", day+1, 3)) + simpleName.substr((found + 10), simpleName.size()));
            } else {
                filename += (string(buildIntParameter("day", day+1, 3)) + extractSimpleName(RootsDataShort->at(node).RootName));
            }

            // ����� �� ��������� �� ��� ��� �������� ����
            ofstream* rtFile = new ofstream(buildFullName(dirForDayRoots, filename.c_str()));
            for (unsigned int k = 0; k < dailyRoot->size(); k++) {
                HotSpotDataRoot* hs = dailyRoot->at(k);
                (*rtFile) << hs->hotSpotName << "\t" << hs->Xmin << "\t" << hs->Xmax << "\t" << hs->Ymin << "\t" << hs->Ymax
                          << "\t" << hs->sumTime << "\t" << hs->waypointNum << endl;
            }
            rtFile->close();
        }

        cout << "\t " << logPrefix << " roots per day " << day << " are collected!";
    }
}
