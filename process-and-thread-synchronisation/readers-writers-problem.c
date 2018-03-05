// Readers and writers problem solved using pthreads and conditional varibles.
// Policy:
// Allow any number of readers to read the resource
// provided no thread is currently writing to the resource
// Here access to the shared resource is the regulated through
// a proxy variable `resource_counter`.
// When resource_counter == -1 => Some thread is writing to the resource
// When resource_counter == 0  => No thread is currently using the resource
// When resource_counter > 0   => More than one thread is reading the resource.

// NOTE: This implementation is highly unfair to writers!
#include<pthread.h>
#include<stdio.h>
#include<stdlib.h>


#define WRITERPRESENT -1

// Proxy variable for regulating access to the shared resource.
int resource_counter;
pthread_mutex_t mutex;
pthread_cond_t condr, condw;


void read_from_file(){
    write(1,"Read from file\n", 15);
}

void write_to_file(){
    write(1,"Wrote to the file\n", 18);
}

void read_to_resource(){
    pthread_mutex_lock(&mutex);
    // Wait if writer is present.
    while(resource_counter == WRITERPRESENT){
        write(1, "Reader waiting as some other thread is writing\n",47);
        pthread_cond_wait(&condr, &mutex);
    }
    // Increment resource counter to update the number of threads currently reading the resource
    resource_counter++;
    pthread_mutex_unlock(&mutex);
    
    read_from_file();

    pthread_mutex_lock(&mutex);
    resource_counter--;
    // Signal writer to wakeup if any writer is waiting.
    if (resource_counter == 0){
        pthread_cond_signal(&condw);
    }
    pthread_mutex_unlock(&mutex);
}


void write_to_resource(){
    pthread_mutex_lock(&mutex);
    // Wait till all readers leave
    while(resource_counter != 0){
        write(1, "Writer waiting as there are readers present \n", 45);
        pthread_cond_wait(&condw, &mutex);
    }
    // Update resource_counter to let other threads know that the resource is currently being written to.
    resource_counter = -1;
    pthread_mutex_unlock(&mutex);
    
    
    write_to_file();


    pthread_mutex_lock(&mutex);
    resource_counter = 0;
    // Broadcast readers waiting to wakeup
    pthread_cond_broadcast(&condr);
    // If a writer came in when the resource was being written to give the writer also a chance 
    // to access the resource by signaling it to wakeup
    pthread_cond_signal(&condw);
    pthread_mutex_unlock(&mutex);
}

int main(){
    pthread_t writer[1000], reader[1000];
    pthread_mutex_init(&mutex, 0);
    pthread_cond_init(&condw, 0);
    pthread_cond_init(&condr, 0);

    // Randomly create the reader and writer threads.
    for(int i = 0; i < 1000; i++){
        if((rand()%2) == 0){
            pthread_create(&writer[i], 0, write_to_resource, 0);
        }
        else{
            pthread_create(&reader[i], 0, read_to_resource, 0);            
        }
    }

    // Wait for the child threads to finish
    for (int i = 0; i < 1000; i++){
        pthread_join(writer[i], 0);
        pthread_join(reader[i], 0);
    }
    // Cleanup the resources
    pthread_cond_destroy(&condw);
    pthread_cond_destroy(&condr);
    pthread_mutex_destroy(&mutex);
    return 0;
}