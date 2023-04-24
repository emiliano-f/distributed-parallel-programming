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
    Queue vehicle[STATION_SIZE];
    pthread_cond_t num_tick;
    int free;
} Station;

typedef struct {
    int id;
    Vehicle vehicle;
    pthread_mutex_t checkout_lock;
    pthread_cond_t checkout_cond;
    FILE* fp_out;
    pthread_mutex_t* fp_lock_out;
} Checkout;
