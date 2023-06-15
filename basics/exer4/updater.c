#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "common.h"
#include "structs.h"
#include "queue.h"
#include "updater.h"

#define BUFFER_SIZE 128

//Station station;
//pthread_mutex_t station_lock;                                                   
//pthread_cond_t station_cond;

Vehicle read_vehicle(char* line){
    /* obtiene atributos de la linea leída */

    Vehicle vehicle;
    // guardamos la linea para el archivo de salida
    strcpy(vehicle.label, line);
    
    // DISCLAIMER: af muy barato
    char tmp[13]; 
    int i = 0;
    line += 3;
    while (*line != ';'){
        tmp[i] = *line;
        i += 1;
        line += 1;
    }
    tmp[i] = '\0';
    vehicle.id = atoi(tmp);

    i = 0;
    line += 5;
    while (*line != ';') {
        tmp[i] = *line;
        i += 1;
        line += 1;
    }
    switch (tmp[0]){
        case 'm': 
            vehicle.cat = MOTOCICLETA;
            break;
        case 'c':
            vehicle.cat = COCHE;
            break;
        case 'u':
            vehicle.cat = UTILITARIO;
            break;
        case 'p':
            vehicle.cat = PESADO;
            break;
        default:
            printf("nave espacial");
    }
    
    
    i = 0;
    line += 5;
    while (*line != '\0'){
        tmp[i] = *line;
        i += 1;
        line += 1;
    }
    tmp[i] = '\0';    
    vehicle.din = atof(tmp);
    return vehicle;
}

void *run_updater(void *things){
    int *neof = ((Updater *) things)->neof;
    pthread_mutex_t *neof_lock = ((Updater *) things)->neof_lock;
    pthread_cond_t *neof_cond = ((Updater *) things)->neof_cond;
    FILE *fp;
    char buffer_line[BUFFER_SIZE];
    Vehicle vehic;

    // file
    fp = fopen("vehicles.in", "r");
    if (fp == NULL){
        perror("file not opened");
        exit(EXIT_FAILURE);
    }
    
    while (TRUE) {

        if (fgets(buffer_line, BUFFER_SIZE, fp)) {
            vehic = read_vehicle(&buffer_line);
        }
        else {
            // ingresa sólo si no hay más que leer
            // por tanto, antes de cerrar ejecucion
            // indica que el archivo está leido completamenet
            pthread_mutex_lock(neof_lock);
            *neof = FALSE;
            pthread_mutex_unlock(neof_lock);
            fclose(fp);
            //pthread_exit(NULL);
            break;
        }

        pthread_mutex_lock(&station_lock);
        while (station.free == 0){
            pthread_cond_wait(&station_cond, &station_lock);
        }
        add_queue(&(station.vehicles_q), STATION_SIZE, (void*) &vehic);
        station.free -= 1;
        pthread_mutex_unlock(&station_lock);
    }
}
