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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#define PI 3.1415926

class Levi {
protected:
    double c, alpha;  // Levi distribution parameters
    double accuracy;   // accuracy for numerical calculation
    double xinv[100], Finv[100];
    int imax;
    double Fmax;
    double step;
    double lim;  // numerical integraton parameters

public:
    // methods for probability density calculation
    double integral(double x);
    double probdens(double x);
    // methods for distribution function calculation
    double integralF(double x);
    double distFun(double x);
    // constructor with inverse function calculations
    Levi(double ci, double ali, double aci);
    // methods for r.v. generation
    double get_Levi_rv();
};

double Levi::integral(double x) {
    double h, t;
    h = 0;
    for (t = -lim + step / 2; t < lim; t += step)
        h += cos(t * x) * exp(-pow(fabs(c * t), alpha)) * step;
    return h;
}

double Levi::probdens(double x) {
    double s1, s2;

// ajusting partition
    s2 = integral(x);
    do {
        s1 = s2;
        step /= 2;
        s2 = integral(x);
    } while (fabs((s1 - s2) / s1) > accuracy);
    step *= 2;
    s2 = s1;
// ajusting interval of integration
    do {
        s1 = s2;
        lim += 1 / c;
        s2 = integral(x);
    } while (fabs((s1 - s2) / s1) > accuracy);
    lim -= 1 / c;
    return s1 / 2 / PI;
}

double Levi::integralF(double x) {
    double h, hh, t;
    h = 0;
    for (t = -lim + step / 2; t < lim; t += step) {
        hh = (t != 0) ? sin(t * x) / t : 1.0;
        h += hh * exp(-pow(fabs(c * t), alpha)) * step;
    }
    return h;
}

double Levi::distFun(double x) {
    double s1, s2;

// ajusting partition
    s2 = integralF(x);
    do {
        s1 = s2;
        step /= 2;
        s2 = integralF(x);
    } while (fabs((s1 - s2) / (PI + s1)) > accuracy);
    step *= 2;
    s2 = s1;
// ajusting interval of integration
    do {
        s1 = s2;
        lim += 1 / c;
        s2 = integralF(x);
    } while (fabs((s1 - s2) / (PI + s1)) > accuracy);
    lim -= 1 / c;
    return 0.5 + s1 / 2 / PI;
}

Levi::Levi(double ci, double ali, double aci) {
    c = ci;
    alpha = ali;
    accuracy = aci;
    lim = 1 / c;
    step = 0.01;
    Fmax = 1000 * c;
    printf("c=%g,  alpha=%g,  acc=%g\n", c, alpha, accuracy);
    printf("step=%g,   lim=%g,  Fmax=%g    \n", step, lim, Fmax);

// inverse function calculation
    double arg, fun, stp;
    fun = xinv[0] = 0.5;
    Finv[0] = arg = imax = 0;
    while (imax < 100 && fun < 0.99) {
        stp = 1000;
        while (distFun(arg + stp) > fun + 0.01)
            stp /= 2;
        fun = xinv[imax] = distFun(Finv[imax] = arg += stp);
        printf("i=%d x=%g  Finv=%g  Fi=%g\n", imax, xinv[imax], Finv[imax],
                tan(PI * (xinv[imax] - 0.5)));
        imax++;
    }
    xinv[imax] = 1;
    Finv[imax] = Fmax;
}

double Levi::get_Levi_rv() {
    int i;
    double rnd, h;
    do
        rnd = ((double) rand()) / RAND_MAX;
    while (rnd < 0.5);
    i = 0;
    while (xinv[i] < rnd && i < imax)
        i++;
    h = Finv[i - 1]
            + (rnd - xinv[i - 1]) * (Finv[i] - Finv[i - 1])
                    / (xinv[i] - xinv[i - 1]);
    return h;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
//        cout << "               updateN: (2*_M)/_h = (2*" << _M << ")/" << _h << endl;
        _N = round((2 * _M) / _h);
//        cout << "               updateN: _N = " << _N << endl;
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
//        cout << endl << "TRACE: Grid(long N, double M, bool symmetric): _N = " << _N << ", _M = " << _M << ", _symmetric = " << _symmetric << endl << endl ;
        updateH();
        updateGrid();
    }

    // Сетка на основе начального интервала интегрирования
    // и шага разбиения
    Grid(double h, double M, bool symmetric, int fictiveVar) {
        this->_M = M;
        this->_h = h;
        this->_symmetric = symmetric;
//        cout << endl << "TRACE: Grid(double h, double M, bool symmetric, int fictiveVar) : _M = " << _M << ", _h = " << _h << ", _symmetric = " << _symmetric << endl << endl ;
        updateN();
        updateGrid();
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
            if (i % 10 == 0)
                cout << endl;
        }
    }
};


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
//        double x = 0;
//        double M = 1/_c;
//        long N = 10;
//        double H = 2 * M / N;
//        cout << "       Initial H = " << H << ", Initial M = " << M << endl;
//        double targetH = find_H(N, M, x, _eps, true);
//        double targetM = find_M(H, M, x, _eps, true);
//        cout << "       Target H = " << targetH << ", Target M = " << targetM << endl << endl;
//        Grid gridForIntegral(targetH, targetM, true, 1); // создаём сетку для интеграла с подзодящими N & M

        Grid gridForProbDensity(100, 100, false); // todo N & M для расчёта значений функции плотности
        vector<double> abscissa = gridForProbDensity.get_Grid();

        probDensity.resize(abscissa.size());
        for (int i = 0; i < abscissa.size(); i++) {
            probDensity[i] = autoSolveIntegral(abscissa[i]);
        }

        cout << endl << "   Probability density of Levy distribution:" << endl << endl;
        gridForProbDensity.printVector(probDensity);
        cout << endl << "   Sum of probability density = " << summ(probDensity) << endl << endl;
        cout << "   Initializing of Probability Density completed." << endl;
    }

    double autoSolveIntegral(double x){
        int N = 10;
        double M = 1/_c;
        double s1, s2;
        Grid grid(N, M, true);

        s2 = solveIntegral(x, grid);
        do {
            s1 = s2;
            grid.set_N(2 * grid.get_N());
            s2 = solveIntegral(x, grid);
        } while (norm(s1, s2) > _eps);

        s2 = solveIntegral(x, grid);
        do {
            s1 = s2;
            grid.set_M(2 * grid.get_M());
            s2 = solveIntegral(x, grid);
        } while (norm(s1, s2) > _eps);

        return s1/2.0/3.14;
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
        return cos(t * x) * exp(-pow(fabs(_c * t), _alpha));
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

        double s1, s2;
        Grid grid(H, initialM, symmetric, 0);

        do {
            s1 = solveIntegral(x, grid);

            initialM = 2 * grid.get_M();
            grid.set_M(initialM);

            s2 = solveIntegral(x, grid);

        } while (norm(s1, s2) > eps);

        return initialM;
    }

    // Расчёт нормы погрешности
    static double norm(double s1, double s2) {
        return fabs(s1 - s2)/s1;
    }

    static double summ(vector<double> vector) {
        double summ = 0;
        for (int i = 0; i < vector.size(); i++)
            summ += vector[i];

        return summ;
    }
};

int main() {
//    LeavyGenerator gen(1, 1, 0.0001);
//    gen.initialize();

        Levi a(1, 1, 0.0001);
        for (int i = 1; i < 10; i++)
            printf("%g   ", a.get_Levi_rv());

    return 0;
}

///--------------------------------------------------------
