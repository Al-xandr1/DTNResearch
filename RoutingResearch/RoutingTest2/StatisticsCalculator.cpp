#include "StatisticsCalculator.h"

Define_Module(StatisticsCalculator);

void StatisticsCalculator::initialize()
{
    //todo сделать напрямую из файлов отсюда
    //ifstream* routeHistoryOneFile = new ifstream(buildFullName(OUT_DIR, routeHistoryOne));
    //ifstream* routeHistoryTwoFile = new ifstream(buildFullName(OUT_DIR, routeHistoryTwo));

    ASSERT(!routeHistoryOne && !routeHistoryTwo);
    routeHistoryOne = par("routeHistoryOne");
    routeHistoryTwo = par("routeHistoryTwo");

    ASSERT(!durationMatrixOne && !durationMatrixTwo);
    durationMatrixOne = loadMatrix(routeHistoryOne);
    print(durationMatrixOne, par("routeHistoryOneFileName").stringValue());
    durationMatrixTwo = loadMatrix(routeHistoryTwo);
    print(durationMatrixTwo, par("routeHistoryTwoFileName").stringValue());

    double deviation = norm(durationMatrixOne, durationMatrixTwo);
    cout << "Standart norm deviation = " << deviation << endl;
}

//todo обобщить в методом из processRouteHistory()
vector<vector<double>*>* StatisticsCalculator::loadMatrix(cXMLElement *routeHistory)
{
    vector<vector<double>*>* matrix = new vector<vector<double>*>();

    int i = 0;
    cXMLElementList nodes = routeHistory->getChildren();
    for(vector<cXMLElement*>::iterator nodePT = nodes.begin(); nodePT != nodes.end(); nodePT++) {
        cXMLElement* node = (*nodePT);
        int nodeId = atoi(node->getAttribute((char*) "nodeId"));

        // ensures that nodeId match to the index of list
        ASSERT(nodeId == i);
        vector<double>* routeDurations = new vector<double>();

        cXMLElementList rotes = node->getChildren();
        for(vector<cXMLElement*>::iterator routePT = rotes.begin(); routePT != rotes.end(); routePT++) {
            cXMLElement* route = (*routePT);

            cStringTokenizer summaryTok(route->getNodeValue());
            vector<double> routeInfo = summaryTok.asDoubleVector();
            const double day      = routeInfo[0];
            const double dayStart = routeInfo[1];
            const double dayEnd   = routeInfo[2];
            const double savedDuration = routeInfo[3];

            // для расчёта длительности дня используем начало и конец дня. savedDuration игнорируем
            double duration = dayEnd - dayStart;
            if (duration <= 0) {
                cout << "ERROR: nodeId=" << nodeId << ",  day=" << day << ", dayStart=" << dayStart << ", dayEnd="<< dayEnd << ", savedDuration=" << savedDuration << endl;}
            ASSERT(duration > 0);

            routeDurations->push_back(duration);
        }
        if (matrix->size() > 0) {//нужно для проверки, что у всех узлов одинаковое количество дней
            ASSERT(matrix->back()->size() == routeDurations->size());
        }

        matrix->push_back(routeDurations);
        i++;
    }
    return matrix;
}


double StatisticsCalculator::norm(vector<vector<double>*>* matrixOne, vector<vector<double>*>* matrixTwo)
{
    ASSERT(matrixOne->size() == matrixTwo->size());
    for (unsigned int i=0; i<matrixOne->size(); i++) {
        ASSERT(matrixOne->at(i)->size() == matrixTwo->at(i)->size());
    }

    //todo реализовать "рассчитывающую меру их схожести: например, усреднённое СКО по количеству элементов в векторе длительностей"
    return 0;
}


void StatisticsCalculator::print(vector<vector<double>*>* matrix, const char* name)
{
    cout << "durationMatrix from '" << name << "': " << endl;
    for (unsigned int nodeId=0; nodeId<matrix->size(); nodeId++) {
        vector<double>* durationsForNode = matrix->at(nodeId);
        cout << "nodeId = " << nodeId << ": ";
        for (unsigned int day=0; day<durationsForNode->size(); day++) {
            cout << durationsForNode->at(day) << " ";
        }
        cout << endl;
    }
    cout << endl;
}
