//Gavin Haugen
//Program 2
//3/1/16

/*
For this programming assignment we wrote a pthreads parallel program
for matrix multiplication. The matrix multiplication is as follows:

A(L*m) * B(m*n) = C(L*n)

we access the command line args for the parameters of this program so compilation should look like:
g++ main.cpp -o main -lpthread
./main L m n num_threads

*/

#include <iostream>
#include <pthread.h> //for using pthreads
#include <sys/time.h> //for the time macro
#include <cstdlib>
#include <iomanip>

using namespace std;

//GET_TIME macro
#define GET_TIME(now)\
{ struct timeval t; gettimeofday(&t, NULL);\
  now = t.tv_sec + t.tv_usec/1000000.0; }

//global values that all threads can reach and have access to
//first we will initialize the dimensions for our matrix
int L;
int m;
int n;

//num of threads we are creating
int num_of_threads;

//Declaring our matrices
double **A;
double **B;
double **C;

//used for storing the quotient
int quotient;
//used for storing the remainder
int remainder;

//declaring our helper functions for this assignment
void print_matrix(int row, int col);
void *multiply(void *threadID);


int main(int argc, char* argv[])
{
    long threadID; //used to store the threadID
    double start, stop, total; //used to check time of program
    pthread_t Threads[num_of_threads];


    //gathering the arguments from the command line
    L = atoi(argv[1]);
    m = atoi(argv[2]);
    n = atoi(argv[3]);
    num_of_threads = atoi(argv[4]);

    //dynamically initializing matrix A
    A = new double *[L];
    for(int i = 0; i < L; i++)
        A[i] = new double[m];
    //adding values to matrix C
    for(int i = 0; i < L; i++)
        for(int j = 0; j < m; j++)
            A[i][j] = i + j;

    //dynamically initializing matrix B
    B = new double *[m];
    for(int i = 0; i < m; i++)
        B[i] = new double [n];
    //adding values to matrix B
    for(int i = 0; i < m; i++)
        for(int j = 0; j < n; j++)
            B[i][j] = i + j + 1;

    //dynamically initializing matrix C
    C = new double *[L];
    for(int i = 0; i < L; i++)
        C[i] = new double [n];

    //calculating the quotient
    quotient = L / num_of_threads;
    //calculating the remainder
    remainder = L % num_of_threads;

    //displaying the arguments given
    cout << "L = " << L << ", m = " << m << ", n = " << n << endl;

    //Start time
    GET_TIME(start);

    //start threads
    for(threadID = 0; threadID < num_of_threads; threadID++)
        pthread_create(&Threads[threadID], NULL, multiply, (void*)threadID);
    //joining threads
    for(threadID = 0; threadID < num_of_threads; threadID++)
        pthread_join(Threads[threadID], NULL);
    //stop time
    GET_TIME(stop);

    //printing the results
    cout << "======First 20 x First 10 for matrix C======\n";
    print_matrix(0,0);
    cout << "\n\n";

    cout << "======Last 20 x Last 10 for Matrix C======\n";
    print_matrix(981,1990);

    cout << "\n\n\n";

    total = stop - start;
    cout << "Total time taken to complete multiplication: " << total << " seconds \n\n";

    return 0;
}

//This function will act as our "slave" function
//It multiplys the two matrices A and B using the equation stated above in the global doc
void *multiply(void *threadID)
{
    //initializing the variables we will track what each thread works on in the matrix
    int MyFirst_i;
    int MyLast_i;

    //setting our argument (threadID) to variable MyRank  so we can perform operations on it
    long MyRank = (long)threadID;

    //took this part from our hw1 solutions so that it deals with
    //setting our first and last parameters for each thread to work on the matrix
    if(MyRank < remainder)
    {
        MyFirst_i = MyRank * (quotient+1);
        MyLast_i = MyFirst_i + quotient;
    }

    else
    {
        MyFirst_i = MyRank * quotient + remainder;
        MyLast_i = MyFirst_i + (quotient-1);
    }

    //requested output from hw prompt
    cout << "Thread_" << MyRank << ": start_row= " << MyFirst_i <<", end_row= " << MyLast_i << "\n";

    //multiplying the matrices together
    for(int i = MyFirst_i; i <= MyLast_i; i++)
        for(int j = 0; j < n; j++)
            for(int k = 0;k < m; k++)
                C[i][j] += A[i][k] * B[k][j];

    return NULL;
}

//function that prints out what is stored in the matrix
void print_matrix(int row, int col)
{
    for(int i = row; i < row+20; i++)
        for(int j = col; j < col+10; j++)
            cout << C[i][j] << "\n";

}
