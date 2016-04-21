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


int world_rank,world_size,hypercube_size,hypercube_rank;
MPI_Comm hypercube_3d;

  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

//Initializing the dimensions and periods
int ndims=3;
int processPerDims[3]={3,3,3};
int periods[3]={1,1,1}

//Create the ring topology
MPI_Cart_create(MPI_COMM_WORLD,ndims,dims,periods,1,&hypercube_3d);



MPI_Comm_rank(ring_1d,&hypercube_rank);
MPI_Comm_size(ring_1d,&hypercube_size);

printf("the ranks are %d\n", hypercube_rank);
printf("the size is %d\n", hypercube_size);

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
  MPI_Scatter(rand_nums, num_elements_per_proc, MPI_INT, sub_rand_nums,
		  num_elements_per_proc, MPI_INT, 0, hypercube_3d);
 for(k=0;k<num_elements_per_proc;k++)
{
        printf("\n My rank is %d and value is: %d",hypercube_rank,sub_rand_nums[k]);
}

  
  // Compute the sum  of your subset
  int sub_sum = compute_sum(sub_rand_nums, num_elements_per_proc);
printf("\nI am rank %d and my sum is %d\n",hypercube_rank,sub_sum);
 
  int *sub_sums=NULL;
  if (hypercube_rank == 0) {
 sub_sums = (int*)malloc(sizeof(int) * hypercube_size);
  }
 
  MPI_Gather(&sub_sum, 1, MPI_INT, sub_sums,1, MPI_INT, 0, hypercube_3d);

int j;
for(j=0;j<hypercube_size;j++){
printf("I am after gather sum %d\n",sub_sums[j]);
}
 
  // p
  // p
  // p
  // p
  // p
  // Now thiae are the random values: %d",rand_nums[k]);
  // }
  // we have all of the partial averages on the root, compute the
  // total average of all numbers. Since we are assuming each process computed
  // an average across an equal amount of elements, this computation will
  // produce the correct answer.
 

  if (hypercube_rank == 0) {
    int sum = compute_sum(sub_sums, hypercube_size);
    printf("Sum of all hypercube_3d is %d\n", sum);
  }
if(hypercube_size==0){
free(rand_nums);
free(sub_sums);
}
free(sub_rand_nums);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}
