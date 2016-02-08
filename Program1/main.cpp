/*Gavin Haugen
CSCI176 Parallel Processing
2/6/16

This program compares the efficiencies of two implementations of the Fibonacci number generation functions. We are testing these
two functions in parallel instead of doing serial testing*/

#include <iostream>
#include <unistd.h> //for fork, pipe, and wait
#include <cstdlib> //for exit(0)
#include <sys/wait.h> //for wait()
#include <sys/time.h> //for timing the fibo functions

using namespace std;

long unsigned int Fibo_i(int n); //iterative fibonacci function
long unsigned int Fibo_r(int n); //recursive fibonaccit function

//our get time macro given to us in class
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

    cout << "Fibo(47) = " << Fibo_i(value) << "\n";
    for(int i = 1; i <= 3; i++)
    {
        pid = fork();
        if(pid == 0 && i == 1)
        {
            write(p1[1], &value, sizeof(value));//pipe1 that writes the value to "child2"
            write(p2[1], &value, sizeof(value));//pipe2 that writes the value to "child3"
            read(p3[0], &Fibo_r_total, sizeof(Fibo_r_total));//pipe3 that reads the input
            read(p4[0], &Fibo_i_total, sizeof(Fibo_i_total));//pipe4 that reads the input
            //outputting times for each fibonacci function
            cout << "Iterative Fibo func: " << Fibo_i_total << " secs\n\n";
            cout << "Recursive Fibo func: " << Fibo_r_total << " secs\n\n";
            exit(0);
        }
        //child(2) handles recursive fibonacci function calculation and timing
        else if (pid == 0 && i == 2)
        {
            GET_TIME(start); //starts the timer
            read(p1[0], &value, sizeof(value)); //reads value from controller(child1)
            Fibo_r(value); //calls recursive fibo function
            GET_TIME(stop); //stops the timer
            Fibo_r_total = stop - start; //finds total computation time
            write(p3[1], &Fibo_r_total, sizeof(Fibo_r_total)); //writes the total computation time back to the controller
            exit(0);
        }
        //child(3) handles iterative fionacci function calculation and timing
        else if(pid == 0 && i == 3)
        {
            GET_TIME(start);//starts the timwer
            read(p2[0], &value, sizeof(value));//reads the value from controller(child1)
            Fibo_i(value); //calls iterative fibo function
            GET_TIME(stop);// stops the timer
            Fibo_i_total = stop - start; //finds total computation time
            write(p4[1], &Fibo_i_total, sizeof(Fibo_i_total)); //writes the total computation time back to the controller
            exit(0);
        }
    }

    //waits 3 times for each of the child processes
    for(int i = 1; i <= 3; i++)
    {
        pid = wait(&status);
        cout << "child (pid = " << pid << ") exited status = " << status << "\n" << endl;
    }

    return 0;
}
//iterative fibo function
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
//recursive fibo function
long unsigned int Fibo_r(int n)
{
    if(n == 1 || n == 2)
        return 1;
    else
        return(Fibo_r(n-1) + Fibo_r(n-2));
}
