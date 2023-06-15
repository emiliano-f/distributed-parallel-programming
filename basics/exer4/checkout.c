#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include "common.h"
#include "structs.h"
#include "queue.h"
#include "checkout.h"

pthread_mutex_t station_lock;                                                   
pthread_cond_t station_cond;

void pay_and_time(Vehicle* vehicle, struct timespec* sleep_time) {
    /* descuenta el pago de din y declara segundos de espera */

    switch (vehicle->cat){
        case MOTOCICLETA: 
            vehicle->din -= 1;
            sleep_time->tv_sec -= 1;
            break;
        case COCHE:
            vehicle->din -= 2.5;
            sleep_time->tv_sec -= 2;
            break;
        case UTILITARIO:
            vehicle->din -= 3.25;
            sleep_time->tv_sec -= 3;
            break;
        case PESADO:
            vehicle->din -= 5;
            sleep_time->tv_sec -= 4;
            break;
        default:
            printf("nave espacial");
            sleep_time->tv_sec -= 0;
            vehicle->din -= 1000;
    }
}

void *run_checkout(void* things){
    /* funcion para thread de cobro */
    
    Vehicle *current = &((Checkout*) things)->vehicle;
    int *busy = &((Checkout*) things)->busy;
    int *end_work = &((Checkout*) things)->end_work;
    Queue *checkouts = &((Checkout*) things)->checkout_q;
    pthread_mutex_t *checkout_q_lock = &((Checkout*) things)->checkout_q_lock;
    pthread_mutex_t *checkout_lock = &((Checkout*) things)->checkout_lock;
    pthread_cond_t *checkout_cond = &((Checkout*) things)->checkout_cond;
    pthread_cond_t *im_available_cond = &((Checkout*) things)->im_available_cond;
    FILE *fp = &((Checkout*) things)->fp_out;
    pthread_mutex_t *fp_lock = &((Checkout*) things)->fp_out_lock;

    int id = ((Checkout*) things)->id;
    int ticket;

    struct timespec sleep_time = {
        .tv_sec = 0,
        .tv_nsec = 0 
    };

    while (TRUE) {
        // while se usa para prevenir falsa activacion, 
        // util cuando hay varios hilos esperando en la misma
        // variable condicional
        //while (current == NULL){
        if (*busy == FALSE) {
            pthread_mutex_lock(checkout_q_lock);
            // compound literal para pasar una copia de id
            add_queue(checkouts, CHECKOUTS, (void*) &(int){id});
            pthread_mutex_unlock(checkout_q_lock);
            // aviso que agregué el hilo a la fila
            pthread_cond_signal(im_available_cond);
            // bloqueo acceso al hilo
            pthread_mutex_lock(checkout_lock);
            if (*busy == FALSE) {
                // no hay nada así que espera a que haya algo y una señal
                pthread_cond_wait(checkout_cond, checkout_lock);
            }
            if (*end_work == TRUE) {
                // se liberó pero no hay nada, por lo que es señal
                // de que no hay más vehiculos que cobrar
                pthread_mutex_unlock(checkout_lock);
                break;
            }
        }

        // get num tick
        pthread_mutex_lock(&station.num_tick_cond);
        ticket = station.num_tick;
        station.num_tick++;
        pthread_mutex_unlock(&station.num_tick_cond);
        pay_and_time(current, &sleep_time);
        nanosleep(&sleep_time, NULL);

        // to file
        strcpy(current->label, ";tick:");
        strcpy(current->label, sprintf("%d\n", ticket));
        pthread_mutex_lock(fp_lock);
        fprintf(fp, current->label);  
        pthread_mutex_unlock(fp_lock);

        //current = NULL;
        *busy = FALSE;
        pthread_mutex_unlock(checkout_lock);
    }
}
