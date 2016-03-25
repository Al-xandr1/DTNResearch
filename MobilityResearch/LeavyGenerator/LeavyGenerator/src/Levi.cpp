/*
 * Leavy.cpp
 *
 *  Created on: 05 дек. 2014 г.
 *      Author: Alexander
 */

// Levi distribution
// #include <omnetpp.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
//
//void main() {
//    Levi a(1, 1, 0.0001);
//    for (int i = 1; i < 10; i++)
//        printf("%g   ", a.get_Levi_rv());
//}
//
