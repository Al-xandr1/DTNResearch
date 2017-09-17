#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <cstdlib>
#include <math.h>
#include <vector>
#include <windows.h>

using namespace std;

ifstream *distHstFile, *pauseHstFile;

struct HistogramData {
    double minValue, maxValue;
    int SamplesTotal, NumOfBaskets;
    double Delta;
    int *numOfSamples;
    int OutOfBounds;
};

// probability density data
int NumPoint;
double Delta;
int startPoint;
double *xPoint;
double *fPoint;
float *fDat;

// characteristic function data
double tDelta;
double* tP;
double* cfP;
float* rDat;
float* iDat;

void loadHst(char *HstFile, struct HistogramData *hd)
{
    ifstream *fName=new ifstream(HstFile);
    if(fName==NULL) exit(1);
    (*fName)>>(hd->minValue)>>(hd->maxValue)>>(hd->SamplesTotal)>>(hd->NumOfBaskets)>>(hd->Delta)>>(hd->OutOfBounds);
    hd->numOfSamples=new int[hd->NumOfBaskets];
    for(int i=0; i<hd->NumOfBaskets; i++) (*fName)>>hd->numOfSamples[i];
    fName->close();

    cout<<"Histogram data loaded from file "<<HstFile<<endl;
    cout<<"Number of data "<<hd->SamplesTotal<<",\n Minimal value of the data "<<hd->minValue<<",\n Maximal value of the data "<<hd->maxValue<<endl;
    cout<<"Size of one basket "<<hd->Delta<<",\n Number of baskets "<<hd->NumOfBaskets<<",\n Number of out of bound data "<<hd->OutOfBounds<<endl;
}

void DensityEstimation(struct HistogramData *hd)
{
   Delta    = hd->Delta;
   NumPoint = hd->NumOfBaskets;
   xPoint = new double[NumPoint];
   fPoint = new double[NumPoint];

   for(int i=0; i<NumPoint; i++) {
       xPoint[i] = (i + 0.5)*Delta;
       fPoint[i] = hd->numOfSamples[i]/Delta/(hd->SamplesTotal-hd->OutOfBounds);
   }

   startPoint=1;
   while( fPoint[startPoint] == 0) startPoint++;

   // normalization a part of density for fitting
   fDat= new float[NumPoint/2+1];
   double h=0;
   for(int i=0; i<=NumPoint/2; i++) h+=fPoint[i]*Delta;
   for(int i=0; i<=NumPoint/2; i++) fDat[i]=fPoint[i]*Delta/h;

    // data for characteristic function preparation
    tP  = new double[NumPoint];
    cfP = new double[NumPoint];
    rDat= new float[NumPoint];
    iDat= new float[NumPoint];
    // tDelta = 2 * 3.141592653589 / Delta / NumPoint;       // !!!!!!!!!!!!!!
    tDelta = 1.0 / Delta / NumPoint;                         // !!!!!!!!!!!!!!
    for(int i=0; i<NumPoint; i++) {
       tP[i] = i*tDelta;
       cfP[i]= 0;
       rDat[i]=iDat[i]=0;
   }


}

//_________________________________________________________________________________________
//_________________________________________________________________________________________
//
// NAME:          FFT.
// PURPOSE:       Быстрое преобразование Фурье: Комплексный сигнал в комплексный спектр и обратно.
//                В случае действительного сигнала в мнимую часть (Idat) записываются нули.
//                Количество отсчетов - кратно 2**К - т.е. 2, 4, 8, 16, ... (см. комментарии ниже).
//
//
// PARAMETERS:
//
//    float *Rdat    [in, out] - Real part of Input and Output Data (Signal or Spectrum)
//    float *Idat    [in, out] - Imaginary part of Input and Output Data (Signal or Spectrum)
//    int    N       [in]      - Input and Output Data length (Number of samples in arrays)
//    int    LogN    [in]      - Logarithm2(N)
//    int    Ft_Flag [in]      - Ft_Flag = FT_ERR_DIRECT  (i.e. -1) - Direct  FFT  (Signal to Spectrum)
//		                 Ft_Flag = FT_ERR_INVERSE (i.e.  1) - Inverse FFT  (Spectrum to Signal)
//
// RETURN VALUE:  false on parameter error, true on success.
//_________________________________________________________________________________________
//
// NOTE: In this algorithm N and LogN can be only:
//       N    = 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384;
//       LogN = 2, 3,  4,  5,  6,   7,   8,   9,   10,   11,   12,   13,    14;
//_________________________________________________________________________________________
//_________________________________________________________________________________________

#define  NUMBER_IS_2_POW_K(x)   ((!((x)&((x)-1)))&&((x)>1))  // x is pow(2, k), k=1,2, ...
#define  FT_DIRECT        -1    // Direct transform.
#define  FT_INVERSE        1    // Inverse transform.

bool  FFT(float *Rdat, float *Idat, int N, int LogN, int Ft_Flag)
{
  // parameters error check:
  if((Rdat == NULL) || (Idat == NULL))                  return false;
  if((N > 16384) || (N < 1))                            return false;
  if(!NUMBER_IS_2_POW_K(N))                             return false;
  if((LogN < 2) || (LogN > 14))                         return false;
  if((Ft_Flag != FT_DIRECT) && (Ft_Flag != FT_INVERSE)) return false;

  register int  i, j, n, k, io, ie, in, nn;
  float         ru, iu, rtp, itp, rtq, itq, rw, iw, sr;

  static const float Rcoef[14] =
  {  -1.0000000000000000F,  0.0000000000000000F,  0.7071067811865475F,
      0.9238795325112867F,  0.9807852804032304F,  0.9951847266721969F,
      0.9987954562051724F,  0.9996988186962042F,  0.9999247018391445F,
      0.9999811752826011F,  0.9999952938095761F,  0.9999988234517018F,
      0.9999997058628822F,  0.9999999264657178F
  };
  static const float Icoef[14] =
  {   0.0000000000000000F, -1.0000000000000000F, -0.7071067811865474F,
     -0.3826834323650897F, -0.1950903220161282F, -0.0980171403295606F,
     -0.0490676743274180F, -0.0245412285229122F, -0.0122715382857199F,
     -0.0061358846491544F, -0.0030679567629659F, -0.0015339801862847F,
     -0.0007669903187427F, -0.0003834951875714F
  };

  nn = N >> 1;
  ie = N;
  for(n=1; n<=LogN; n++)
  {
    rw = Rcoef[LogN - n];
    iw = Icoef[LogN - n];
    if(Ft_Flag == FT_INVERSE) iw = -iw;
    in = ie >> 1;
    ru = 1.0F;
    iu = 0.0F;
    for(j=0; j<in; j++)
    {
      for(i=j; i<N; i+=ie)
      {
        io       = i + in;
        rtp      = Rdat[i]  + Rdat[io];
        itp      = Idat[i]  + Idat[io];
        rtq      = Rdat[i]  - Rdat[io];
        itq      = Idat[i]  - Idat[io];
        Rdat[io] = rtq * ru - itq * iu;
        Idat[io] = itq * ru + rtq * iu;
        Rdat[i]  = rtp;
        Idat[i]  = itp;
      }

      sr = ru;
      ru = ru * rw - iu * iw;
      iu = iu * rw + sr * iw;
    }

    ie >>= 1;
  }

  for(j=i=1; i<N; i++)
  {
    if(i < j)
    {
      io       = i - 1;
      in       = j - 1;
      rtp      = Rdat[in];
      itp      = Idat[in];
      Rdat[in] = Rdat[io];
      Idat[in] = Idat[io];
      Rdat[io] = rtp;
      Idat[io] = itp;
    }

    k = nn;

    while(k < j)
    {
      j   = j - k;
      k >>= 1;
    }

    j = j + k;
  }

  if(Ft_Flag == FT_DIRECT) return true;

  rw = 1.0F / N;

  for(i=0; i<N; i++)
  {
    Rdat[i] *= rw;
    Idat[i] *= rw;
  }

  return true;
}

// ----------------------------------------------------------------------------------------------------------------------------

double pi = 3.141592653589;

double squareDeviation(double c, double alpha)
{
     // numerical data already in global variables
     double h=0,hh=0,hhh;
     cfP[0]=1; rDat[0]=1;
     for(int i=1; i<=NumPoint/2; i++) {
        cfP[i]=exp(-pow(2*pi*c*tP[i], alpha));
        rDat[NumPoint-i]=rDat[i]=cfP[i];
     }
     FFT(rDat, iDat, NumPoint, (int)log2(NumPoint), FT_INVERSE);

     for(int i=startPoint; i<=NumPoint/2; i++) hh+=rDat[i];
     for(int i=startPoint; i<=NumPoint/2; i++) {
        hhh=fDat[i]-rDat[i]/hh;
        h+=hhh*hhh;
     }
     // cout<<"c="<<c<<"\t alpha="<<alpha<<"\t dev="<<h<< endl;
     return h;
}

//-----------------------------------------------------------------------------------------------------------------------------

/* Two variable function minimization by Nelder and Mead downhill simplex method.
   On the basis of the algorithm realization from "Numerical Recipes in C: The Art of scientific Computing".
   In the book the algorithm presented for many dimensions. This version is for two variable function only,
   and therefore greatly simplified.
   Privalov A.Yu. (r) 2011
*/

#define TINY 1.0e-10  /* A small number. */
#define NMAX 5000
#define SWAP(i,j) {(swap)=F[i];F[i]=F[j];F[j]=(swap);(swap)=X[i];X[i]=X[j];X[j]=(swap);(swap)=Y[i];Y[i]=Y[j];Y[j]=(swap);}

double amotry2d(double (*func)(double, double), double F[3], double X[3], double Y[3], double c);

double amoeba2D(double (*func)(double, double), double F[3],double X[3], double Y[3], double ftol, int *nfunk )
/* Two dimensional minimization of the function func(x,y) by the downhill simplex method of Nelder and Mead.
The vectors X[3],Y[3] are input. They are x and y coordinates of a vertices of the starting simplex.
Also input is the vector F[3], whose components must be preinitialized to the values of func evaluated at the vertices;
and ftol the fractional convergence tolerance to be achieved in the function value (n.b.!).
On output, X,Y and F will have been reset to 3 new points all within ftol of a minimum
function value (F[0] is the best), and nfunk gives the number of function evaluations taken. */
{
double swap, rtol, Ftry, Fsave;

*nfunk=0;
while(1) {
   /* sorting to rearrange best (F[0]), next to worst (F[1]) and worst (F[2]) simplex points */
   if( F[0]>F[1] ) SWAP(0,1)
   if( F[1]>F[2] ) SWAP(1,2)
   if( F[0]>F[1] ) SWAP(0,1)
   /* Compute the fractional range from highest to lowest and return if satisfactory (return also if too many iterations made). */
   rtol=2.0*fabs(F[2]-F[0])/(fabs(F[2])+fabs(F[0])+TINY);
   if (rtol < ftol || *nfunk >= NMAX) return rtol;

   /* Begin a new iteration. First extrapolate by a factor -1 through the face of the simplex across from the high point,
   i.e., reflect the simplex from the high point. */
   Ftry = amotry2d(func,F,X,Y, -1);  (*nfunk)++;
   /* Gives a result better than the best point, so try an additional extrapolation by a factor 2. */
   if (Ftry <= F[0]) { Ftry = amotry2d(func,F,X,Y, 2);  (*nfunk)++; }
   else if (Ftry >= F[1]) {
      /* The reflected point is worse than the second-highest, so look for an intermediate lower point,
      i.e., do a one-dimensional contraction. */
      Fsave= F[2];
      Ftry = amotry2d(func,F,X,Y, 0.5);  (*nfunk)++;
      if (Ftry >= Fsave) {
         /* Can’t seem to get rid of that high point. Better contract around the lowest (best) point. */
         F[1]=(*func)( X[1]=0.5*(X[0]+X[1]), Y[1]=0.5*(Y[0]+Y[1]) );
         F[2]=(*func)( X[2]=0.5*(X[0]+X[2]), Y[2]=0.5*(Y[0]+Y[2]) );
         *nfunk+=2;
         }
      }
   }
return rtol;     /* never get here */
}


double amotry2d(double (*func)(double, double), double F[3], double X[3], double Y[3], double c)
/* Extrapolates by a factor fac through the face of the simplex across from the high point, tries it,
   and replaces the high point if the new point is better. */
{
double Ftry, Xtry, Ytry;

/* Evaluate the function at the trial point. */
Xtry=(1-c)*0.5*(X[0]+X[1]) + c*X[2];
Ytry=(1-c)*0.5*(Y[0]+Y[1]) + c*Y[2];
Ftry=(*func)(Xtry,Ytry);
/* If it’s better than the worst, then replace the worst. */
if( Ftry<F[2] ) {F[2]=Ftry; X[2]=Xtry; Y[2]=Ytry;}
return Ftry;
}

//-----------------------------------------------------------------------------------------------------------------------------

void TimeDistanceRegression(char* TDFile, double* k, double* ro)
{
    int n=0;
    double t,d,x=0,x2=0,y=0,xy=0;
    ifstream *fName=new ifstream(TDFile);
    if(fName==NULL) exit(1);
    while(!fName->eof()) {
        (*fName)>>d>>t;
        n++;

        x+=log(d); x2+=log(d)*log(d);
        y+=log(t); xy+=log(t)*log(d);
    }
    *ro = 1-(xy/n - x/n*y/n)/(x2/n - x/n*x/n);
    *k=exp( y/n - (1 - *ro) * x/n );
    fName->close();
    return;
}





// ------------------------------------------------------------------------------------------------------------------------------
char* getHstFileName(char* HstFileName)
{
    return strcat(HstFileName,".hst");
}

char* buildFullName(char* buffer, char* dir, char* fileName)
{
    strcpy(buffer, dir);
    strcat(buffer, "/");
    return strcat(buffer, fileName);
}

int main(int argc, char** argv)
{
    char* histogramFilesDir; // full path name of directory
    histogramFilesDir = "./histogramfiles";

    char* f1Name;
    f1Name="pauseTime.hst";

    char f1FullName[256];
    buildFullName(f1FullName, histogramFilesDir, f1Name);

    struct HistogramData h1;
    loadHst(f1FullName, &h1);
    DensityEstimation(&h1);

    double x[3]={20,  21,  20};
    double y[3]={0.5, 0.5, 0.6};
    double f[3], tol;
    int count=0;

    f[0] = squareDeviation(x[0], y[0]);
    f[1] = squareDeviation(x[1], y[1]);
    f[2] = squareDeviation(x[2], y[2]);

    tol=amoeba2D( squareDeviation, f, x, y, 1.e-8, &count );
    cout<<"minimal deviation="<<f[0]<<" at point c="<<x[0]<<" alpha="<<y[0]<<" (tol="<<tol<<" count=<"<<count<<")\n\n";

    char* f2Name;
    f2Name="distances.hst";

    char f2FullName[256];
    buildFullName(f2FullName, histogramFilesDir, f2Name);

    struct HistogramData h2;
    loadHst(f2FullName, &h2);
    DensityEstimation(&h2);

    double x2[3]={1, 2, 1};
    double y2[3]={1, 1, 1.1};
    double f2[3], tol2;
    int count2=0;

    f2[0] = squareDeviation(x2[0], y2[0]);     // cout<<"initial deviation"<<f2[0]<<endl;
    f2[1] = squareDeviation(x2[1], y2[1]);
    f2[2] = squareDeviation(x2[2], y2[2]);

    tol2=amoeba2D( squareDeviation, f2, x2, y2, 1.e-8, &count2 );
    cout<<"minimal deviation="<<f2[0]<<" at point c="<<x2[0]<<" alpha="<<y2[0]<<" (tol="<<tol2<<" count=<"<<count2<<")\n\n";

    char* f3Name;
    f3Name="speed.dat";

    char f3FullName[256];
    buildFullName(f3FullName, histogramFilesDir, f3Name);

    double k, ro;
    TimeDistanceRegression(f3FullName, &k, &ro);
    cout<<"k="<<k<<"\t ro="<<ro<<endl;
}
