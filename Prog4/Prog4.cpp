/* 
Gavin Haugen
Program 4
4/17/16

For this programming assignment we implemented count sort in 3 different ways
the first way was a serial version of the algorithm
the second way was a parallelized version of the algorithm that parallelized the outermost for loop (var i)
the third way was another parallelized version but it parallelized the outer for loop and the last for loop that over wrote the temp variables

We tested each algorithm on 3 seperate Arrays: A, B, and C
The program initializes each array with random values no bigger than given variable N(array size)
Then array A is sorted by the serial version
Array B is sorted by the first parallel version
Array C is sorted by the second parallel version

Compilation Instr:

g++ Prog4.cpp -o main
./main Num_of_Threads N

where N is the desired size of the Arrays.
*/

#include <iostream>
#include <omp.h> //OpenMP library
#include <sys/time.h> //used for time macro
#include <stdlib.h> //used for random #

using namespace std;

//GET_TIME macro
#define GET_TIME(now)\
{ struct timeval t; gettimeofday(&t, NULL);\
  now = t.tv_sec + t.tv_usec/1000000.0; }

//global declarations for command line args
int Num_of_Threads;
int N;

void serial_count_sort(int Arr[], int n);
void parallel_count_sort1(int Arr[], int n);
void parallel_count_sort2(int Arr[], int n);
int* Initialize_Array(int n, int* EleValues);
int* Array_Values(int n);
bool Is_Sorted(int Arr[], int n);
void Print_Arr(int Arr[], int n, int domain);

int main(int argc, char* argv[])
{
	//receiving command line arguments
	Num_of_Threads = atoi(argv[1]);
	N = atoi(argv[2]);

	//variables used to track time of the sorts
	double start, stop;

	//Initizlizing the random 
	int *values = Array_Values(N);

	//Initizlizing the 3 required Arrays
	int *A = Initialize_Array(N, values);
	int *B = Initialize_Array(N, values);
	int *C = Initialize_Array(N, values);
	free(values);


	//Outputting results for our First Array A that handles our Serial Count Sort
	cout << "Serial Count Sort: \n";
	cout << "Unsorted Array A:\n";
	Print_Arr(A, N, 20);
	cout << endl;
	cout << "Sorting Array A..\n\n\n";
	GET_TIME(start);
	serial_count_sort(A, N);
	GET_TIME(stop);
	cout << "Sorted Array A: \n";
	Print_Arr(A, N, 20);
	cout << endl;
	cout << "Sorting Array A took: " << stop - start << " seconds\n";
	if(Is_Sorted(A, N))
		cout << "Array A was sorted correctly\n";
	else
		cout << "Array A was not sorted correctly\n";
	cout << "Done with Serial Count Sort\n\n\n\n";

	//Outputting results for our Second Array B that handles our Parallel Count Sort 1
	cout << "Parallel Count Sort 1: \n";
	cout << "Unsorted Array B:\n";
	Print_Arr(B, N, 20);
	cout << endl;
	cout << "Sorting Array B...\n\n\n";
	GET_TIME(start);
	parallel_count_sort1(B, N);
	GET_TIME(stop);
	cout << "Sorted Array B: \n";
	Print_Arr(B, N, 20);
	cout << endl;
	cout << "Sorting Array B took: " << stop - start << " seconds\n";
	if(Is_Sorted(A, N))
		cout << "Array B was sorted correctly\n";
	else
		cout << "Array B was not sorted correctly\n";
	cout << "Done with Parallel Count Sort 1\n\n\n";

	//Outputting results for our Third Array C that handles our Parallel Count Sort 2
	cout << "Parallel Count Sort 2: \n";
	cout << "Unsorted Array C:\n";
	Print_Arr(C, N, 20);
	cout << endl;
	cout << "Sorting Array C...\n\n\n";
	GET_TIME(start);
	parallel_count_sort1(C, N);
	GET_TIME(stop);
	cout << "Sorted Array C: \n";
	Print_Arr(C, N, 20);
	cout << endl;
	cout << "Sorting Array C took: " << stop - start << " seconds\n";
	if(Is_Sorted(A, N))
		cout << "Array C was sorted correctly\n";
	else
		cout << "Array C was not sorted correctly\n";
	cout << "Done with Parallel Count Sort 2\n\n\n";

	return 0;
}

//This is our serial version of the count sort, this version is basically taken straight from the book example except
//I changed the memcpy part to a for loop like suggested in the prompt.
void serial_count_sort(int Arr[], int n)
{
	int i, j, count;
	int *temp = new int[n];

	for(i = 0; i < n; i++)
	{
		count = 0;
		for(j = 0; j < n; j++)
		{
			if(Arr[j] < Arr[i])
				count++;
			else if(Arr[j] == Arr[i] && j < i)
				count++;
		}
		temp[count] = Arr[i];
	}

	for(i = 0; i < n; i++){
		Arr[i] = temp[i];
	}

	free(temp);
}

//This is our first parallel version of the count sort, it is essentially the same as the serial one aboce except
//in this version we have parallized the first for loop with variable i
void parallel_count_sort1(int Arr[], int n)
{
	int i, j, k, count;
	int *temp = new int[n];

	# pragma omp parallel for num_threads(Num_of_Threads) \
			default(none) shared(Arr, n, temp) private(i, j, count)

	for(i = 0; i < n; i++)
	{
		count = 0;
		for(j = 0; j < n; j++)
		{
			if(Arr[j] < Arr[i])
				count++;
			else if(Arr[j] == Arr[i] && j < i)
				count++;
		}
		temp[count] = Arr[i];
	}

	for(k = 0; k < n; k++)
		Arr[k] = temp[k];

	free(temp);
}

//This is our second parallel version of the count sort, again it is essentially the same as the parallel one above it except
// not only is our first for loop with variable i parallelized but so is our for loop with variable k that overwrites the temp array
void parallel_count_sort2(int Arr[], int n)
{
	int i, j, k, count;
	int *temp = new int[n];

	# pragma omp parallel for num_threads(Num_of_Threads) \
			default(none) shared(Arr, n, temp) private(i, j, count)

	for(i = 0; i < n; i++)
	{
		count = 0;
		for(j = 0; j < n; j++)
		{
			if(Arr[j] < Arr[i])
				count++;
			else if(Arr[j] == Arr[i] && j < i)
				count++;
		}
		temp[count] = Arr[i];
	}

	# pragma omp parallel for num_threads(Num_of_Threads) \
			default(none) shared(Arr, n, temp) private(k)

	for(k = 0; k < n; k++)
		Arr[k] = temp[k];

	free(temp);
}

//Function in intialize our 3 arrays we need for the program
//We initialize their values with values taken from the Array_Values function
int* Initialize_Array(int n, int* EleValues)
{
	int *Arr = new int[n];
	for(int i = 0; i < n; i++)
	{
		Arr[i] = EleValues[i];
	}

	return Arr;
}

//giving an arbitrary array random numbers no bigger than N which is our Array size
//We will be using this function to fill our actual arrays with random numbers
int* Array_Values(int n)
{
	int *Arr = new int[n];
	for(int i = 0; i < n; i++)
	{
		Arr[i] = rand() % n;
	}

	return Arr;
}

//checks to see if the arrays are sorted correctly
bool Is_Sorted(int Arr[], int n)
{
	for(int i = 0; i < n; i++)
	{
		if(Arr[i] > Arr[i+1] && Arr[i] != Arr[i+1] && (i+1) < n)
			return false;
	}

	return true;
}

//prints out the specified array with a given range of first X and last X
void Print_Arr(int Arr[], int n, int domain)
{
	int i;
	cout << "First " << domain << ": \n";
	for(i = 0; i < domain; i++)
	{
		cout << Arr[i] << " ";
	}
	cout << "\n\n";

	cout << "Last " << domain << ": \n";
	for(i = n - domain; i < n; i++)
	{
		cout << Arr[i] << " ";
	}
	cout << "\n";
}