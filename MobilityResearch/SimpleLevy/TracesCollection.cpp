#include <TracesCollection.h>


TracesCollection* TracesCollection::instance = NULL;   // указатель на singleton объект


TracesCollection* TracesCollection::getInstance()
{
    if (!instance) instance = new TracesCollection();
    return instance;
}


void TracesCollection::readTraces(char* TracesDir, char* filePatter)
{
    ASSERT(!traces);

    traces = new vector<vector<TracePoint>*>();
    char* traceFileNamePattern = buildFullName(TracesDir, filePatter);

    WIN32_FIND_DATA f;
    HANDLE h = FindFirstFile(traceFileNamePattern, &f);
    if(h != INVALID_HANDLE_VALUE) {
        do {
            char* traceName = new char[256];
            traceName = strcpy(traceName, f.cFileName);
            char* inputFileName = buildFullName(TracesDir, traceName);
            ifstream* infile = new ifstream(inputFileName);
            vector<TracePoint>* trace = new vector<TracePoint>();
            double lastRedT = -1;
            while (!infile->eof()) {
                double T, X, Y;
                (*infile) >> T >> X >> Y;
                if (lastRedT != -1) {
                    //это значит наткнулись на дубль последней строки (или в общем случае вообще на дубль строки)
                    bool nextIter = false;
                    if (lastRedT == T) nextIter = true;
                    lastRedT = -1;
                    if (nextIter) continue;
                }
                lastRedT = T;
                trace->push_back(TracePoint(T, X, Y, traceName));
            }
            infile->close();
            delete infile;
            delete[] inputFileName;
            traces->push_back(trace);
        }
        while(FindNextFile(h, &f));
    } else cout << "Directory or files not found\n";
}


void TracesCollection::print()
{
    for (unsigned int i=0; i<traces->size(); i++) {
        vector<TracePoint>* trace = traces->at(i);
        cout << "Trace index " << i << "/" << traces->size() << ", points: ";
        for (unsigned int j=0; j<trace->size(); j++) {
            TracePoint point = trace->at(j);
            cout << point.T << ", " << point.X << ", "<< point.Y << ";  ";
            if (j % 30 == 0) cout << endl;
        }
        cout << endl << "----------------------------------------------------" << endl << endl;
    }
}
