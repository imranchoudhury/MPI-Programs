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
int n=90;

double time_1, time_2, time_4, time_5, time_7, time_8, time_10, time_11;
double time_3=0.0;
double time_6 = 0.0;
double time_9 = 0.0;
double time_12 = 0.0;
int world_rank,world_size,hypercube_size,hypercube_rank;
MPI_Comm hypercube_3d;

  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

//Initializing the dimensions and periods
int ndims=3;
int processPerDims[3]={2,2,2};
int periods[3]={0,0,0};

time_1 = MPI_Wtime();

//Create the ring topology
MPI_Cart_create(MPI_COMM_WORLD,ndims,processPerDims,periods,0,&hypercube_3d);
time_2 = MPI_Wtime();
MPI_Barrier(hypercube_3d);
time_3 = time_2-time_1;
printf(" the time is %f\n", time_3);

MPI_Comm_rank(hypercube_3d,&hypercube_rank);
MPI_Comm_size(hypercube_3d,&hypercube_size);


  int num_elements_per_proc = n/9;

  // Create a random array of elements on the root process. Its total
  // size will be the number of elements per process times the number
  // of processes
 int *rand_nums =(int*)malloc(sizeof(int) * n);
int k=0;
  if (hypercube_rank == 0) {
   
    rand_nums = create_rand_nums(n);
  }

  // For each process, create a buffer that will hold a subset of the entire
  // array
  int *sub_rand_nums = (int*)malloc(sizeof(int) * num_elements_per_proc);

  // Scatter the random numbers from the root process to all processes in
  // the MPI world

time_4 = MPI_Wtime();
  MPI_Scatter(rand_nums, num_elements_per_proc, MPI_INT, sub_rand_nums,
		  num_elements_per_proc, MPI_INT, 0, hypercube_3d);
 time_5 = MPI_Wtime();
MPI_Barrier(hypercube_3d);
time_6 = time_5 - time_4;

  printf(" the time is %f\n", time_6);
  // Compute the sum  of your subset
  int sub_sum = compute_sum(sub_rand_nums, num_elements_per_proc);
printf("\nI am rank %d and my sum is %d\n",hypercube_rank,sub_sum);
 
  int *sub_sums=NULL;
  if (hypercube_rank == 0) {
 sub_sums = (int*)malloc(sizeof(int) * hypercube_size);
  }

time_7 = MPI_Wtime();
 
  MPI_Gather(&sub_sum, 1, MPI_INT, sub_sums,1, MPI_INT, 0, hypercube_3d);

time_8 = MPI_Wtime();
time_9 = time_8 - time_7;
printf(" the time is %f\n", time_9);
  // we have all of the partial averages on the root, compute the
  // total average of all numbers. Since we are assuming each process computed
  // an average across an equal amount of elements, this computation will
  // produce the correct answer.
 
time_10 = MPI_Wtime();
  if (hypercube_rank == 0) {
    int sum = compute_sum(sub_sums, hypercube_size);
    printf("Sum of all hypercube_3d is %d\n", sum);
  }

time_11 = MPI_Wtime();
time_12 = time_11- time_10;
printf(" the time is %f\n", time_12);
if(hypercube_size==0){
free(rand_nums);
free(sub_sums);
}
free(sub_rand_nums);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
