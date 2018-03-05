
// The dining philosophers problem is useful for modeling processes that are competing 
// for exclusive access to a limited number of resources, such as I/O devices.

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>

#define THINKING 0
#define HUNGRY   1
#define EATING   2

// Number of philosophers
#define N 5
#define LEFT (i+N-1)%N
#define RIGHT (i+1)%N
// Acts as a semaphore here
sem_t semaphore[N];
// Acts as a mutex to protect critical regions,using a binary semaphore here
sem_t mutex;
sem_t mutex_moniter;

int state[N];
pthread_t threads[N];

// Function prototypes
void initialize_semaphores(void);
void put_down_forks(int);
void take_forks(int);
void check(int);
void eat(int);
void think(int);


void initialize_semaphores(){
    // Initialize mutex with value `1` and share it amoung threads.
    // For more info checkout POSIX `sem_init()`
    sem_init(&mutex, 1, 1);
    sem_init(&mutex_moniter, 1, 1);

    // Initialize semaphores for each of the philosophers.
    for(int i=0; i< N; i++){
        sem_init(&semaphore[i], 1, 0);
    }
    return;
}

void *philosopher(void *a){
    int i = *((int *)a);
    free(a);
    printf("Philosopher %d is here \n", i);
    while(1){
        think(i);
        take_forks(i);
        eat(i);
        put_down_forks(i);
    }
}

void put_down_forks(int i){
    sem_wait(&mutex);
    check(LEFT);
    check(RIGHT);
    sem_post(&mutex);
    return;
}

void take_forks(int i){
    // Acquire mutex
    sem_wait(&mutex);
    // Philosopher is hungry
    state[i] = HUNGRY;
    check(i);
    sem_post(&mutex);
    // Blocks if the philosopher couldnt acquire the forks.
    sem_wait(&semaphore[i]);
    return;
}


void check(int i){
    if (state[i] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING){
        state[i] = EATING;
        sem_post(&semaphore[i]);
    }
    return;
}

void think(int i){
    // enter critical region
    printf("Philosopher %d is thinking\n", i+1);
    sem_wait(&mutex);
    // Philosopher is thinking
    state[i] = THINKING;
    // Release mutex
    sem_post(&mutex);
    return;
}

void eat(int i) {
    sem_wait(&mutex_moniter);
    int count = 0;
    for (int i = 0; i < N; i++){
        if (state[i] == EATING){
            count++;
        }
    }
    sem_post(&mutex_moniter);
    printf("Number of philosophers eating %d \n", count);
    return;
}



int main(){
    initialize_semaphores();
    for(int i=0; i< N; i++){
        int *arg = malloc(sizeof(*arg));
        *arg = i;
        pthread_create(&threads[i], NULL, philosopher, arg);
    }
    // Wait for child threads to finish
    for (int i = 0; i < N; i++){
        pthread_join(threads[i], NULL);
    }

    return 0;
}

