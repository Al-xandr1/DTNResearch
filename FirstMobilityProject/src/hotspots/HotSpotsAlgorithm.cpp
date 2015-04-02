/*
 * HotSpotsAlgorithm.cpp
 *
 *  Created on: 31 марта 2015 г.
 *      Author: atsarev
 */

#include <HotSpotsAlgorithm.h>

HotSpotsAlgorithm::HotSpotsAlgorithm(LevyMobilityDEF* levyMobility, double powA,
        bool useLATP, bool useBetweenCentersLogic, bool useHotSpotAvailabilities) {

    this->levyMobility = levyMobility;

    this->useLATP = useLATP;
    this->useBetweenCentersLogic = useBetweenCentersLogic;
    this->useHotSpotAvailabilities = useHotSpotAvailabilities;

    this->allHotSpots = NULL;
    this->distMatrix = NULL;
    this->currentIndexHS = -1;
    this->powA = powA;

    initialize();
}

HotSpotsAlgorithm::~HotSpotsAlgorithm() {
}

void HotSpotsAlgorithm::initialize() {
    HotSpotReader hsReader;
    allHotSpots = hsReader.readAllHotSpots(DEF_HS_DIR);
    checkHotSpotsBound();

    initializeHotSpotAvailabilities();
    initializeDistanceMatrix();
}

// Инициализирует доступность всех кластеров на основании файла spotcount.cnt
void HotSpotsAlgorithm::initializeHotSpotAvailabilities() {
    availabilityPerHS = new vector<int>();

    if (useHotSpotAvailabilities) {
        HotSpotReader hsReader;
        vector<HotSpotAvailability*>* hsAvailabilities = hsReader.readHotSpotsAvailabilities(DEF_HS_DIR);
        // сопоставление двух структур по имени кластера
        for(uint i = 0; i < allHotSpots->size(); i++) {
            char* hotSpotName = (*allHotSpots)[i]->hotSpotName;
            bool found = false;
            for (uint j = 0; j < hsAvailabilities->size(); j++) {
                cout << "\t\thotSpotName =" << hotSpotName << ", (*hsAvailabilities)[" << j << "]->hotSpotName =" << (*hsAvailabilities)[j]->hotSpotName << "." << endl;
                if (found = (strcmp(hotSpotName, (*hsAvailabilities)[j]->hotSpotName) == 0) ) {
                    availabilityPerHS->push_back((*hsAvailabilities)[j]->count);
                    break;
                }
            }
            if (!found) exit(-127);
        }
        // проверка соответствия и удаление временного вектора
        for(uint i = 0; i < allHotSpots->size(); i++) {
            cout << "hotSpotName = " << (*allHotSpots)[i]->hotSpotName << ", count = " << (*availabilityPerHS)[i] << endl;
            HotSpotAvailability* hs = hsAvailabilities->front();
            hsAvailabilities->erase(hsAvailabilities->begin());
            delete hs;
        }
        delete hsAvailabilities;
    } else {
        for (uint i = 0; i < allHotSpots->size(); i++) availabilityPerHS->push_back(1);
    }
}

// Инициализирует матрицу расстояний между центрами кластеров
void HotSpotsAlgorithm::initializeDistanceMatrix() {
    if (useBetweenCentersLogic) {
        // заполняем матрицу расстояний между центрами локаций
        distMatrix = new double*[allHotSpots->size()];
        for (uint i = 0; i < allHotSpots->size(); i++) {
            distMatrix[i] = new double[allHotSpots->size()];
            for (uint j = 0; j < allHotSpots->size(); j++) {
                if (i == j) {
                    distMatrix[i][i] = 0;
                } else {
                    distMatrix[i][j] = (*allHotSpots)[i]->getDistance((*allHotSpots)[j]);
                }
            }
        }
    }
}

// Проверяет, что все горячие точки не выходят за общую границу
void HotSpotsAlgorithm::checkHotSpotsBound() {
    if (allHotSpots) {
        for (uint i = 0; i < allHotSpots->size(); i++) {
            HotSpot* hotSpot = (*allHotSpots)[i];
            if (hotSpot->Xmin < levyMobility->getConstraintAreaMin().x || hotSpot->Xmax > levyMobility->getConstraintAreaMax().x ||
                hotSpot->Ymin < levyMobility->getConstraintAreaMin().y || hotSpot->Ymax > levyMobility->getConstraintAreaMax().y) {
                cout << "HotSpots has wrong bounds!"; EV << "HotSpots has wrong bounds!";
                exit(123);
            }
        }
    }
}

// получаем индекс новой горячой точки из списка allHotSpots, отличный от текущего
// возвращаяет TRUE - можно выбрать новый кластер, FALSE - в противном случае
bool HotSpotsAlgorithm::setNextCurrentHotSpotIndex() {
    int resultIndex = -1;

    if (useLATP && currentIndexHS != -1) {
        // выбор по алгоритму "Least action trip planning" И в случае, когда уже установлен текущий кластер
        double checkSum = 0;
        // вероятности посещения в порядке соответствующем массиву со всеми кластерами
        double* hotSpotProbability = new double[allHotSpots->size()];
        for (uint i = 0; i < allHotSpots->size(); i++) {
            if (isAvailable(i)) {
                // если кластер доступен, то считаем вероятность его посещения
                double denominator = 0;
                for (uint k = 0; k < allHotSpots->size(); k++)
                    if (isAvailable(k))
                        denominator += 1 / pow(getDistanceFromCurrentHS(k), powA);
                if (denominator == 0) exit(-111);

                checkSum += (hotSpotProbability[i] = 1 / pow(getDistanceFromCurrentHS(i), powA) / denominator);
            } else {
                // кластер имеет нулевую вероятность посещения
                checkSum += (hotSpotProbability[i] = 0);
            }
        }
        if (checkSum != 1) cout << "\t\t checkSum = " << checkSum << endl;

        double rnd = ((double) rand()) / RAND_MAX,
               probSumm = 0;
        for (uint i = 0; i < allHotSpots->size(); i++)
            if ( (probSumm += hotSpotProbability[i]) >= rnd ) {resultIndex = i; break;}

    } else {
        // обычный случайный выбор кластера: если без LATP либо при первом выставлении текущего кластера
        resultIndex = currentIndexHS;
        do {
            resultIndex = rint(uniform(0, allHotSpots->size() - 1));
        } while (resultIndex == currentIndexHS);
    }

    if (resultIndex == currentIndexHS) exit(-432);
    if (resultIndex == -1) return false; // не смогли выбрать новый кластер

    if (useLATP) setVisited(resultIndex);
    currentIndexHS = resultIndex;
    cout << "setNextCurrentHotSpotIndex: index = " << currentIndexHS << ", size = " << allHotSpots->size() << endl;
    return true;
}

// получаем случайное положение внутри заданной горячей точки
Coord HotSpotsAlgorithm::getRandomPositionInsideHS(uint hotSpotIndex) {
    HotSpot* hotSpot = (*allHotSpots)[hotSpotIndex];
    Coord newPoint(uniform(hotSpot->Xmin, hotSpot->Xmax),
                   uniform(hotSpot->Ymin, hotSpot->Ymax),
                   0);
    if (!hotSpot->isPointBelong(newPoint)){
        exit(-343);
    }
    return newPoint;
}

Coord HotSpotsAlgorithm::getInitialPosition() {
    if (!setNextCurrentHotSpotIndex()) {
        exit(-344); // при выставдлении начального положения обязательно должен быть выбран кластер
    }
    return getRandomPositionInsideHS(currentIndexHS);
}

// попраляет точку в зависимости от кластеров.
// возвращаяет TRUE - если правка прошла удачно, FALSE - в противном случае
bool HotSpotsAlgorithm::fixTargetPosition(Coord& targetPosition, Coord delta, double distance) {
    HotSpot* currentHotSpot = (*allHotSpots)[currentIndexHS];
    // принадлежит ли новая точка текущей горячей точке
    if ( !currentHotSpot->isPointBelong(targetPosition) ) {
        // для ускорения пределяем вспомогательные переменные
        double x, y, Xdir, Ydir, dir;
        bool flag = ( (y=levyMobility->getLastPosition().y) < currentHotSpot->Ycenter);

        // выбираем самую дальнюю вершину прямоугольника
        if ( (x=levyMobility->getLastPosition().x) < currentHotSpot->Xcenter ) {
            if (flag) { Xdir=currentHotSpot->Xmax-x; Ydir=currentHotSpot->Ymax-y; }
            else      { Xdir=currentHotSpot->Xmax-x; Ydir=currentHotSpot->Ymin-y; }
        } else {
            if (flag) { Xdir=currentHotSpot->Xmin-x; Ydir=currentHotSpot->Ymax-y; }
            else      { Xdir=currentHotSpot->Xmin-x; Ydir=currentHotSpot->Ymin-y; }
        }

        // проверяем, можем ли остаться в прямоугольнике
        if ( distance > (dir=sqrt(Xdir*Xdir+Ydir*Ydir)) ) {
            if (!setNextCurrentHotSpotIndex())
                return false;
            targetPosition = getRandomPositionInsideHS(currentIndexHS);

        } else {
            delta.x = Xdir * distance/dir;
            delta.y = Ydir * distance/dir;
            targetPosition = levyMobility->getLastPosition() + delta;
        }
    }

    return true;
}

// Получение дистанции между текущей локацией и указанной по их индексу в основном массиве allHotSpots.
// В зависимости от настройки useBetweenCentersLogic считает расстояние либо между центрами локаций,
// либо или между текущим положением и центром другого кластера
double HotSpotsAlgorithm::getDistanceFromCurrentHS(uint targetHotSpotIndex) {
    if (currentIndexHS == targetHotSpotIndex) {
        exit(-765);
    }

    if (useBetweenCentersLogic) {
        return distMatrix[currentIndexHS][targetHotSpotIndex];

    } else {
        HotSpot* targetHotSpot = (*allHotSpots)[targetHotSpotIndex];
        double deltaX = (targetHotSpot->Xcenter - levyMobility->getLastPosition().x),
               deltaY = (targetHotSpot->Ycenter - levyMobility->getLastPosition().y);
        return sqrt(deltaX * deltaX + deltaY * deltaY);
    }
}

// проверяет доступен ли кластер для посещения или нет
// на основании проверки "НЕ текущий кластер" и счётчика доступности
bool HotSpotsAlgorithm::isAvailable(uint hotSpotIndex) {
    return (hotSpotIndex != currentIndexHS) && (*availabilityPerHS)[hotSpotIndex] > 0;
}

// обновляет доступность кластера, если он НЕ текущий и доступен по счётчику
void HotSpotsAlgorithm::setVisited(uint hotSpotIndex) {
    if (isAvailable(hotSpotIndex))
        (*availabilityPerHS)[hotSpotIndex]--;
    else
        exit(-766);
}
