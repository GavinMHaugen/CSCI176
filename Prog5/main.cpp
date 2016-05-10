/* Gavin Haugen

In this program we were assigned to implement a parallel version of merge sort with MPI
Parallel merge sort starts with n/commSize keys assigned to each process and it all ends on process 0 in a sorted order.
To achieve this we use the tree structured communication method similar to the global sum example

I'm not sure if its my code or if I dont have the MPI library set up correctly but the program will compile using the cmd:
mpicc -o main main.cpp

however when i try to use:

mpirun -np 2/4/8 ./main problem_size
OR
mpirexec "

it gives me an error like:

gavin@gavin-X502CA:~/Desktop/CSCI176/Prog5$ mpirun -np 2 ./main 20
[gavin-X502CA:29511] [[39798,0],0] mca_oob_tcp_recv_handler: invalid message type: 15
[gavin-X502CA:29511] [[39798,0],0] mca_oob_tcp_recv_handler: invalid message type: 15
--------------------------------------------------------------------------
mpirun noticed that the job aborted, but has no info as to the process
that caused that situation.

and wont give any output at all. 

*/

#include <iostream>
#include "mpi.h" // used for MPI library
#include <algorithm> // used for merge sort
#include <stdlib.h> //used for quick sort

using namespace std;

int* MakeArray(int numProcessors, int n, int rank);
int* ArrMerge(int *ArrA, int *ArrB, int ArrSize);
int Compare(const void *a, const void *b);

int main(int argc, char *argv[]){
	//recieving problem size from the command line
	int n = atoi(argv[1]);

	//pointer for local array
	int *local_Array;

	//variables used by MPI functions
	int Comm_size;
	int Comm_rank;

	//Variables used in Full Tree Reduction process
	bool fin = false;
	int partner;
	int inc = 1;
	int diV = 2;
	int *tempArr;
	int ArrSizeFTR = n/Comm_size;

	//Initializing MPI feature
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &Comm_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &Comm_rank);

	//case for process 0 so it can broadcast to all other processes
	if(Comm_rank == 0){
		for(int p = 1; p < Comm_size; p++){

			//sending the problem size n to process number p
			MPI_Send(&n, 1, MPI_INT, p, 0, MPI_COMM_WORLD);

			//process 0 local array
			local_Array = MakeArray(Comm_size, n, Comm_rank);
			qsort(local_Array, n/Comm_size, sizeof(int), Compare);
		}
	}
	else{

		//recieving the problem size from parent process 0
		MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		//the rest of the processes local arrays
		local_Array = MakeArray(Comm_size, n, Comm_rank);
		qsort(local_Array, n/Comm_size, sizeof(int), Compare);
	}

	//Outputting each processes local array
	cout << "process " << Comm_rank << ": ";

	for(int i = 0; i < n/Comm_size; i++)
			cout << local_Array[i] << " ";
	cout << endl;


	//Full Tree Reduction

	while(Comm_size >= 2 && !fin){
		if(Comm_rank % 2 == 0){

			tempArr = new int[ArrSizeFTR];
			partner = Comm_rank + inc;
			MPI_Recv(tempArr, ArrSizeFTR, MPI_INT, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			local_Array = ArrMerge(tempArr, local_Array, ArrSizeFTR);
			ArrSizeFTR = ArrSizeFTR * 2;
			free(tempArr);
		}
		else{

			partner = Comm_rank - inc;
			MPI_Send(local_Array, ArrSizeFTR, MPI_INT, partner, 0, MPI_COMM_WORLD);
			fin = true;
		}

		diV = diV * 2;
		inc = inc *2;
	}


	//outputting the sorted merged list from process 0
	if(Comm_rank == 0){
		cout << "Sorted list: " << endl;
		for(int i = 0; i < ArrSizeFTR; i++)
			cout << local_Array[i] << " ";
		cout << endl;
	}

	free(local_Array);
	MPI_Finalize();
	return 0;
}

//This function dynamically creates an array with arguments of number of processors, Problem Size, and rank.
//As requested, the array will be filled with numbers that are less than 100 
int* MakeArray(int numProcessors, int n, int rank){
	int i;
	int ArrSize = n / numProcessors;
	int *Arr = new int[ArrSize];

	//initializing the array with random values
	srand(rank + 1);
	for(i = 0; i < ArrSize; i++)
		Arr[i] = rand() % 100;

	return Arr;
}

// This function merges two arrays that are passed to it through the first two arguments into one array of ArraySize*2 elements
//The final array will contain the sorted elements
int* ArrMerge(int *ArrA, int *ArrB, int ArrSize){
	int *temp = new int[ArrSize*2];
	merge(ArrA, ArrA + ArrSize, ArrB, ArrB + ArrSize, temp);
	return temp;
}

//A comparing function used for Qsort function in std library
int Compare(const void *a, const void *b){
	return(*(int*)a- *(int*)b);
}
