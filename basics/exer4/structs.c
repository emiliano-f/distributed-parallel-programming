#include <stdio.h>
#include "structs.h"
#include "common.h"

#define STATION_SIZE

// queue
typedef struct {
    void* queue;
    int tail;
    int head;
} Queue;

// vehicles id
typedef struct {
    int id;
    int cat;
    float din;
} Vehicle;

// station with queue
typedef struct {
    Queue vehicles_q[STATION_SIZE];
    int num_tick;
    pthread_cond_t num_tick_lock;
    int free;
} Station;

typedef struct {
    int id;
    Vehicle vehicle;
    // availability of checkout point
    pthread_mutex_t checkout_lock;
    // signal for me
    pthread_cond_t checkout_cond;
    // i'm available
    pthread_cond_t* im_available_cond;
    // output file
    FILE* fp_out;
    pthread_mutex_t* fp_lock_out;
} Checkout;

typedef struct {
    int *neof;
    pthread_mutex_t* neof_lock;
    pthread_cond_t* neof_cond;
} Updater;
