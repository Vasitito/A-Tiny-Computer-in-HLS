#include<iostream>
#include <iostream>
#include "ac_int.h"
#include "wave_sort.h"
#include "mc_scverify.h"
#include <cstdlib>
#include <ctime>


CCS_MAIN(int argc ,char* argv[]){

 srand (time(NULL));
    int a[n];
    for (int i=0;i<n;i++){
        a[i] = 0;
        a[i] = rand() % 99 + 1;
        std::cout << a[i] <<"  ";
    }
    std::cout << "\n";
    wave_sort (a);
    for (int i=0;i<n;i++){
        std::cout << a[i] <<"  ";
    }
    return 0;

return 0;
}
