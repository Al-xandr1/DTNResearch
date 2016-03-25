#ifndef TRACEMERGER_H_INCLUDED
#define TRACEMERGER_H_INCLUDED

#include "Reader.h"

class TraceMerger
{
private:
    char* resultFileName;
    ofstream* resultFile;

public:
    TraceMerger(char* resultFileName)
    {
        this->resultFileName = resultFileName;
        this->resultFile = new ofstream(resultFileName);
        if (!this->resultFile) {
            cout << "\t" << "TraceMerger: allTrace file "<< resultFileName <<" is not created." << endl;
            exit(-324);
        }
    }

    ~TraceMerger()
    {
        if (this->resultFile)
        {
            cout << endl << "   ResultFileName: " << this->resultFileName << endl;
            this->resultFile->close();
            delete this->resultFile;
            this->resultFile = NULL;
        }
    }

    void addFile(char* traceFileName)
    {
        TracePointReader* reader = new TracePointReader(traceFileName);
        TracePoint *point = NULL;
        while (reader->hasNext()) {
            point = reader->next();

            point->write(resultFile);
            (*resultFile) << "\t";

            delete point;
        }
        delete reader;
        (*resultFile) << "\n";
    }
};

#endif // TRACEMERGER_H_INCLUDED
