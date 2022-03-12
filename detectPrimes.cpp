#include "detectPrimes.h"
#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <cmath>
#include <atomic>

// global variables
pthread_barrier_t barrier;
std::vector<int64_t> result;
// a copy of nums
std::vector<int64_t> global_nums;
// global_finished:stop the infinite loop
// notify: if it is true that mean do not do calculations
bool global_finished, notify;
// temp : stores the size of nums
// add : to control adding the values into result
int global_counter, extra, temp, add;
// n : number form the vector
// val : the amount of number each thread should process
// th : number of threads global
int64_t n, val, th;
// atomic for thread cancellation
std::atomic<int> stop (0);


void * worker(void *arg){
  // each thread has an index (id)
  int index = (long) arg, res;
  int64_t start, end, local_n, r;
  while(1){
    // SERIAL
    res = pthread_barrier_wait(&barrier);
    if( res == PTHREAD_BARRIER_SERIAL_THREAD){
      if(add!=0){
         // stop = 0 that means there wan no devisor
         // else means do not add n and set stop to 0
         if(stop==0) result.push_back(local_n);
         else stop=0;
         add = 0;
      }
      // if we reach the end of the vector then stop
      if(global_counter >= temp) global_finished = true;
      else{
        // get a number and set notify false
        n = global_nums[global_counter++];
        // notify true means do not do the calculations
        notify = false;

        if (n<2) notify = true;
        else if (n <= 3 || n==5){
          result.push_back(n);
          notify = true;
        }else if((n%2==0)||(n%3==0)) notify = true;

        if(notify == false){
          // r : number of numbers between 1 and sqrt(n)
          r = round(sqrt(n)/6);
          if(r==0){
            // r = 0 no need to check for
            notify = true;
            if(n%5 != 0) result.push_back(n);
          }else{
            // divide r into number of threads
            val = floor(r/th);
            // extra means : which threads should an extra value
            extra = th - (r%th);
          }
    }}}
    pthread_barrier_wait(&barrier);
    // PARALLEL
    if(global_finished) break;
    else if(!notify){
      local_n = n;
      // calc the end for each thread
      end = val * index;
      if(index>extra) end+=index-extra;
      // calc start for each thread
      start = end - val;
      if(index>extra) start--;
      // make start and end be a multible of 6 and sub 1
      // the -1 because we start from 5 not 6 
      start = (6*(start+1))-1;
      end = (6*end)-1;
      while(start<=end){
        if(stop!=0) break; // thread cancellation
        else if (local_n % start == 0) stop++;
        else if (local_n % (start + 2) == 0) stop++;
        start += 6;
      }
    // set add to 1 to add N into result (look at add above::if statements::)
     add=1; 
    }}
  return NULL;
}

std::vector<int64_t>
detect_primes(const std::vector<int64_t> & nums, int n_threads)
{
  // set global vars.
  global_nums = nums;
  global_finished = false;
  global_counter = 0;
  th = n_threads;
  temp = global_nums.size();
  // threads array and barrier
  pthread_t threads[n_threads];
  pthread_barrier_init(&barrier, NULL, n_threads);
  for(int i=1; i<=n_threads; i++){
    if(pthread_create(&threads[i-1], NULL, worker, (void *)(long)i)!=0){
      return result;
    }
  }
  // wait until all threads finish
  for(int i=0; i<n_threads; i++){
    pthread_join(threads[i], 0);
  }
  return result;
}
