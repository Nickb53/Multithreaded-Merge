/**
*     Mutlithreaded version of MergeSort. 
*     Author: Nicholas Brown
**/

#include "merge.h"
#include <pthread.h>
#include <stdio.h>

#define MAX_THREADS 4 // For creating threads later on

// Struct to pass to pthread_create
typedef struct array_struct
{
  int left;
  int right;
  int *arr;
} array_struct;

/* LEFT index and RIGHT index of the sub-array of ARR[] to be sorted */
void singleThreadedMergeSort(int arr[], int left, int right) 
{
  if (left < right) {
    int middle = (left+right)/2;
    singleThreadedMergeSort(arr, left, middle); 
    singleThreadedMergeSort(arr, middle+1, right); 
    merge(arr, left, middle, right); 
  } 
}

// first helper method for pthread_create
// simply sorts the given array using singleThreadedMergeSort
void* helper_sort(void * param)
{
  struct array_struct *arr_ptr = (struct array_struct*) param;
  singleThreadedMergeSort(arr_ptr->arr, arr_ptr->left, arr_ptr->right); 
  return NULL;
}

// second helper method for pthread_create
// this method is responsible for sorting the second half of the array
// it creates an aditional thread, and then awaits its completion before returning
void* helper_sort2(void * param)
{
  struct array_struct *arr_ptr = (struct array_struct*) param;

  int right = arr_ptr->right;
  int left = arr_ptr->left;
  int middle = (right + left) / 2;

  pthread_t threads[MAX_THREADS];

  array_struct t3 = { .left = left, .right = middle, .arr = arr_ptr->arr};
  pthread_create(&threads[2], NULL, helper_sort, &t3);

  singleThreadedMergeSort(arr_ptr->arr, middle + 1, right);
  pthread_join(threads[2], NULL);

  merge(arr_ptr->arr, left, middle , right); 
  return NULL;
}

/* 
 * This function stub needs to be completed
 */
void multiThreadedMergeSort(int arr[], int left, int right) 
{
  // Your code goes here
 
  // Setting up indecies 
  int leftright, middle;
  leftright = left + ((right - left) / 4);
  middle = (right + left)/2;

  // Create array structs to pass in thread creation
  array_struct t1 = { .left = leftright + 1, .right = middle, .arr = arr};
  array_struct t2 = { .left = middle + 1, .right = right, .arr = arr};

  pthread_t threads[MAX_THREADS];

  // Create threads 1 and 2
  // Thread 3 will be created within thread 2
  pthread_create(&threads[0], NULL, helper_sort, &t1);
  pthread_create(&threads[1], NULL, helper_sort2, &t2);

  // Parent process sorts the first quarter
  singleThreadedMergeSort(arr, left, leftright);

  // Wait until thread 1 is done with the second quarter
  // Then merge the first half
  pthread_join(threads[0], NULL);
  merge(arr, left, leftright, middle);
  
  // Meanwhile.. 
  // Thread 2 and 3 are busy sorting the second half (see second helper function)
  // Once they are done, merge the final array
  pthread_join(threads[1], NULL);
  merge(arr, left, middle, right); 
}
