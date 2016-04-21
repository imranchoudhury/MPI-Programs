#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <assert.h>
  
// Creates an array of random numbers. Each number has a value from 0 - 1
float *create_rand_nums(int num_elements) {
  float *rand_nums = (float *)malloc(sizeof(float) * num_elements);
  assert(rand_nums != NULL);
  int i;
  for (i = 0; i < num_elements; i++) {
    rand_nums[i] = (rand() / (float)RAND_MAX);
  }
  return rand_nums;
}

// Computes the sum of an array of numbers
float compute_sum(float *array, int num_elements) {
  float sum = 0.f;
  int i;
  for (i = 0; i < num_elements; i++) {
    sum += array[i];
  }
  return sum;
}

int main(int argc, char** argv) {

int dims[1];periods[1];
int npes,world_rank,world_size,ring_size,ring_rank;
MPI_Comm ring_1d;

  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);

//Intializing the dimentions and periods
dims[0]=npes;
periods[0]=1;

//Create the ring topology
MPI_Cart_Create(MPI_Comm comm,1,dims,periods,1,&ring_1d);



MPI_Comm_rank(ring_1d,&ring_rank);
MPI_Comm_size(ring_1d,&ring_size);

printf("the ranks are %d\n", ring_rank);
printf("the size is %d\n", ring_size);

}