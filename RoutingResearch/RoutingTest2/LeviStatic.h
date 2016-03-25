// Levi distribution

#ifndef LEVISTATIC_H
#define LEVISTATIC_H

#include <omnetpp.h>
#include <math.h>

class LeviJump
{
public:
  static bool isTableReady;

  static double c, alpha;   // Levi distribution parameters
  static double accuracy;   // accuracy for numerical calculation
  static double xinv[100], Finv[100];
  static int imax; 
  static double Fmax;
  static double step;  static double lim;  // numerical integraton parameters

  // methods for distribution function calculation
  double integralF(double x);
  double distFun(double x);

  // constructor with inverse function calculations
  LeviJump(double ci, double ali, double aci);

  // methods for r.v. generation
  double get_Levi_rv();
};


class LeviPause
{
public:
  static bool isTableReady;

  static double c, alpha;   // Levi distribution parameters
  static double accuracy;   // accuracy for numerical calculation
  static double xinv[100], Finv[100];
  static int imax; 
  static double Fmax;
  static double step;  static double lim;  // numerical integraton parameters

  // methods for distribution function calculation
  double integralF(double x);
  double distFun(double x);

  // constructor with inverse function calculations
  LeviPause(double ci, double ali, double aci);

  // methods for r.v. generation
  double get_Levi_rv();
};

#endif
