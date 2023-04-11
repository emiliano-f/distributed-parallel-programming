#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>

int global_variable = 0;

void *thread_type1(void *str){
    printf("Thread1 instancia %d\n", pthread_self());
    struct timespec *sleep_time = (struct timespec*) str;

    // Suspender la ejecución del hilo durante el tiempo especificado
    struct timespec sleep_spec = { .tv_sec = 0, .tv_nsec = sleep_time };
    nanosleep(&sleep_spec, NULL);

    global_variable++;
}

void *thread_type2(void *str){
    printf("Thread2 instancia %d\n", pthread_self());
    struct timespec * sleep_time = (struct timespec*) str;

    struct timespec sleep_spec = { .tv_sec = 0, .tv_nsec = sleep_time };
    nanosleep(&sleep_spec, NULL);

    printf("Valor variable global: %d\n", global_variable);
}

int main(int argc, char *argv[]) {
    if(argc < 3) {
        printf("Error: se deben proporcionar al menos dos argumentos.\n");
        return 1;
    }

    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    
    //printf("El primer argumento es: %d\n", arg1);
    //printf("El segundo argumento es: %d\n", arg2);

    // Creacion de hilos
    pthread_t threads_type1[n];
    pthread_t threads_type2[m];
    
    // Estructura timespec para el tiempo de suspensión
    struct timespec sleep_time = {
        .tv_sec = 0, 
        .tv_nsec = 0
    };

    int i = 0;
    
    for (i=0; i<n; i++){
        sleep_time.tv_nsec = rand() % 1000000000;
        pthread_create(&threads_type1[i], NULL, thread_type1, (void *)&sleep_time);
    }

    for (i=0; i<m; i++){
        sleep_time.tv_nsec = rand() % 1000000000;
        pthread_create(&threads_type2[i], NULL, thread_type2, (void *)&sleep_time);
    }

    for (i = 0; i<n; i++){
        pthread_join(threads_type1[i], NULL);
    }

    for (i = 0; i<m; i++){
        pthread_join(threads_type2[i], NULL);
    }
    
    printf("Final value global variable: %d", global_variable);

    return 0;
}

