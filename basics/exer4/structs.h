#ifndef STRUCTS_H
#define STRUCTS_H
#include <stdio.h>
#include <pthread.h>
#include "structs.h"
#include "common.h"

// queue
typedef struct Queue{
    void* queue;
    int tail;
    int head;
} Queue;

// vehicles id
typedef struct Vehicle{
    int id;
    int cat;
    float din;
    char label[50];
} Vehicle;

// station with queue
typedef struct Station{
    Vehicle vehicles[STATION_SIZE];
    Queue vehicles_q;
    int num_tick;
    pthread_cond_t num_tick_cond;
    int free;
} Station;

typedef struct Checkout{
    int id;
    Vehicle vehicle;
    int end_work;
    int busy;
    // queue of checkpoints
    Queue *checkout_q;
    pthread_mutex_t *checkout_q_lock;
    // availability of checkout point
    pthread_mutex_t checkout_lock;
    // signal for me
    pthread_cond_t checkout_cond;
    // i'm available
    pthread_cond_t *im_available_cond;
    // output file
    FILE* fp_out;
    pthread_mutex_t *fp_out_lock;
} Checkout;

typedef struct Updater{
    // lectura del input file
    int *neof;
    pthread_mutex_t* neof_lock;
    pthread_cond_t* neof_cond;
} Updater;

extern Station station;
extern pthread_mutex_t station_lock;
extern pthread_cond_t station_cond;
#endif
