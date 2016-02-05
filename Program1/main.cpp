#include <iostream>
#include <unistd.h> //for fork, pipe, and wait
#include <cstdlib> //for exit(0)
#include <cstring>

using namespace std;

double Fibo_i(double n);
double Fibo_r(double n);

#define GET_TIME(now)\
{struct timeval t; gettimeofday(&t, NULL);\
 now = t.tv_sec + t.tv_usec/1000000.0;}



int main()
{


    return 0;
}

double Fibo_i(double n)
{
    int p1 = 1, p2 = 1, temp;

    if(n == 1 || n == 2)
        return 1;

    else
    {
        for(int i = 3; i <= n; ++i)
        {
            temp = p1 + p2;
            p1 = p2;
            p2 = temp;
        }
        return temp;
    }
}

double Fibo_r(double n)
{
    if(n == 1 || n == 2)
        return 1;
    else
        return(Fibo_r(n-1) + Fibo_r(n-2));
}
