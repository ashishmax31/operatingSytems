// Priority Readers and Writers
// Write a multi-threaded C program that gives readers priority over writers concerning a shared (global) variable. 
// Essentially, if any readers are waiting, then they have priority over
// writer threads -- writers can only write when there are no readers. 
// This program should adhere to the following constraints:

// Multiple readers/writers must be supported (5 of each is fine)
// Readers must read the shared variable X number of times
// Writers must write the shared variable X number of times
// Readers must print:
// The value read
// The number of readers present when value is read
// Writers must print:
// The written value
// The number of readers present were when value is written (should be 0)
// Before a reader/writer attempts to access the shared variable it should wait some random amount of time
// Note: This will help ensure that reads and writes do not occur all at once
// Use pthreads, mutexes, and condition variables to synchronize access to the shared variable


#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Define max number of readers,writers and maximum read and write count.
#define MAX_READERS 5
#define MAX_WRITERS 5
#define MAX_READ_WRITE_COUNT 5

// Mutex for mutual exclusion
pthread_mutex_t mutex;

int resource_counter;
int readers_waiting;
int writers_waiting;
int num_readers;
int reader_args[5];
int writer_args[5];
// Pthread conditional variables
pthread_cond_t condr, condw;


// Shared global variable used by readers and writers.
int shared_variable;

// Reader
void *reader(void *args){
    int thread_id = *((int*)args);
    for(int i = 0 ; i<MAX_READ_WRITE_COUNT ; ++i){
        // Wait for a random amount of time.
        usleep(1000 * (random() % MAX_READERS + MAX_WRITERS));
        pthread_mutex_lock(&mutex);
        ++readers_waiting;
        while(resource_counter == -1){
            pthread_cond_wait(&condr, &mutex);
        }
        --readers_waiting;
        ++num_readers;
        ++resource_counter;
        pthread_mutex_unlock(&mutex);
        fprintf(stdout,"[R%d] Reading from shared variable: %d [%d threads currently reading it]\n",thread_id+1,shared_variable,num_readers);

        pthread_mutex_lock(&mutex);
        --num_readers;
        --resource_counter;
        pthread_mutex_unlock(&mutex);
        if(resource_counter == 0){
            pthread_cond_signal(&condw);
        }
    }
    pthread_exit(0);
}



// Writer
void *writer(void *args){
    int thread_id = *((int *)args);
    int i;
    for(i = 0; i < MAX_WRITERS; ++i){
        // Wait for a random amount of time.
        usleep(1000 * (random() % MAX_READERS + MAX_WRITERS));
        pthread_mutex_lock(&mutex);
        ++writers_waiting;
        while(resource_counter != 0){
            pthread_cond_wait(&condw, &mutex);
        }
        --writers_waiting;
        resource_counter = -1;
        pthread_mutex_unlock(&mutex);
        ++shared_variable;
        fprintf(stdout,"[W%d] Writing to the shared  variable the value: %d [%d Readers present]\n",thread_id+1, shared_variable, num_readers);
        pthread_mutex_lock(&mutex);
        --writers_waiting;
        resource_counter = 0;
        if(readers_waiting > 0){
            pthread_cond_broadcast(&condr);
        }
        else{
            pthread_cond_signal(&condw);
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(0);
}


int main(){
    // Initialize threads, mutex and conditional variables
    pthread_t readers[MAX_READERS],writers[MAX_WRITERS];
    pthread_mutex_init(&mutex, 0);
    pthread_cond_init(&condr, 0);
    pthread_cond_init(&condw, 0);
    
    srandom((unsigned int)time(NULL));

    // Create the threads
    for (int i = 0; i < 5; ++i){
        reader_args[i] = i;
        pthread_create(&readers[i], NULL, reader, &reader_args[i]);
    }

    for (int i = 0; i < 5; ++i){
        writer_args[i] = i;
        pthread_create(&writers[i], NULL, writer, &writer_args[i]);
    }

    // Wait for the child threads to finish
    for (int i = 0; i < 5; ++i){
        pthread_join(readers[i], 0);
        pthread_join(writers[i], 0);
    }
        // Cleanup the resources
        pthread_cond_destroy(&condr);
        pthread_cond_destroy(&condw);
        pthread_mutex_destroy(&mutex);
        return 0;
}

