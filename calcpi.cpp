#include "calcpi.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// global var
double rsqGlobal;
uint64_t rGlobal;
uint64_t val;
int extra;

void * worker(void *arg){
  // each thread has index (id)
  int index = (long) arg;
  //------------------
  // calc. start and end
  uint64_t start, end;
  end = val * index;
  if(index>extra) end+=index-extra;
  start = end - val;
  if(index>extra) start--;
  //------------------
  uint64_t count = 0, rLocal = rGlobal, rsqLocal = rsqGlobal;

  for(double x = start+1; x<=end; x++)
        for(double y=0; y<=rLocal; y++)
                if(x*x+y*y<=rsqLocal) count++;
  // return count
  uint64_t *ptr =(uint64_t *)malloc(sizeof(uint64_t));
  *ptr = count;
  return ptr;
}

uint64_t count_pixels(int r, int n_threads)
{
        // set global vars
        rGlobal = r;
        rsqGlobal = double(r) * r;
        // val : the amount of number each thread should process
        // extra : which threads should an extra value
        val = r/n_threads;
        extra = n_threads - (r%n_threads);
        // declare array of threads
        pthread_t threads[n_threads];
        for(int i=1; i<=n_threads; i++){
                if(pthread_create(&threads[i-1], NULL, worker, (void *)(long)i) != 0){
                        return -1;
                }
        }
        // join threads and get result
        uint64_t result = 0;
        uint64_t *ptr;
        for(int i=0; i<n_threads; i++){
                pthread_join(threads[i], (void**)&ptr);
                result += *ptr;
                free(ptr);
        }
        return result * 4 + 1;
}
