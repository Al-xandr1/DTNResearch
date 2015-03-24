#include "LevyMobility.h"
#include "HotSpot.h"
#include "HotSpot.h"

#define DEF_HS_DIR "./hotspotfiles"         //Директория по умолчанию для "горячих точек"

Define_Module(LevyMobility);

LevyMobility::LevyMobility() {
    nextMoveIsWait = false;
    jump = NULL;
    pause = NULL;
    kForSpeed = 1;
    roForSpeed = 0;

    useHotSpots = false;
    useLATP = false;
    useBetweenCentersLogic = false;
    allHotSpots = NULL;
    visitedHotSpots = NULL;
    distMatrix = NULL;
    currentHotSpot = NULL;
    currentIndexHS = -1;
    powA = -1;
}

void LevyMobility::initialize(int stage) {
    LineSegmentsMobilityBase::initialize(stage);

    if (stage == 0) {
        stationary = (par("speed").getType() == 'L' || par("speed").getType() == 'D') && (double) par("speed") == 0;
    }

    if (hasPar("ciJ") && hasPar("aliJ") && hasPar("aciJ")
        && hasPar("ciP") && hasPar("aliP") && hasPar("aciP")) {

        double ciJ  = par("ciJ").doubleValue();
        double aliJ = par("aliJ").doubleValue();
        double aciJ = par("aciJ").doubleValue();

        double ciP  = par("ciP").doubleValue();
        double aliP = par("aliP").doubleValue();
        double aciP = par("aciP").doubleValue();

        if (jump == NULL || pause == NULL) {
            jump = new LeviJump(ciJ, aliJ, aciJ);
            pause = new LeviPause(ciP, aliP, aciP);
        }
    } else {
        cout << "It is necessary to specify ALL parameters for length and pause Levy distribution";
        exit(-112);
    }

    initializeHotSpots();
}

// Инициализирует горячие точки если их использование включено
void LevyMobility::initializeHotSpots() {
    useHotSpots = par("useHotSpots").boolValue();
    if (useHotSpots && allHotSpots == NULL)
    {
        useLATP = par("useLATP").boolValue();
        powA = par("powA").doubleValue();
        useBetweenCentersLogic = par("useBetweenCentersLogic").boolValue();

        HotSpotReader hsReader;
        allHotSpots = hsReader.readAllHotSpots(DEF_HS_DIR);
        visitedHotSpots = new vector<HotSpot*>();
        checkHotSpotsBound();

        if (useBetweenCentersLogic) {
            // заполняем матрицу расстояний между центрами локаций
            distMatrix = new double*[allHotSpots->size()];
            for (uint i = 0; i < allHotSpots->size(); i++) {
                distMatrix[i] = new double[allHotSpots->size()];
                for (uint j = 0; j < allHotSpots->size(); j++) {
                    if (i == j) {
                        distMatrix[i][i] = -1;
                    } else {
                        distMatrix[i][j] = (*allHotSpots)[i]->getDistance((*allHotSpots)[j]);
                    }
                }
            }
        }
    }
}

// Получение дистанции между локациями по их индексу в основном массиве allHotSpots.
// В зависимости от настройки todo <НАСТРОЙКА> считает расстояние либо между центрами локаций,
// либо или между текущим положением и центром другого кластера
double LevyMobility::getDistance(int fromHotSpot, int toHotSpot) {
    if (fromHotSpot == toHotSpot) {
        exit(-765);
    }

    if (useBetweenCentersLogic) {
        return distMatrix[fromHotSpot][toHotSpot];

    } else {
        HotSpot* targetHotSpot = (*allHotSpots)[toHotSpot];
        double deltaX = (targetHotSpot->Xcenter - lastPosition.x),
               deltaY = (targetHotSpot->Ycenter - lastPosition.y);
        return sqrt(deltaX * deltaX + deltaY * deltaY);
    }
}

// Проверяет: что все горячие точки не выходят за общую границу
void LevyMobility::checkHotSpotsBound() {
    if (allHotSpots) {
        for (uint i = 0; i < allHotSpots->size(); i++) {
            HotSpot* hotSpot = (*allHotSpots)[i];
            if (hotSpot->Xmin < constraintAreaMin.x || hotSpot->Xmax > constraintAreaMax.x ||
                hotSpot->Ymin < constraintAreaMin.y || hotSpot->Ymax > constraintAreaMax.y) {
                cout << "HotSpots has wrong bounds!"; EV << "HotSpots has wrong bounds!";
                exit(123);
            }
        }
    }
}

void LevyMobility::setInitialPosition() {
    MobilityBase::setInitialPosition();

    if (useHotSpots) {
        currentHotSpot = getRandomHotSpot(NULL);
        lastPosition = getRandomPositionInsideHS(currentHotSpot);
    }
}

// получаем случайную горячую точку из списка hotSpots, отличную от указанной excludedHotSpot
HotSpot* LevyMobility::getRandomHotSpot(HotSpot* currentHotSpot) {
    uint index = 0;
    HotSpot* newHotSpot = NULL;

    if (useLATP && currentHotSpot != NULL) {
        // выбор по алгоритму "Least action trip planning"
        // и в случае, когда уже установлен текущий кластер, потому что тогда установлен currentIndexHS

        double checkSum = 0; //todo remove

        // вероятности посещения в порядке соответствующем массиву со всеми кластерами
        double* hotSpotProbability = new double[allHotSpots->size()];
        for (uint i = 0; i < allHotSpots->size(); i++) {
            if (i != currentIndexHS) {
                // если кластер не текущий, то считаем вероятность его посещения
                double denominator = 0;
                for (uint k = 0; k < allHotSpots->size(); k++) {
                    bool isVisited = false;

                    HotSpot* currentHS = (*allHotSpots)[k];
                    for (uint j=0; j < visitedHotSpots->size(); j++) {
                        if ( isVisited = (currentHS == (*visitedHotSpots)[j]) ) break;
                    }

                    if (!isVisited) {
                        denominator += 1 / pow(getDistance(currentIndexHS, k), powA);
                    }
                }

                if (denominator == 0) exit(-111);

                hotSpotProbability[i] = 1 / pow(getDistance(currentIndexHS, i), powA) / denominator;
                if (hotSpotProbability[i] > 1) {//todo remove
                    cout << "BINGO" << endl;
                }
            } else {
                // текущий кластер имеет нулевую вероятность посещения
                hotSpotProbability[currentIndexHS] = 0;
            }

            checkSum += hotSpotProbability[i];
        }
        if (checkSum != 1) cout << "\t\t checkSum = " << checkSum << endl;

        double rnd = ((double) rand()) / RAND_MAX,
               probSumm = 0;
        for (uint i = 0; i < allHotSpots->size(); i++)
            if ( (probSumm += hotSpotProbability[i]) >= rnd ) {index = i; break;}
        newHotSpot = (*allHotSpots)[index];

    } else {
        // обычный случайный выбор кластера: если без LATP либо при первом выставлении текущего кластера
        newHotSpot = currentHotSpot;
        do {
            index = rint(uniform(0, allHotSpots->size() - 1));
            newHotSpot = (*allHotSpots)[index];
        } while (newHotSpot == currentHotSpot);
    }

    // запоминаем текущий индекс
    if (index == currentIndexHS) exit(-432);
    currentIndexHS = index;
    if (useLATP) {
        // обновляем множество посещённых кластеров
        visitedHotSpots->push_back(newHotSpot);
        // если кол-во посещённых кластеров больше некоторого порога, то удаляем самую старую,
        // тем самым повышая вероятность вернуться туда опять
        if (visitedHotSpots->size() >=  rint(allHotSpots->size() * 0.8)) {
            visitedHotSpots->erase(visitedHotSpots->begin());
        }
    }

    cout << "getRandomHotSpot: index = " << currentIndexHS << ", size = " << allHotSpots->size() << endl;
    return newHotSpot;
}

// получаем случайное положение внутри заданной горячей точки
Coord LevyMobility::getRandomPositionInsideHS(HotSpot* hotSpot) {
    Coord newPoint(uniform(hotSpot->Xmin, hotSpot->Xmax),
                   uniform(hotSpot->Ymin, hotSpot->Ymax),
                   0);
    if (!hotSpot->isPointBelong(newPoint)){
        exit(-343);
    }
    return newPoint;
}

void LevyMobility::finish() {
    saveStatistics();
}

void LevyMobility::setTargetPosition() {
    if (nextMoveIsWait) {
        simtime_t waitTime = (simtime_t) pause->get_Levi_rv();
        nextChange = simTime() + waitTime;
    } else {
        collectStatistics(simTime(), lastPosition.x, lastPosition.y);

        generateNextPosition(targetPosition, nextChange);
    }
    nextMoveIsWait = !nextMoveIsWait;
}

// Генерирует следующую позицию в зависимости от того, включено использование горячих точек или нет
void LevyMobility::generateNextPosition(Coord& targetPosition, simtime_t& nextChange) {
    // генерируем как обычно
    const double angle = uniform(0, 2 * PI);
    const double distance = jump->get_Levi_rv();
    const double speed = kForSpeed * pow(distance, 1 - roForSpeed);
    Coord delta(distance * cos(angle), distance * sin(angle), 0);
    simtime_t travelTime = distance / speed;

    targetPosition = lastPosition + delta;
    nextChange = simTime() + travelTime;

     // принадлежит ли новая точка текущей горячей точке
     if ( useHotSpots && !currentHotSpot->isPointBelong(targetPosition) ) {
         // для ускорения пределяем вспомогательные переменные
         double x, y, Xdir, Ydir, dir;
         bool flag = ( (y=lastPosition.y) < currentHotSpot->Ycenter);

         // выбираем самую дальнюю вершину прямоугольника
         if ( (x=lastPosition.x) < currentHotSpot->Xcenter ) {
             if (flag) { Xdir=currentHotSpot->Xmax-x; Ydir=currentHotSpot->Ymax-y; }
             else      { Xdir=currentHotSpot->Xmax-x; Ydir=currentHotSpot->Ymin-y; }
         } else {
             if (flag) { Xdir=currentHotSpot->Xmin-x; Ydir=currentHotSpot->Ymax-y; }
             else      { Xdir=currentHotSpot->Xmin-x; Ydir=currentHotSpot->Ymin-y; }
         }

         // проверяем, можем ли остаться в прямоугольнике
         if ( distance > (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
             // TODO в этом случае или сразу ищем новый кластер или до определённой длины (порога) ещё остаёмся в этом, заново генерируя шаг
             //пусть пока выбираем случайный кластер
             currentHotSpot = getRandomHotSpot(currentHotSpot);
             targetPosition = getRandomPositionInsideHS(currentHotSpot);
         } else {
             delta.x = Xdir * distance/dir;
             delta.y = Ydir * distance/dir;
             targetPosition = lastPosition + delta;
         }
     }
}

void LevyMobility::move() {
    LineSegmentsMobilityBase::move();
}

//-------------------------- Statistic collection ---------------------------------
void LevyMobility::collectStatistics(simtime_t appearenceTime, double x, double y) {
    times.push_back(appearenceTime);
    xCoordinates.push_back(x);
    yCoordinates.push_back(y);
}

void LevyMobility::saveStatistics() {
    char outFileName[256];
    char *fileName = createFileName(outFileName, 0,
            par("traceFileName").stringValue(), (int) ((par("fileSuffix"))));

    ofstream* file = new ofstream(fileName);
    for (int i = 0; i < times.size(); i++) {
        simtime_t time = times[i];
        double x = xCoordinates[i];
        double y = yCoordinates[i];
        (*file) << time << "\t" << x << "\t" << y << endl;
    }

    file->close();
    delete file;
}
