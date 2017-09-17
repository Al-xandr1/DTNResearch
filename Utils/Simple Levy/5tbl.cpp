#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

using namespace std;

// ---------------------------------------------------------------------------------------------------------------------
// Random value generation by 5 compact tables method

# define dg(m,k) ((m>>(30-6*k))&63)  /* gets kth digit of m (base 64) */

extern int *P;                             /* Probabilities as an array of 30-bit integers*/
extern int size;                           /* size of P[] */
static int t1, t2, t3, t4, offset, last;   /* limits for table lookups */
unsigned long jxr=182736531;             /* Xorshift RNG */
static short int *AA,*BB,*CC,*DD,*EE;           /* Tables for condensed table-lookup */

void get5tbls()                           /* Creates the 5 tables after array P[size] has been created */
{
    int i, j, k, m, na=0, nb=0, nc=0, nd=0, ne=0;

    /* get table sizes, malloc */
    for(i=0;i<size;i++) {
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

/* Discrete random variable generating function */
int Dran()             /* Uses 5 compact tables */
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
