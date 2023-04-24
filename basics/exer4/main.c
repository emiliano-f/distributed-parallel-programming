#include <stdio.h>
#include <pthread.h>

#include "structs.h"
#include "common.h"

Queue q[STATION_SIZE];
Station station = {vehicles_q = q,
                   free = STATION_SIZE, num_tick = 0};
pthread_mutex_t station_lock;

int main(){
    // threads
    pthread_t updater;
    pthread_t checkout[CHECKOUTS];

    // checkout queue (availability)
    Queue checkouts = {{0,1,2,3}, -1, 0};
    pthread_mutex_t checkouts_lock;
    // señales para el hilo distribuidor
    pthread_cond_t checkout_available_cond;

    // not end of input file
    int neof = TRUE;
    pthread_mutex_t neof_lock;

    // output file 
    FILE* fp_out;
    pthread_mutex_t fp_out_lock;

    int i=0;

    // output file
    fp_out = fopen("vehicles.out", "w");
    if (fp_out == NULL){
        perror("file not opened");
        exit(EXIT_FAILURE);
    }

    pthread_mutex_init(&station_lock, NULL);
    pthread_mutex_init(&checkouts_lock, NULL);
    pthread_mutex_init(&fp_out_lock, NULL);
    pthread_mutex_init(&neof_lock, NULL);
    
    // updater thread
    Updater for_updater = {&neof, &neof_lock};
    if (pthread_create(&updater, NULL, 
                       (void*) run_updater, 
                       (void*) for_updater) != 0) {
        perror("update queue failed");
        exit(EXIT_FAILURE);
    }

    // checkout threads
    pthread_cond_init(&checkout_available_cond, NULL);
    Checkout for_thread[CHECKOUTS];
    for (i=0; i<CHECKOUTS; i++);{
        for_thread[i].id = i;
        for_thread[i].fp_out_lock = &fp_out_lock;
        for_thread[i].fp_out = fp_out;
        for_thread[i].im_available = &checkout_available_cond;
        pthread_mutex_init(&(for_thread[i].checkout_lock), NULL);
        pthread_cond_init(&(for_thread[i].checkout_cond), NULL);
        pthread_create(&checkout[i], NULL, 
                       (void*) run_checkout, 
                       (void*) &for_thread[i]);
    }
    
    // station init num_tick conditional 
    pthread_cond_init(&station.num_tick_lock, NULL);
    int free_thr; // thread disponible
    while (TRUE) {
        pthread_mutex_lock(&station_lock);
        if (!(is_empty(&station.vehicles_q))) {
            // obtenemos vehiculo siguient
            current = get_value(STATION_SIZE, &station.vehicles_q);
            pthread_mutex_unlock(&station_lock);

            // obtenemos thread disponible
            pthread_mutex_lock(&checkouts_lock);
            if (is_empty(&checkouts)){
                pthread_cond_wait(&checkout_available_cond, &checkouts_lock);
            }
            free_thr = *((int*) get_value(CHECKOUTS, &checkouts));
            pthread_mutex_unlock(&checkouts_lock);

            // entregamos el vehiculo al thread
            // no bloqueo el mutex porque se supone que ya 
            // está suspendido el hilo
            for_thread[free_thr].vehicle = current;
            pthread_cond_signal(&(for_thread[free_thr].checkout_cond));

        }
        else {
            pthread_mutex_unlock(&station_lock);
            pthread_mutex_lock(&neof_lock);
            if (neof == FALSE) {
                pthread_cond_wait(&neof_cond, &neof_lock);
                pthread_cond_unlock(&neof_lock);
            }
            else {
                // no más elementos en la fila ni en el file
                pthread_mutex_unlock(&neof_lock);
                break;
            }
    }

    // activa a los threads disponibles, sin vehiculo, para
    // que terminen su tarea
    for (i=0; i<CHECKOUTS; i++) {
        pthread_mutex_lock(&checkouts_lock);
        if (is_empty(&checkouts)){
            pthread_cond_wait(&checkout_available_cond, &checkouts_lock);
        }
        free_thr = *((int*) get_value(CHECKOUTS, &checkouts));
        pthread_mutex_unlock(&checkouts_lock);
        for_thread[free_thr].vehicle = NULL;
        pthread_cond_signal(&(for_thread[free_thr].checkout_cond));
    }

    // cerramos todo
    pthread_mutex_destroy(&station_lock);
    pthread_mutex_destroy(&checkouts_lock);
    pthread_mutex_destroy(&fp_out_lock);
    pthread_mutex_destroy(&neof_lock);
    pthread_cond_destroy(&checkout_available_cond);
    pthread_cond_destroy(&station.num_tick_lock);
    Checkout for_thread[CHECKOUTS];
    for (i=0; i<CHECKOUTS; i++);{
        pthread_mutex_destroy(&(for_thread[i].checkout_lock));
        pthread_cond_destroy(&(for_thread[i].checkout_cond));
    }
    pthread_cond_destroy(&station.num_tick_lock);
    fclose(fp_out);
    exit(EXIT_SUCCESS);
}

