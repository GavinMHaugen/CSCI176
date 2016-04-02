/////////////////////////////////
/// Java Threads Matrix Multiplication
/// Gavin Haugen
/// Program3
/// use the following commands on the cmd line to run this Program
/// $>javac P3.java 
/// $>java P3
/* 
	For this programming assignment we wrote a java parallel program
	for matrix multiplication. The matrix multiplication is as follows:

	A(L*m) * B(m*n) = C(L*n)

	Unlike my last pthreads program I decided to not use cmd line arguments,
	insted, you manually change the number of threads yourself in the code 
	where i noted
*/
////////////////////////////////

public class P3//main class
{
	//create a shared_object in class global
	static globals glob = new globals();
	//create a shared object in class communication
	static communication comm = new communication();
	public static long start, stop, total;

	public static void main(String args[])
	{	
		/////////number of threads /////////
		glob.number_of_threads = 8;
		////////////////////////////////////
		
		mythread[] thread = new mythread[glob.number_of_threads];
		
		//matrix related variables initialization here
		glob.l = 1001;
		glob.m = 2000;
		glob.n = 3000;

		//Matrix A, B, C memory allocation here
		glob.A = new double[glob.l][glob.m];
		glob.B = new double[glob.m][glob.n];
		glob.C = new double[glob.l][glob.n];

		//matric A, B, and C initialization here
		for(int i = 0; i < glob.l; i++)
			for(int j = 0; j < glob.m; j++)
				glob.A[i][j] = (i + j);

		for(int i = 0; i < glob.m; i++)
			for(int j = 0; j < glob.n; j++)
				glob.B[i][j] = (i + j + 1);

		for(int i = 0; i < glob.l; i++)
			for(int j = 0; j < glob.n; j++)
				glob.C[i][j] = 0;


		System.out.println("L = " + glob.l + ", m = " + glob.m + ", n = " + glob.n);
		//getting our start time
		start = System.currentTimeMillis();
		//creating the threads
		for(int i = 0; i < glob.number_of_threads; i++)
		{
			thread[i] = new mythread(i, glob, comm);
			thread[i].start();
		}
		//joining the threads
		for(int i = 0; i < glob.number_of_threads; i++)
		{
			try{
				thread[i].join();
			}catch(InterruptedException e){}
		}
		System.out.println("Total computation time(sec) = " + (double)(stop - start)/1000);
	} //main
}

class globals
{
	//declaring matrices A B and C
	public double[][] A;
	public double[][] B;
	public double[][] C;

	//declaring the matrix parameters and number of threads
	public int l, m, n;
	public int number_of_threads;
}

//communication class used to increment
class communication
{
	private static int counter = 0;

	//mutex
	public synchronized void increment(int id, int number_of_threads)
	{
		counter++;
		if(id == 0 && counter != number_of_threads)
		{
			try
			{
				wait();
			}

			catch(InterruptedException e)
			{
				System.out.println("Exception " + e.getMessage());
			}
		}

		else
		{
			System.out.println("Done");
			notify();
		}
	}
}

class mythread extends Thread
{
	static globals glob;	//local for shared object
	static communication comm;	//local for shared object
	private int id;
	private int number_of_threads;

	//constructor
	public mythread(int id, globals glob, communication comm) 
	{
		this.id = id;
		this.glob = glob;
		this.comm = comm;
	}

	public void run()
	{
		//here matrix computation
		//start/end index computation based on id
		//initializing and setting our quotient and remainder
		int quotient = glob.l / glob.number_of_threads;
		int remainder = glob.l % glob.number_of_threads;

		//initializing our threadID
		int threadID = id;

		//initializing and setting our indexes for our matrix
		int my_first_i, my_last_i, my_count;

		if(threadID < remainder)
		{
			my_count = quotient + 1;
			my_first_i = threadID * my_count;
		}

		else
		{
			my_count = quotient;
			my_first_i = threadID * my_count + remainder;
		}

		my_last_i = my_first_i + my_count - 1;

		//printing out the indexes of each thread
		System.out.println("Thread " + threadID + ": start row: " + my_first_i + " end row: " + my_last_i);

		//multiplying our matrices
		for(int i = my_first_i; i <= my_last_i; i++)
		{
			for(int j = 0; j < glob.n; j++)
			{
				double sum = 0;
				for(int k = 0; k < glob.m; k++)
				{
					sum += glob.A[i][k] * glob.B[k][j];
				}

				glob.C[i][j] = sum;
			}
		}

		//increment done_counter in shared object communication
		comm.increment(id, glob.number_of_threads);
		//when all threads are done, display reasulting C matric in Thread_1
		if(id == 0)
		{	
			//finds the stop time for total time computation
			P3.stop = System.currentTimeMillis();
			//Print the First 20x10 results
			System.out.println("First 20 x First 10: ");
			for(int i = 0; i < 20; i++)
			{
				for(int j = 0; j < 10; j++)
				{
					//If i didnt convert the double values to a string they were giving me really big negative numbers
					System.out.print(String.format("| %-10s ", Double.toString(glob.C[i][j])));
				}
				System.out.println("|");
			}
			System.out.println();

			//Print the Last 20x10 results
			System.out.println("Last 20 x Last 10: ");
			for(int i = glob.l - 21; i < glob.l - 1; i++)
			{
				for(int j = glob.n - 11; j < glob.n - 1; j++)
				{
					System.out.print(String.format("| %-10s ", Double.toString(glob.C[i][j])));
				}
				System.out.println("|");
			}
			System.out.println();
		}

	}//run

}//class mythread 