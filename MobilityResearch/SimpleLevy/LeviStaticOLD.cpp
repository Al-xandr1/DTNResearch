#include "LeviStaticOLD.h"

bool LeviJumpOLD::isTableReady = false;

double LeviJumpOLD::c = 1;
double LeviJumpOLD::alpha = 1;
double LeviJumpOLD::accuracy = 0.001;

double LeviJumpOLD::xinv[100];
double LeviJumpOLD::Finv[100];
int LeviJumpOLD::imax;
double LeviJumpOLD::Fmax;
double LeviJumpOLD::step;
double LeviJumpOLD::lim;

bool LeviPauseOLD::isTableReady = false;

double LeviPauseOLD::c = 1;
double LeviPauseOLD::alpha = 1;
double LeviPauseOLD::accuracy = 0.001;

double LeviPauseOLD::xinv[100];
double LeviPauseOLD::Finv[100];
int LeviPauseOLD::imax;
double LeviPauseOLD::Fmax;
double LeviPauseOLD::step;
double LeviPauseOLD::lim;

// -----------------------------------------------------------------

double LeviJumpOLD::integralF(double x) {
    double h, hh, t;
    h = 0;
    for (t = -lim + step / 2; t < lim; t += step) {
        hh = (t != 0) ? sin(t * x) / t : 1.0;
        h += hh * exp(-pow(fabs(c * t), alpha)) * step;
    }
    return h;
}

double LeviJumpOLD::distFun(double x) {
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

LeviJumpOLD::LeviJumpOLD(double ci, double ali, double aci) {
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

double LeviJumpOLD::get_Levi_rv() {
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

double LeviPauseOLD::integralF(double x) {
    double h, hh, t;
    h = 0;
    for (t = -lim + step / 2; t < lim; t += step) {
        hh = (t != 0) ? sin(t * x) / t : 1.0;
        h += hh * exp(-pow(fabs(c * t), alpha)) * step;
    }
    return h;
}

double LeviPauseOLD::distFun(double x) {
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

LeviPauseOLD::LeviPauseOLD(double ci, double ali, double aci) {
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

double LeviPauseOLD::get_Levi_rv() {
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

