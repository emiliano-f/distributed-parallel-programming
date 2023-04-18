#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> // Para la función fork()
#include <time.h>

int global_variable = 0;

int main(int argc, char* argv[]) {
    if (argc < 3){
        printf("Error, se necesitan 2 argumentos");
        return 1;
    }
    int pid;
    int n = atoi(argv[1]);
    int m = atoi(argv[2]);
    struct timespec sleep_time = {.tv_sec = 0, .tv_nsec = 0};
    int i = 0;

    for (i = 0; i<n; i++){
        pid = fork();
        if (pid == 0){
            printf("Process1 instancia %d\n", i);
            sleep_time.tv_nsec = rand() % 1000000000;
            nanosleep(&sleep_time, NULL);
            global_variable++;
            break;
        }
    }
    
    for (i = 0; i<m; i++){
        pid = fork();
        if (pid == 0){
            printf("Process2 instancia %d\n", i);
            sleep_time.tv_nsec = rand() % 1000000000;
            nanosleep(&sleep_time, NULL);
            printf("Global variable value %d\n", global_variable);
            break;
        }
    }

    return 0;
}
