#include <stdio.h>
#include <iostream>
#include <cmath>
#include<vector>
#include <numeric>
#include "matrix.h"
#include <stdint.h>
#include <gmp.h>
#include <gmpxx.h>
#include"CycleTimer.h"
using namespace std;

int main(int argc,char** argv){
    string num = argv[1];
    mpz_class n (num);
    int factorBase[] = {2,3,5,7};
    size_t size = sizeof(factorBase) / sizeof(factorBase[0]);
    vector<uint32_t> smooth;
    vector<vector<uint32_t>> smoothFactors;
    const mpz_class sqrtN = sqrt(n);
    uint32_t z = 0;
    uint32_t interval = 1000;
    // cout<<"z = "<<z<<endl;
    double start = CycleTimer::currentSeconds();
    while(smooth.size()<size+20){
        uint32_t a;
        uint32_t i;
        for(i=0,a = z;i<interval;i++,a++){
            // test if b-smooth
            mpz_class z_square = ((a+sqrtN)*(a+sqrtN))%n; 
            vector<uint32_t> factors;
            for(uint32_t j=0;j<size;j++){
                const int p = factorBase[j];
                while(mpz_divisible_ui_p(z_square.get_mpz_t(), p)){
                    mpz_divexact_ui(z_square.get_mpz_t(), z_square.get_mpz_t(), p);
                    factors.push_back(j);
                }
            }
            //check whether z_sqaure is b_smooth
            if(z_square ==1){
                smooth.push_back(a); //need change
                smoothFactors.push_back(factors);
            }
            if(smooth.size()>size+20){ 
                // for(auto s:smooth){
                //     cout<<s<<endl;
                // }
                break;
            }
        }
        z+= interval;
    }
    // cout<<"matrix construction : "<<size<<" "<<smoothFactors.size()+1<<endl;
    Matrix M(size,smoothFactors.size()+1);
    for(int i=0;i<smoothFactors.size();i++){
        // cout<<"smooth factors : "<<smoothFactors[i].size()<<endl;
        for(int j=0;j<smoothFactors[i].size();j++){
            M(smoothFactors[i][j],i).flip();//
        }
    }
    // cout<<M<<endl;
    M.reduce();
    // cout<<"reduce!\n";
    // cout<<M<<endl;
    //guassian elimination over !
    mpz_class a;
    mpz_class b;
    do {
        vector<uint32_t> x = M.solve();
        // for(auto ss:x){
        //     cout<<ss<<" ";
        // }
        // cout<<endl;
        a = 1;
        b = 1;
        vector<uint32_t> decomp(size,0);
        for(uint32_t i=0;i<smoothFactors.size();i++){
            if(x[i]==1){
                for(uint32_t p=0;p<smoothFactors[i].size();p++) ++decomp[smoothFactors[i][p]];
                b *= (smooth[i]+sqrtN);
            }
        }
        for(uint32_t p=0;p<size;++p){
            for(uint32_t i=0;i<(decomp[p]/2);++i){
                a *= factorBase[p];
            }
        }
    }while(a % n == b % n || a % n == (- b) % n + n);
    double conduct_time = CycleTimer::currentSeconds()-start;
    mpz_class factor = gcd(b-a,n);
    cout<<factor<<" "<<n/factor<<" "<<factor*(n/factor)<<endl;
    printf("time : %f \n",conduct_time);

}

