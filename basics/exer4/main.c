#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "common.h"
#include "structs.h"
#include "queue.h"
#include "checkout.h"
#include "updater.h"

//Station station = {.free = STATION_SIZE, .num_tick = 0};
//pthread_mutex_t station_lock;
//pthread_cond_t station_cond;

int main(){
    station.free = STATION_SIZE; 
    station.num_tick = 0;
    station.vehicles_q.queue = (void*) &station.vehicles;
    station.vehicles_q.head = 0;
    station.vehicles_q.tail = EMPTY;
    // threads
    pthread_t updater;
    pthread_t checkout[CHECKOUTS];

    // checkout queue (availability)
    Queue checkouts = {{0,1,2,3}, EMPTY, 0};
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
    pthread_cond_init(&station_cond, NULL);

    pthread_mutex_init(&checkouts_lock, NULL);
    pthread_mutex_init(&fp_out_lock, NULL);
    pthread_mutex_init(&neof_lock, NULL);

    // updater thread
    Updater for_updater = {&neof, &neof_lock};
    if (pthread_create(&updater, NULL,
                       (void*) run_updater,
                       (void*) &for_updater) != 0) {
        perror("update queue failed");
        exit(EXIT_FAILURE);
    }

    // checkout threads
    pthread_cond_init(&checkout_available_cond, NULL);
    Checkout for_thread[CHECKOUTS];
    for (i=0; i<CHECKOUTS; i++);{
        for_thread[i].id = i;
        for_thread[i].busy = FALSE;
        for_thread[i].end_work = FALSE;
        for_thread[i].fp_out_lock = &fp_out_lock;
        for_thread[i].fp_out = fp_out;
        for_thread[i].im_available_cond = &checkout_available_cond;
        for_thread[i].checkout_q = &checkouts;
        for_thread[i].checkout_q_lock = &checkouts_lock;
        pthread_mutex_init(&(for_thread[i].checkout_lock), NULL);
        pthread_cond_init(&(for_thread[i].checkout_cond), NULL);
        pthread_create(&checkout[i], NULL,
                       (void*) run_checkout,
                       (void*) &for_thread[i]);
    }

    // station init num_tick conditional
    pthread_cond_init(&station.num_tick_cond, NULL);
    int free_thr; // thread disponible
    Vehicle current;
    while (TRUE) {
        pthread_mutex_lock(&station_lock);
        if (!(is_empty(&station.vehicles_q))) {
            // obtenemos vehiculo siguient
            current = *((Vehicle *) get_queue(STATION_SIZE, &station.vehicles_q));
            pthread_mutex_unlock(&station_lock);

            // obtenemos thread disponible
            pthread_mutex_lock(&checkouts_lock);
            if (is_empty(&checkouts)){
                pthread_cond_wait(&checkout_available_cond, &checkouts_lock);
            }
            free_thr = *((int*) get_queue(CHECKOUTS, &checkouts));
            pthread_mutex_unlock(&checkouts_lock);

            // entregamos el vehiculo al thread
            // no bloqueo el mutex porque se supone que ya
            // está suspendido el hilo
            // UPDATE
            // sí bloqueo porque en una seccion vuelve a consultar
            // si lo tiene antes de suspenderse
            pthread_mutex_lock(&for_thread[free_thr].checkout_lock);
            for_thread[free_thr].vehicle = current;
            for_thread[free_thr].busy = TRUE;
            pthread_mutex_unlock(&for_thread[free_thr].checkout_lock);
            pthread_cond_signal(&(for_thread[free_thr].checkout_cond));

            // señal para «updater» por si está esperando que se liberen
            // lugares de la cola de vehiculos
            pthread_mutex_lock(&station_lock);
            station.free += 1;
            pthread_mutex_unlock(&station_lock);
            pthread_cond_signal(&station_cond);
        }
        else {
            pthread_mutex_unlock(&station_lock);
            pthread_mutex_lock(&neof_lock);
            if (neof == FALSE) {
                pthread_mutex_unlock(&neof_lock);
            }
            else {
                // no más elementos en la fila ni en el file
                pthread_mutex_unlock(&neof_lock);
                break;
            }
        }
    }

    // activa a los threads disponibles, sin vehiculo, para
    // que terminen su tarea
    for (i=0; i<CHECKOUTS; i++) {
        pthread_mutex_lock(&checkouts_lock);
        if (is_empty(&checkouts)){
            pthread_cond_wait(&checkout_available_cond, &checkouts_lock);
        }
        free_thr = *((int*) get_queue(CHECKOUTS, &checkouts));
        pthread_mutex_unlock(&checkouts_lock);
        for_thread[free_thr].end_work = TRUE;
        pthread_cond_signal(&(for_thread[free_thr].checkout_cond));
    }

    // cerramos todo
    pthread_mutex_destroy(&station_lock);
    pthread_mutex_destroy(&checkouts_lock);
    pthread_mutex_destroy(&fp_out_lock);
    pthread_mutex_destroy(&neof_lock);
    //pthread_mutex_destroy(&station.num_tick_cond); //no existe
    pthread_cond_destroy(&checkout_available_cond);
    pthread_cond_destroy(&station_cond);
    pthread_cond_destroy(&station.num_tick_cond);
    for (i=0; i<CHECKOUTS; i++);{
        pthread_mutex_destroy(&(for_thread[i].checkout_lock));
        pthread_cond_destroy(&(for_thread[i].checkout_cond));
    }
    fclose(fp_out);
    exit(EXIT_SUCCESS);
}

