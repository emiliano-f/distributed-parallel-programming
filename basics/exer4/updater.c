#include <stdio.h>

#define BUFFER_SIZE 128

Vehicle read_vehicle(char* line){
    /* obtiene atributos de la linea leída */

    Vehicle vehicle;
    
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
        'm': 
            vehicle.cat = MOTOCICLETA;
            break;
        'c':
            vehicle.cat = COCHE;
            break
        'u':
            vehicle.cat = UTILITARIOS;
            break;
        'p':
            vehicle.cat = PESADOS;
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

void *run_updater(){
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
            pthread_mutex_lock(&neof_lock);
            neof = FALSE;
            pthread_mutex_unlock(&neof_lock);
            fclose(fp);
            pthread_exit(NULL);
        }

        pthread_mutex_lock(&station_lock);
        while (station.free == 0){
            pthread_cond_wait(&station_cond, &station_lock);
        } 
        add_to_station(&(station.vehicle), STATION_SIZE, (void*) &vehic);
        station.free -= 1;
        pthread_mutex_unlock(&station_lock);
    }
}
