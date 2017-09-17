#include "LeviStatic.h"
#include <iostream>
#include <fstream>

using namespace std;

int main()
{
    LeviJump test(2.23069, 0.326218, 1.0, 4);

    LeviPause test2(30.0126, 1.09653, 1.0, 4);

    for(int i=0; i<1000; i++) { cout<<test.get_Levi_rv()<<"\t"<<test2.get_Levi_rv()<<endl; }
}
