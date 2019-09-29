#ifndef LEVISTATIC_H
#define LEVISTATIC_H

#include <omnetpp.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>

// Levi distribution for distance of travel between way points
class LeviJump
{
  static bool isTableReady;

  static double c, alpha;   // Levi distribution parameters
  static double DeltaX;     // step for probability density
  static double deltaT;     // step for characteristic function

  // array for probability density values
  static double *f;

  // random generator tables
  static int *P;
  static int size;    // size P

  static int Join;
  static double *a;
  static double *b;

  static int t1, t2, t3, t4, offset, last;        /* limits for table lookups */
  static unsigned long jxr;                       /* Xorshift RNG */
  static short int *AA,*BB,*CC,*DD,*EE;           /* Tables for condensed table-lookup */

public:
  // method for probability density calculation
  void CalculateDensity();

  // method for random generator tables calculation
  void get5tbls();

  LeviJump(double ci, double ali, double deltaX, int join);

  // methods for r.v. generation
  int Dran();
  double get_Levi_rv();
  double get_Levi_rv(const double maxPermittedValue);
};


// Levi distribution for pause in way points
class LeviPause
{
  static bool isTableReady;

  static double c, alpha;   // Levi distribution parameters
  static double DeltaX;     // step for probability density
  static double deltaT;     // step for characteristic function

  // array for probability density values
  static double *f;

  // random generator tables
  static int *P;
  static int size;    // size P

  static int Join;
  static double *a;
  static double *b;

  static int t1, t2, t3, t4, offset, last;        /* limits for table lookups */
  static unsigned long jxr;                       /* Xorshift RNG */
  static short int *AA,*BB,*CC,*DD,*EE;           /* Tables for condensed table-lookup */

public:
  // method for probability density calculation
  void CalculateDensity();

  // method for random generator tables calculation
  void get5tbls();

  LeviPause(double ci, double ali, double deltaX, int join);

  // methods for r.v. generation
  int Dran();
  double get_Levi_rv();
  double get_Levi_rv(const double maxPermittedValue);
};

#endif
