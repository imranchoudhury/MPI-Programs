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

  int num_elements_per_proc = atoi(argv[1]);
  // Seed the random number generator to get different results each time
  srand(time(NULL));

  MPI_Init(NULL, NULL);

  // Create a random array of elements on the root process. Its total
  // size will be the number of elements per process times the number
  // of processes
  float *rand_nums = NULL;
  if (ring_rank == 0) {
    rand_nums = create_rand_nums(num_elements_per_proc * ring_size);
  }

  // For each process, create a buffer that will hold a subset of the entire
  // array
  float *sub_rand_nums = (float *)malloc(sizeof(float) * num_elements_per_proc);
  assert(sub_rand_nums != NULL);

  // Scatter the random numbers from the root process to all processes in
  // the MPI world
  MPI_Scatter(rand_nums, num_elements_per_proc, MPI_FLOAT, sub_rand_nums,
              num_elements_per_proc, MPI_FLOAT, 0, ring_1d);

  // Compute the average of your subset
  float sub_sum = compute_sum(sub_rand_nums, num_elements_per_proc);
 
  // Gather all partial averages down to the root process
  float *sub_sum = NULL;
  
  if (ring_rank == 0) {
    sub_sum = (float *)malloc(sizeof(float) * ring_size);
    assert(sub_sum != NULL);
  }

  MPI_Gather(&sub_sum, 1, MPI_FLOAT, sub_sum, 1, MPI_FLOAT, 0, ring_1d);

  // Now that we have all of the partial averages on the root, compute the
  // total average of all numbers. Since we are assuming each process computed
  // an average across an equal amount of elements, this computation will
  // produce the correct answer.
  
  if (ring_rank == 0) {
    float sum = compute_sum(sub_sum, ring_size);
    printf("Ring of all elements is %f\n", sum);
    // Compute the average across the original data for comparison
    float original_data_sum = compute_sum(rand_nums, num_elements_per_proc * ring_size);
    printf("Avg computed across original data is %f\n", original_data_sum);
  }

  // Clean up
  if (ring_rank == 0) {
    free(rand_nums);
    free(sub_sum);
  }
  free(sub_rand_nums);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
}