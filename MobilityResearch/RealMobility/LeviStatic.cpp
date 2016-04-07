#include "LeviStatic.h"

bool LeviJump::isTableReady = false;

double LeviJump::c = 1;
double LeviJump::alpha = 1;
double LeviJump::accuracy = 0.001;

double LeviJump::xinv[100];
double LeviJump::Finv[100];
int LeviJump::imax;
double LeviJump::Fmax;
double LeviJump::step;
double LeviJump::lim;

bool LeviPause::isTableReady = false;

double LeviPause::c = 1;
double LeviPause::alpha = 1;
double LeviPause::accuracy = 0.001;

double LeviPause::xinv[100];
double LeviPause::Finv[100];
int LeviPause::imax;
double LeviPause::Fmax;
double LeviPause::step;
double LeviPause::lim;

// -----------------------------------------------------------------

double LeviJump::integralF(double x) {
    double h, hh, t;
    h = 0;
    for (t = -lim + step / 2; t < lim; t += step) {
        hh = (t != 0) ? sin(t * x) / t : 1.0;
        h += hh * exp(-pow(fabs(c * t), alpha)) * step;
    }
    return h;
}

double LeviJump::distFun(double x) {
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

LeviJump::LeviJump(double ci, double ali, double aci) {
    if (!isTableReady) {
        c = ci;
        alpha = ali;
        accuracy = aci;
        lim = 1 / c;
        step = 0.01;
        Fmax = 1000 * c;

        // inverse function calculation
        double arg, fun, stp;
        fun = xinv[0] = 0.5;
        Finv[0] = arg = imax = 0;
        while (imax < 100 && fun < 0.99) {
            stp = 1000;
            while (distFun(arg + stp) > fun + 0.01)
                stp /= 2;
            fun = xinv[imax] = distFun(Finv[imax] = arg += stp);
            imax++;
        }
        xinv[imax] = 1;
        Finv[imax] = Fmax;
        isTableReady = true;
    }
}

double LeviJump::get_Levi_rv() {
    int i;
    double rnd, h;

    // do
    //   rnd = ((double) rand()) / RAND_MAX;
    // while (rnd < 0.5);
    rnd= 0.5 + (0.5*rand())/RAND_MAX;
    i = 0;
    while (xinv[i] < rnd && i < imax)
        i++;
    h = Finv[i - 1]
            + (rnd - xinv[i - 1]) * (Finv[i] - Finv[i - 1])
                    / (xinv[i] - xinv[i - 1]);
    return h;
}

// -------------------------------------------------------------------

double LeviPause::integralF(double x) {
    double h, hh, t;
    h = 0;
    for (t = -lim + step / 2; t < lim; t += step) {
        hh = (t != 0) ? sin(t * x) / t : 1.0;
        h += hh * exp(-pow(fabs(c * t), alpha)) * step;
    }
    return h;
}

double LeviPause::distFun(double x) {
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

LeviPause::LeviPause(double ci, double ali, double aci) {
    if (!isTableReady) {
        c = ci;
        alpha = ali;
        accuracy = aci;
        lim = 1 / c;
        step = 0.01;
        Fmax = 1000 * c;

        // inverse function calculation
        double arg, fun, stp;
        fun = xinv[0] = 0.5;
        Finv[0] = arg = imax = 0;
        while (imax < 100 && fun < 0.99) {
            stp = 1000;
            while (distFun(arg + stp) > fun + 0.01)
                stp /= 2;
            fun = xinv[imax] = distFun(Finv[imax] = arg += stp);
            imax++;
        }
        xinv[imax] = 1;
        Finv[imax] = Fmax;
        isTableReady = true;
    }
}

double LeviPause::get_Levi_rv() {
    int i;
    double rnd, h;

    // do
    //    rnd = ((double) rand()) / RAND_MAX;
    // while (rnd < 0.5);
    rnd= 0.5 + (0.5*rand())/RAND_MAX;
    i = 0;
    while (xinv[i] < rnd && i < imax)
        i++;
    h = Finv[i - 1]
            + (rnd - xinv[i - 1]) * (Finv[i] - Finv[i - 1])
                    / (xinv[i] - xinv[i - 1]);
    return h;
}

