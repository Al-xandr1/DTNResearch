#include "LeviStatic.h"

using namespace std;

// FFT ----------------------------------------------------------------------------------------------
#define  FT_DIRECT        -1    // Direct transform.
#define  FT_INVERSE        1    // Inverse transform.

double deltaT;

bool  FFT(float *Rdat, float *Idat, int N, int LogN, int Ft_Flag);

#define pi (3.141592653589)
#define N  16384
#define LN 14

// for random generator -----------------------------------------------------------------------------------

# define dg(m,k) ((m>>(30-6*k))&63)  /* gets kth digit of m (base 64) */


// static variables of class ------------------------------------------------------------------------------
bool LeviJump::isTableReady = false;

double  LeviJump::c      = 1;
double  LeviJump::alpha  = 1;
double  LeviJump::DeltaX = 1;
double  LeviJump::deltaT = 2*pi/N;
int     LeviJump::Join   = 1;

double* LeviJump::f;
int*    LeviJump::P;
int     LeviJump::size;

int     LeviJump::t1, LeviJump::t2, LeviJump::t3, LeviJump::t4, LeviJump::offset, LeviJump::last;    /* limits for table lookups */
unsigned long LeviJump::jxr=182736531;                                                               /* Xorshift RNG */
short int    *LeviJump::AA, *LeviJump::BB, *LeviJump::CC, *LeviJump::DD, *LeviJump::EE;              /* Tables for condensed table-lookup */

double *LeviJump::a;
double *LeviJump::b;

void LeviJump::CalculateDensity()
{
    float Re[N];
    float Im[N];

    Re[0]=1; Im[0]=0;
    for(int i=1; i<=N/2; i++) { Re[i]=Re[N-i]=exp( -pow(c*i*deltaT, alpha)); Im[i]=Im[N-i]=0; }

    FFT(Re, Im, N, LN, FT_INVERSE);

    f[0]=Re[0]/DeltaX;
    for(int i=1; i<=N/2; i++) f[i]=2*Re[i]/DeltaX;
}


void LeviJump::get5tbls()
{
    int i, j, k, m, na=0, nb=0, nc=0, nd=0, ne=0;

    /* get table sizes and memory allocation */
    for(i=0; i<size; i++) {
         m=P[i];
         na+=dg(m,1);
         nb+=dg(m,2);
         nc+=dg(m,3);
         nd+=dg(m,4);
         ne+=dg(m,5);
    }
    AA=new short int[na];   // (short int*)malloc(na*sizeof(int));
    BB=new short int[nb];   // (short int*)malloc(nb*sizeof(int));
    CC=new short int[nc];   // (short int*)malloc(nc*sizeof(int));
    DD=new short int[nd];   // (short int*)malloc(nd*sizeof(int));
    EE=new short int[ne];   // (short int*)malloc(ne*sizeof(int));

    if( AA==NULL || BB==NULL || CC==NULL || DD==NULL || EE==NULL) exit(1);

    cout<<" Table sizes:"<<na<<" "<<nb<<" "<<nc<<" "<<nd<<" "<<ne<<" total="<<na+nb+nc+nd+ne<<endl;

     t1=na<<24;
     t2=t1+(nb<<18);
     t3=t2+(nc<<12);
     t4=t3+(nd<<6);

     na=nb=nc=nd=ne=0;

     /* Fill tables AA,BB,CC,DD,EE */
     for(i=0;i<size;i++) {
         m=P[i];
         k=i+offset;
         for(j=0;j<dg(m,1);j++) AA[na+j]=k;
         na+=dg(m,1);
         for(j=0;j<dg(m,2);j++) BB[nb+j]=k;
         nb+=dg(m,2);
         for(j=0;j<dg(m,3);j++) CC[nc+j]=k;
         nc+=dg(m,3);
         for(j=0;j<dg(m,4);j++) DD[nd+j]=k;
         nd+=dg(m,4);
         for(j=0;j<dg(m,5);j++) EE[ne+j]=k;
         ne+=dg(m,5);
     }

} // end get5tbls


LeviJump::LeviJump(double ci, double ali, double deltaX, int join)
{
    if (!isTableReady) {

        c      = ci;
        alpha  = ali;
        DeltaX = deltaX;
        Join   = join;
        if( (N/2+1)%Join == 1 ) size = (N/2+1)/Join;
        else { cout<<"LevyJump constructor: incorrect argument"; exit(1);}

        deltaT = 2*pi/DeltaX/N;
        f = new double[N/2+1];
        CalculateDensity();

        P = new int[size];
        double h=f[0];
        for(int i=1; i<=N/2; i++) {
            if( i%join == 0 ) { P[i/join-1] = (h*deltaX)*(1<<30); h = f[i]; }
            else h += f[i];
        }

        get5tbls();

        // create table for linear approximation of density
        a = new double[size];
        b = new double[size];
        double S,f1,f2;
        for(int i=0; i<size-1; i++) {
            S  = 0.5*( f[i*Join] + f[(i+1)*Join] ) * DeltaX * Join;
            f1 = f[i*Join]/S;
            f2 = f[(i+1)*Join]/S;
            a[i]  =(f2-f1)/DeltaX/Join;
            b[i]  = f1;
        }
        isTableReady = true;
    }
}


/* Discrete random variable generator */
int LeviJump::Dran()   /* Uses 5 compact tables */
{
    unsigned long j;
    jxr^=jxr<<13;
    jxr^=jxr>>17;
    jxr^=jxr<<5;
    j=(jxr>>2);

    if(j<t1) return AA[j>>24];
    if(j<t2) return BB[(j-t1)>>18];
    if(j<t3) return CC[(j-t2)>>12];
    if(j<t4) return DD[(j-t3)>>6];
    return EE[j-t4];
}


double LeviJump::get_Levi_rv() {
    int i;
    double x,y;

    i=Dran();

    y=1.0*jxr/4294967295;

    if(a[i]<0)      x = i*Join*DeltaX - b[i]/a[i] - sqrt(2*y/a[i] + b[i]*b[i]/(a[i]*a[i]));
    else if(a[i]>0) x = i*Join*DeltaX - b[i]/a[i] + sqrt(2*y/a[i] + b[i]*b[i]/(a[i]*a[i]));
    else            x = i*Join*DeltaX + y*Join*DeltaX;

    return x;
}


double LeviJump::get_Levi_rv(double maxPermittedValue) {
    ASSERT(maxPermittedValue > 0);
    double result = get_Levi_rv();
    while (result > maxPermittedValue) {
        cout << "WARNING!!! Value is too long: LeviJump::get_Levi_rv = " << result << endl;
        result = get_Levi_rv();
    }
    return result;
}



// =========================================================================



// static variables of class ------------------------------------------------------------------------------
bool LeviPause::isTableReady = false;

double  LeviPause::c      = 1;
double  LeviPause::alpha  = 1;
double  LeviPause::DeltaX = 1;
double  LeviPause::deltaT = 2*pi/N;
int     LeviPause::Join   = 1;

double* LeviPause::f;
int*    LeviPause::P;
int     LeviPause::size;

int     LeviPause::t1, LeviPause::t2, LeviPause::t3, LeviPause::t4, LeviPause::offset, LeviPause::last;    /* limits for table lookups */
unsigned long LeviPause::jxr=182736531;                                                                    /* Xorshift RNG */
short int    *LeviPause::AA, *LeviPause::BB, *LeviPause::CC, *LeviPause::DD, *LeviPause::EE;               /* Tables for condensed table-lookup */

double *LeviPause::a;
double *LeviPause::b;


void LeviPause::CalculateDensity()
{
    float Re[N];
    float Im[N];

    Re[0]=1; Im[0]=0;
    for(int i=1; i<=N/2; i++) { Re[i]=Re[N-i]=exp( -pow(c*i*deltaT, alpha)); Im[i]=Im[N-i]=0; }

    FFT(Re, Im, N, LN, FT_INVERSE);

    f[0]=Re[0]/DeltaX;
    for(int i=1; i<=N/2; i++) f[i]=2*Re[i]/DeltaX;
}


void LeviPause::get5tbls()
{
    int i, j, k, m, na=0, nb=0, nc=0, nd=0, ne=0;

    /* get table sizes and memory allocation */
    for(i=0; i<size; i++) {
         m=P[i];
         na+=dg(m,1);
         nb+=dg(m,2);
         nc+=dg(m,3);
         nd+=dg(m,4);
         ne+=dg(m,5);
    }
    AA=new short int[na];   // (short int*)malloc(na*sizeof(int));
    BB=new short int[nb];   // (short int*)malloc(nb*sizeof(int));
    CC=new short int[nc];   // (short int*)malloc(nc*sizeof(int));
    DD=new short int[nd];   // (short int*)malloc(nd*sizeof(int));
    EE=new short int[ne];   // (short int*)malloc(ne*sizeof(int));

    if( AA==NULL || BB==NULL || CC==NULL || DD==NULL || EE==NULL) exit(1);

    cout<<" Table sizes:"<<na<<" "<<nb<<" "<<nc<<" "<<nd<<" "<<ne<<" total="<<na+nb+nc+nd+ne<<endl;

     t1=na<<24;
     t2=t1+(nb<<18);
     t3=t2+(nc<<12);
     t4=t3+(nd<<6);

     na=nb=nc=nd=ne=0;

     /* Fill tables AA,BB,CC,DD,EE */
     for(i=0;i<size;i++) {
         m=P[i];
         k=i+offset;
         for(j=0;j<dg(m,1);j++) AA[na+j]=k;
         na+=dg(m,1);
         for(j=0;j<dg(m,2);j++) BB[nb+j]=k;
         nb+=dg(m,2);
         for(j=0;j<dg(m,3);j++) CC[nc+j]=k;
         nc+=dg(m,3);
         for(j=0;j<dg(m,4);j++) DD[nd+j]=k;
         nd+=dg(m,4);
         for(j=0;j<dg(m,5);j++) EE[ne+j]=k;
         ne+=dg(m,5);
     }

} // end get5tbls


LeviPause::LeviPause(double ci, double ali, double deltaX, int join)
{
    if (!isTableReady) {

        c      = ci;
        alpha  = ali;
        DeltaX = deltaX;
        Join   = join;
        if( (N/2+1)%Join == 1 ) size = (N/2+1)/Join;
        else { cout<<"LevyJump constructor: incorrect argument"; exit(1);}

        deltaT = 2*pi/DeltaX/N;
        f = new double[N/2+1];
        CalculateDensity();

        P = new int[size];
        double h=f[0];
        for(int i=1; i<=N/2; i++) {
            if( i%join == 0 ) { P[i/join-1] = (h*deltaX)*(1<<30); h = f[i]; }
            else h += f[i];
        }

        get5tbls();

        // create table for linear approximation of density
        a = new double[size];
        b = new double[size];
        double S,f1,f2;
        for(int i=0; i<size-1; i++) {
            S  = 0.5*( f[i*Join] + f[(i+1)*Join] ) * DeltaX * Join;
            f1 = f[i*Join]/S;
            f2 = f[(i+1)*Join]/S;
            a[i]  =(f2-f1)/DeltaX/Join;
            b[i]  = f1;
        }
        isTableReady = true;
    }
}


/* Discrete random variable generator */
int LeviPause::Dran()   /* Uses 5 compact tables */
{
    unsigned long j;
    jxr^=jxr<<13;
    jxr^=jxr>>17;
    jxr^=jxr<<5;
    j=(jxr>>2);

    if(j<t1) return AA[j>>24];
    if(j<t2) return BB[(j-t1)>>18];
    if(j<t3) return CC[(j-t2)>>12];
    if(j<t4) return DD[(j-t3)>>6];
    return EE[j-t4];
}


double LeviPause::get_Levi_rv() {
    int i;
    double x,y;

    i=Dran();

    y=1.0*jxr/4294967295;

    if(a[i]<0)      x = i*Join*DeltaX - b[i]/a[i] - sqrt(2*y/a[i] + b[i]*b[i]/(a[i]*a[i]));
    else if(a[i]>0) x = i*Join*DeltaX - b[i]/a[i] + sqrt(2*y/a[i] + b[i]*b[i]/(a[i]*a[i]));
    else            x = i*Join*DeltaX + y*Join*DeltaX;

    return x;
}


double LeviPause::get_Levi_rv(double maxPermittedValue) {
    ASSERT(maxPermittedValue > 0);
    double result = get_Levi_rv();
    while (result > maxPermittedValue) {
        cout << "WARNING!!! Value is too long: LeviPause::get_Levi_rv = " << result << endl;
        result = get_Levi_rv();
    }
    return result;
}
