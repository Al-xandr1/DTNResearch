//============================================================================
// Name        : LeavyGenerator.cpp
// Author      : alexander
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <math.h>

using namespace std::__1;

// Сетка для интегрирования
class Grid {
private:
    long _N;         // Колчичество интервалов разбиения для оси абсцисс
    double _M;       // Границы интервала интегрирования
    double _h;       // шаг разбиения для оси абсцисс
    bool _symmetric; // означает симметричные ли границы интервала интегрирования (true - [-M;M]) или нет (false - [0;2M])
    std::vector<double> _grid; // Значения сетки разбиения

    void updateH() {
        _h = (2 * _M) / (1.0 * _N);
    }

    void updateN() {
        cout << "               updateN: (2*_M)/_h = (2*" << _M << ")/" << _h << endl;
        _N = floor((2 * _M) / _h);
        cout << "               updateN: _N = " << _N << endl;
    }

    void updateGrid() {
        _grid.clear();
        _grid.resize(_N + 1);
        for (long i = 0; i < _grid.size(); i++) {
            _grid[i] = getLeftBound() + i * _h;
        }
    }

public:

    // Сетка на основе начального интервала интегрирования
    // и количества интервалов разбиения
    Grid(long N, double M, bool symmetric) {
        this->_N = N;
        this->_M = M;
        this->_symmetric = symmetric;
        cout << endl << "TRACE: Grid(long N, double M, bool symmetric): _N = " << _N << ", _M = " << _M << ", _symmetric = " << _symmetric << endl << endl ;
        updateH();
        updateGrid();
    }

    // Сетка на основе начального интервала интегрирования
    // и шага разбиения
    Grid(double h, double M, bool symmetric, int fictiveVar) {
        this->_M = M;
        this->_h = h;
        this->_symmetric = symmetric;
        cout << endl << "TRACE: Grid(double h, double M, bool symmetric, int fictiveVar) : _M = " << _M << ", _h = " << _h << ", _symmetric = " << _symmetric << endl << endl ;
        updateN();
        updateGrid();
    }

    ~Grid() {
//        ~_grid();
        //todo описать удаление всех объектов
    }

    void set_N(long N) {
        this->_N = N;
        updateH();
        updateGrid();
    }

    long get_N() {
        return _N;
    }

    void set_M(double M) {
        this->_M = M;
        updateN();
        updateGrid();
    }

    double get_M() {
        return _M;
    }

    double getLeftBound() {
        if (_symmetric)
            return -get_M();
        else
            return 0;
    }

    double getRightBound() {
        if (_symmetric)
            return get_M();
        else
            return 2 * get_M();
    }

    double get_h() {
        return _h;
    }

    bool isSymmetric() {
        return _symmetric;
    }

    vector<double> get_Grid() {
        return _grid;
    }

    void printVector() {
        printVector(this->_grid);
    }

    void static printVector(vector<double> vector) {
        for (long i = 0; i < vector.size(); i++) {
            cout << vector[i] << " ";
            if (i % 20 == 0)
                cout << endl;
        }
    }
};

#define CTpowALPHA(c, t, alpha) (pow(fabs(c * t), alpha))

//Кдасс для расчёта плотности распределения Леви
//и для последующей генерации случайных чисел.
class LeavyGenerator {

private:
    double _c;         // нормировочная константа
    double _alpha;     // параметр распределения
    double _eps;       // точность

    vector<double> probDensity; // значения плотности вероятностей Леви

public:
    LeavyGenerator(double alpha, double c, double eps) {
        this->_alpha = alpha;
        this->_c = c;
        this->_eps = eps;
    }

    ~LeavyGenerator() {
        //todo описать удаление всех объектов
    }

    // генерирует новое число Леви
    double nextValue() {
        //todo генерировать новое число
        return 0;
    }

    void initialize() {
        cout << "Generator initializing..." << endl;
        initializeProbabilityDensity();
        initializeDistributionFunction();
        cout << "Generator initializing completed." << endl;
    }

private:
    // Расчитывает плотность вероятностей Леви
    void initializeProbabilityDensity() {
        cout << "   Initializing of Probability Density..." << endl << endl;

        // Определяем нужные значения N & M
        double x = 0;
        double M = CTpowALPHA(_c, 1, _alpha);
        long N = 10;
        double H = 2 * M / N;

        cout << "       Initial H = " << H << ", Initial M = " << M << endl;
        double targetH = find_H(N, M, x, _eps, true);
        double targetM = find_M(H, M, x, _eps, true);
        cout << "       Target H = " << targetH << ", Target M = " << targetM << endl << endl;

        Grid gridForProbDensity(100, 100, false); // todo N & M для расчёта значений функции плотности
        vector<double> abscissa = gridForProbDensity.get_Grid();
//        gridForProbDensity.printVector(); //todo remove
        cout << endl << "           count integral.. " << endl << endl;

        probDensity.resize(abscissa.size());

        Grid gridForIntegral(targetH, targetM, true, 1); // создаём сетку для интеграла с подзодящими N & M
        cout << "           grid for integral..." << endl;
//        gridForIntegral.printVector();
        for (int i = 0; i < abscissa.size(); i++) {
            probDensity[i] = solveIntegral(abscissa[i], gridForIntegral);
        }

        cout << endl << "   Probability density of Levy distribution:" << endl << endl;
        gridForProbDensity.printVector(probDensity);
        cout << endl << "   Sum of probability density = " << summ(probDensity) << endl << endl;
        cout << "   Initializing of Probability Density completed." << endl;
    }

    // Решение интеграла по формуле Котеса
    // x - переменнная исходной функции
    // grid - сетка интегрирования
    double solveIntegral(double x, Grid &grid) {
        vector<double> vec = grid.get_Grid();

        double sum = (func(vec[0], x) + func(vec[vec.size() - 1], x)) / 2; // (f_0+f_n)/2
        for (int i = 1; i < vec.size() - 1; i++) {
            sum += func(vec[i], x);   //sum(f_i)
        }
        sum *= grid.get_h();

        return sum;
    }

    // это подынтегральное выражение для любого x
    double func(double t, double x) {
        if (x == 0)
            return func_0(t);
        else
            return cos(t * x) * func_0(t);
    }

    // это подынтегральное выражение для x=0
    double func_0(double t) {
        return exp(-1 * CTpowALPHA(_c, t, _alpha));
    }

    // Расчитывает функцию распрделения Леви
    void initializeDistributionFunction() {
        cout << "   Initializing of Distribution Function..." << endl;
        //todo
        cout << "   Initializing of Distribution Function completed." << endl;
    }

    //---------- static members ---------------

    // Определяет подходящее разбиение N для заданной точности
    // initialN - начальное значение разбиения
    // M - интервал интегрирования [-M; M] или [0, 2*M] (в зависимости от symmetric)
    // x - значение аргумента функции
    // eps - заданная точность
    // return - подходящий шаг
    double find_H(long initialN, double M,
            double x, double eps, bool symmetric) {

        double s1, s2;
        Grid grid(initialN, M, symmetric);

        do {
            s1 = solveIntegral(x, grid);

            initialN = 2 * grid.get_N();
            grid.set_N(initialN);

            s2 = solveIntegral(x, grid);

        } while (norm(s1, s2) > eps);

        return grid.get_h();
    }

    // Определяет интервал интегрирования
    // initialM - начальный интервал интегрирования [-initialM; initialM] или [0, 2*initialM] (в зависимости от symmetric)
    // H - нужный шаг интегрирования
    // x - значение аргумента функции
    // eps - заданная точность
    // return - подходящий интервал инегрирования
    double find_M(double H, double initialM,
            double x, double eps, bool symmetric) {
        cout << "              !!! find_M start....  " << endl;

        int i = 1;
        double s1, s2;
        cout << "              !!! H = "<< H << ", M = " << initialM << endl;
        Grid grid(H, initialM, symmetric, 0);

        do {
            s1 = solveIntegral(x, grid);

            initialM = 2 * grid.get_M();
//            cout << "              !!! first set initialM " << initialM << ", _h = " << grid.get_h() << endl;
            grid.set_M(initialM);
//            cout << "              !!! after update initialM " << initialM << ", _h = " << grid.get_h()<< endl;

            s2 = solveIntegral(x, grid);

//            if (i++ % 5 == 0) {
//                cout << "            i = " << i++ << endl;
//                cout << "           s1 = " << s1 << ", M = " << initialM << ", h = " << H << endl;
//                cout << "           s2 = " << s2 << ", M = " << initialM << ", h = " << H << endl;
//                cout << endl << endl;
//            }

        } while (norm(s1, s2) > eps);

        return initialM;
    }

    // Расчёт нормы погрешности
    static double norm(double s1, double s2) {
        return fabsl(s1 - s2)/s1;         //todo правильно считается погрешность???
    }

    static double summ(vector<double> vector) {
        double summ = 0;
        for (int i = 0; i < vector.size(); i++)
            summ += vector[i];

        return summ;
    }
};

int main() {
    LeavyGenerator gen(1, 1, 0.00000001); // 0.1%
    gen.initialize();
//    cout << "nextVal = " << gen.nextValue() << endl;

    return 0;
}
