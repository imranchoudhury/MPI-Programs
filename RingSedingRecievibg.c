#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
  
// Creates an array of random numbers. Each number has a value from 0 - 1
int *create_rand_nums(int num_elements) {
  int *rand_nums =(int*) malloc(sizeof(int) * num_elements);
  int i;
  for (i = 0; i < num_elements; i++) {
    rand_nums[i] = rand()%100 ;
    printf("\narray values : %d",rand_nums[i]);
  }
  return rand_nums;
}

// Computes the sum of an array of numbers
int compute_sum(int *array, int num_elements) {
  int sum = 0;
  int i;
  for (i = 0; i < num_elements; i++) {
    sum += array[i];
 printf("My sum is %d\n",sum);
  }
  return sum;
}

int main(int argc, char** argv) {
 MPI_Init(NULL, NULL);
int n=16;

int dims[1],periods[1];
int world_rank,world_size,ring_size,ring_rank;
MPI_Comm ring_1d;

  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

//Initializing the dimensions and periods
dims[0]=world_size;
periods[0]=1;

//Create the ring topology
MPI_Cart_create(MPI_COMM_WORLD,1,dims,periods,1,&ring_1d);



MPI_Comm_rank(ring_1d,&ring_rank);
MPI_Comm_size(ring_1d,&ring_size);

printf("the ranks are %d\n", ring_rank);
printf("the size is %d\n", ring_size);

  int num_elements_per_proc = n/world_size;

  

  // Create a random array of elements on the root process. Its total
  // size will be the number of elements per process times the number
  // of processes
 int *rand_nums =(int*)malloc(sizeof(int) * n);
int k=0;
  if (ring_rank == 0) {
   
    rand_nums = create_rand_nums(n);
  }

  // For each process, create a buffer that will hold a subset of the entire
  // array
  int *sub_rand_nums = (int*)malloc(sizeof(int) * num_elements_per_proc);

  // Scatter the random numbers from the root process to all processes in
  // the MPI world
  MPI_Scatter(rand_nums, num_elements_per_proc, MPI_INT, sub_rand_nums,
		  num_elements_per_proc, MPI_INT, 0, ring_1d);
 for(k=0;k<num_elements_per_proc;k++)
{
        printf("\n My rank is %d and value is: %d",ring_rank,sub_rand_nums[k]);
}

  
  // Compute the sum  of your subset
  int sub_sum = compute_sum(sub_rand_nums, num_elements_per_proc);
printf("\nI am rank %d and my sum is %d\n",ring_rank,sub_sum);
 
  int *sub_sums=NULL;
  
 sub_sums = (int*)malloc(sizeof(int) * ring_size);
  
 

int i;
 for(i=1;i<ring_size;i++) {
  if (ring_rank == 1) {
        MPI_Send( &sub_sum, 1, MPI_INT,i+1, 0, ring_1d );
  }
  else {
      MPI_Recv( &sub_sums, 1, MPI_INT, i-1, 0, ring_1d,MPI_STATUS_IGNORE);
       sub_sum+=sub_sums;
      if (i <= 0) 
    MPI_Send( &sub_sum, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD );
  }
  printf( "Process %d got %d\n",ring_rank, sub_sum);
    }
   
 

  if (ring_rank == 0) {
    
    printf("Ring of all elements is %d\n", sub_sum);
  }


  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
