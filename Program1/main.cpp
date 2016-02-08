#include <iostream>
#include <unistd.h> //for fork, pipe, and wait
#include <cstdlib> //for exit(0)
#include <cstring>
#include <sys/wait.h>
#include <sys/time.h>

using namespace std;

long unsigned int Fibo_i(int n);
long unsigned int Fibo_r(int n);

#define GET_TIME(now)\
{struct timeval t; gettimeofday(&t, NULL);\
 now = t.tv_sec + t.tv_usec/1000000.0;}



int main()
{
    int pid, status; //process id and status initialization
    int p1[2], p2[2], p3[2], p4[2]; //pipe initialization
    int value = 47; //value we will be using for the fibo func. We will have to use 3 diff values
    double start, stop, Fibo_i_total, Fibo_r_total; //values to time the fibp functions

    //initializing all of the pipes we need for this program
    pipe(p1); //pipe 1. will talk to the recursive fibo func (child3)
    pipe(p2); //pipe 2. will talk to the incremental fibo func (child2)
    pipe(p3); //pipe 3. will give info to child1(controller) from recursive fibo func
    pipe(p4); //pipe 4. will give info to child1(controller) from incremental fibo func

    cout << "Fibo(20) = " << Fibo_i(value) << "\n";
    for(int i = 1; i <= 3; i++)
    {
        pid = fork();
        if(pid == 0 && i == 1)
        {
            write(p1[1], &value, sizeof(value));//pipe1 that writes the value to "child2"
            write(p2[1], &value, sizeof(value));//pipe2 that writes the value to "child3"
            read(p3[0], &Fibo_r_total, sizeof(Fibo_r_total));//pipe3 that reads the input
            read(p4[0], &Fibo_i_total, sizeof(Fibo_i_total));//pipe4 that reads the input
            cout << "Iterative Fibo func: " << Fibo_i_total << " secs\n\n";
            cout << "Recursive Fibo func: " << Fibo_r_total << " secs\n\n";
            exit(0);
        }

        else if (pid == 0 && i == 2)
        {
            GET_TIME(start);
            read(p1[0], &value, sizeof(value));
            Fibo_r(value);
            GET_TIME(stop);
            Fibo_r_total = stop - start;
            write(p3[1], &Fibo_r_total, sizeof(Fibo_r_total));
            exit(0);
        }

        else if(pid == 0 && i == 3)
        {
            GET_TIME(start);
            read(p2[0], &value, sizeof(value));
            Fibo_i(value);
            GET_TIME(stop);
            Fibo_i_total = stop - start;
            write(p4[1], &Fibo_i_total, sizeof(Fibo_i_total));
            exit(0);
        }
    }

    for(int i = 1; i <= 3; i++)
    {
        pid = wait(&status);
        cout << "child (pid = " << pid << ") exited status = " << status << "\n" << endl;
    }

    return 0;
}

long unsigned int Fibo_i(int n)
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
    }
     return temp;
}

long unsigned int Fibo_r(int n)
{
    if(n == 1 || n == 2)
        return 1;
    else
        return(Fibo_r(n-1) + Fibo_r(n-2));
}
