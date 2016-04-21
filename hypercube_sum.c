#include<mpi.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<math.h>

void *generate_random(int n);
int sum(int *array, int n);

int main(int argc, char *argv[])
{
	int n;// The total number of elements to be stored.
	int npes; // The total number of processes.
	int myrank; //The rank of the calling processes.
	int nlocal; // Local number of elements.
	int *elmnts; //The array that stores local number of elements.
	int *relmnts; // The array that stores received elements.
	int i;
	int sourcenode=0;
	MPI_Status status;
	MPI_Comm hyper_assign;
	
	/* Initialising MPI and getting system Information */
	
	unsigned int iseed = (unsigned int)time(NULL);
	 // Initialising the random number generator.
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &npes);
	MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
	
	n=90; // Taking number of elements input from command line.
	nlocal=n/npes; // Computing number of elements that will be send to each process.
	
	// Creating array of random numbers on root process.
	
	int *random_nums=NULL;
	if(myrank==sourcenode)
	{
		random_nums=(int *)malloc(nlocal * npes);
	}
	
	/* Creating sub buffer to hold subset of array on each processor.*/
	
	int *local_random_nums= (int *)malloc(sizeof(int) * nlocal);
	
	// Setting up the Cartesian Topology
	

	int ndim, reorder;
  ndim= 3;
  int dims[3], periods[3];
  for (i=0; i<3; i++){
    dims[i] = 2;
    periods[i] = 1;
  }
  reorder = 0;
	/*int dimension=3;
	int reorder;
	int dims[3], periods[3];
	dims[0]= npes;
	periods[0]=1; // Setting the periods for Wrap around Connections.
	reorder=0;*/
	
	MPI_Cart_create(MPI_COMM_WORLD, dimension, dims, periods, reorder, &hyper_assign);
	
	// Getting Rank and size with respect to new Topology.
	
	int ringrank;
	MPI_Comm_rank(hyper_assign, &ringrank);
	int ringsize;
	MPI_Comm_size(hyper_assign, &ringsize);
	
	/* Scattering the Random numbers from root process to all processes.*/
	
	MPI_Scatter(random_nums, nlocal, MPI_INT, local_random_nums, MPI_INT, nlocal, 0, hyper_assign);
	
	// Computing sum of the individual sub array to do the sum. Please create separate function for this at end.
	
	int sub_sum=sum(local_random_nums , nlocal);
	
	
	// Gathering all partial sum to Root process.
	
	int *sub_sums=NULL;
	if(ringrank==sourcenode)
	{
		sub_sums=(int *)malloc(sizeof(int ) * ringsize);
	}
	
	MPI_Gather(&sub_sum, 1, MPI_INT,sub_sums, 1, MPI_INT, 0, hyper_assign );
	
	// Computer total sum of all partial sum on root.
	
	if(ringrank==sourcenode)
	{
		int totsum= sum(sub_sums, ringsize);
		printf("sum of all elements is %d\n", totsum);
		
		// Computing the sum across original data.
		
		int original_sum=sum(random_nums, nlocal * ringsize);
		printf("sum computed across orginal data is %d\n", original_sum);
	}
	
	//cleanup
	
	if(ringsize==sourcenode)
	{
		free(random_nums);
		free(sub_sums);
	}
	
	free(local_random_nums);
	
	MPI_Barrier(hyper_assign);
	MPI_Finalize();
	
	
	
}

void *generate_random(int n)
{
	int *random_nums= (int *)malloc(n * sizeof(int));
	int i=0;
	
	while(i<n)
	{
		random_nums[i]=random();
		i++;
	}
	
	return random_nums;
	
}

int sum(int *array, int n)
{
	int sum=0;
	int i;
	for(i=0;i<n;i++)
	{
		sum=sum+array[i];
	}
	
	return sum;
}	