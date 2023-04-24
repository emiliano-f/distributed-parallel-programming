#include <stdio.h>
#include <pthread.h>
#include "common.h"
#include "struct.h"


void pay_and_time(Vehicle* vehicle, struct timespec* sleep_time) {
    /* descuenta el pago de din y declara segundos de espera */

    switch (vehicle->cat){
        MOTOCICLETA: 
            vehicle->din -= 1;
            sleep_time->tv_sec -= 1;
            break;
        COCHE:
            vehicle->din -= 2.5;
            sleep_time->tv_sec -= 2;
            break;
        UTILITARIO:
            vehicle->din -= 3.25;
            sleep_time->tv_sec -= 3;
            break;
        PESADO:
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
    
    Vehicle* current = &((Checkout*) things).vehicle;
    pthread_mutex_t* checkout_lock = &((Checkout*) things).checkout_lock;
    pthread_cond_t* checkout_cond = &((Checkout*) things).checkout_cond;
    int id = ((Checkout*) things).id;

    struct timespec sleep_time = {
        .tv_sec = 0,
        .tv_nsec = 0 
    };

    while (TRUE) {
        pthread_mutex_lock(checkout_lock);
        // while se usa para prevenir falsa activacion, 
        // util cuando hay varios hilos esperando en la misma
        // variable condicional
        //while (current == NULL){
        if (current == NULL) {
            // que sucede si no hay nada para cobrar?
            // se vuelve a agregar a la fila el mismo checkout?
            // compound literal para pasar una copia de id
            add_queue(&checkouts, CHECKOUTS, (void*) &(int){id});
            pthread_cond_wait(checkout_cond, checkout_mutex);
            if (current == NULL) {
                // se liberó pero no hay nada, por lo que es señal
                // de que no hay más vehiculos que cobrar
                pthread_mutex_unlock(checkout_lock);
                break;
            }
        }
        pay_and_time(&current, &sleep_time);
        nanosleep(&sleep_time, NULL);
        current = NULL;
        pthread_mutex_unlock(checkout_lock);
    }
}
