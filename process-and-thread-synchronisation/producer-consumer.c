// Producer consumer model implemented using POSIX threads and condtional variables.

// All writes to the stdout are done through `write()` syscall.
// Not using printf here as printf is buffered, ignoring the performance penalty for directly using write().
// Compile the below program and checkout the console output to understand how producer and consumer interact with eachother.

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
// Define the maximum size of the buffer
#define MAX 5
// Mutex for mutual exclusion
pthread_mutex_t mutex;

// Pthread conditional variables
pthread_cond_t condc, condp;

// Common buffer used by consumers and producers.
int buffer[MAX];
int CurrentBufferCapacity;

void add_item_to_buffer();
void print_and_clear_buffer();

// Producer
void *producer(){
    while(1){
        pthread_mutex_lock(&mutex);
        if(CurrentBufferCapacity == MAX){
            // Signal the consumer that the buffer is full.
            pthread_cond_signal(&condc);
            write(1, "I'm the producer... I'm going to sleep as the buffer is full! Bye\n", 66);
            // Wait for someone to signal that the buffer is empty.
            pthread_cond_wait(&condp, &mutex);
            write(1, "Hey producer here, someone just woke me up... Time to get to work, gotta produce stuff!\n", 88);
            write(1, "---------------------------------------------------------- \n", 60);               
        }
        // Critical region 
        add_item_to_buffer();
        // Exit critical region
        pthread_mutex_unlock(&mutex);
    }
}

// Consumer
void *consumer(){
    while(1){
        pthread_mutex_lock(&mutex);
        while(!(CurrentBufferCapacity == MAX)){
            // Wait for someone to signal that the condition is met.
            pthread_cond_wait(&condc, &mutex);
        }
        // Critical region
        
        // Actually do something with the data from the buffer, ie consume the data.
        print_and_clear_buffer();
        // Exit critical region
        
        pthread_mutex_unlock(&mutex);

        // Wake up the producer
        if (CurrentBufferCapacity == 0){
            // Signal the producer to wakeup
            pthread_cond_signal(&condp);
        }
    }
}

void add_item_to_buffer(){
    if(CurrentBufferCapacity != MAX){
        buffer[CurrentBufferCapacity] = rand();
        CurrentBufferCapacity++;
    }
}

void print_and_clear_buffer(){
    write(1, "Consumer consuming the buffer and flushing it..\n", 48);
    int i;
    for(i=0; i < MAX;++i){
        write(1, "Consuming \n", 11);
    }
    // Mark the buffer as empty
    CurrentBufferCapacity = 0;
}

int main(){
    // Initialize threads, mutex and conditional variables
    pthread_t pro, con;
    pthread_mutex_init(&mutex, 0);
    pthread_cond_init(&condc, 0);
    pthread_cond_init(&condp, 0);
    // Create the threads
    pthread_create(&con, 0, consumer, 0);
    pthread_create(&pro, 0, producer, 0);

    // Wait for the child threads to finish
    pthread_join(pro, 0);
    pthread_join(con, 0);
    
    // Cleanup the resources
    pthread_cond_destroy(&condc);
    pthread_cond_destroy(&condp);
    pthread_mutex_destroy(&mutex);
    return 0;
}

