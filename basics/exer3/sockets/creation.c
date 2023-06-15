#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>
#include <sys/mman.h>
#include "player.h"
#include "croupier.h"

#define SEM_PROCESS 1
#define SEM_INIT_PLAYERS 0
#define READ_END 0
#define WRITE_END 1

int main(int argc, char *argv[]){
    if (argc < 2){
        perror("Error, se debe ingresar un argumento\n");
        exit(EXIT_FAILURE);
    }
    
    int n = atoi(argv[1]);
    sem_t *sem_players[n];
    sem_t *sem_croupier;
    int i, fd[2], fd_players[2];
        
    if (pipe(fd) < 0 || pipe(fd_players) < 0){ 
        perror("Pipe creation");
        exit(EXIT_FAILURE);
    }

    // init
    // page size para kernel mmap
    // si len es menor a un page size, se reserva 
    // al multiplo superior de un page size mas cercano
    // al unmapping, sucede igual: por lo tanto, se 
    // puede dar el mismo len y hará unmapping al 
    // multiplo superior mas cercano
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size == -1) {
        perror("Error al obtener el tamaño de página");
        exit(EXIT_FAILURE);
    }
    
    sem_croupier = mmap(NULL, page_size, PROT_READ | PROT_WRITE,
                        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (sem_croupier == MAP_FAILED) {
        perror("error mapping for croupier sem\n");
        exit(EXIT_FAILURE);
    }
    if (sem_init(sem_croupier, SEM_PROCESS, SEM_INIT_PLAYERS) == -1){
        perror("error init croupier sem\n");
        exit(EXIT_FAILURE);
    }

    for (i=0; i<n; i++){
        // anonymous mapping
        sem_players[i] = mmap(NULL, page_size, PROT_READ | PROT_WRITE, 
                              MAP_SHARED | MAP_ANONYMOUS, -1, 0);
        if (sem_players[i] == MAP_FAILED) { 
            fprintf(stderr, "error mapping for %d\n", i);
            exit(EXIT_FAILURE);
        }

        //init sem
        if (sem_init(sem_players[i], SEM_PROCESS, SEM_INIT_PLAYERS) == -1){
            fprintf(stderr, "sem init error %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    // to fork()
    for (i=0; i<n; i++){
        //create_child(i, fd, fd_players, &pid[i], sem_players[i]);  
        switch (fork()){
            case -1:
                fprintf(stderr, "fork process %d\n", i);
                exit(EXIT_FAILURE);
            case 0: 
                close(fd[WRITE_END]); //close write side 
                close(fd_players[READ_END]);

                become_player(i, fd, fd_players, sem_players[i], sem_croupier);

                close(fd[READ_END]);
                close(fd_players[WRITE_END]);       

                // closing semaphores and unmapping
                sem_destroy(sem_croupier);
                if (munmap(sem_croupier, page_size) == -1) {
                    fprintf(stderr, "error munmap sem_croupier from process %d\n", i);
                    printf("errno value: %d\n", errno);
                    //exit(EXIT_FAILURE);
                }
                for (i=0; i<n; i++){
                    sem_destroy(sem_players[i]);
                    if (munmap(sem_players[i], page_size) == -1) {
                        fprintf(stderr, "error munmap %d\n", i);
                        printf("errno value: %d\n", errno);
                        exit(EXIT_FAILURE);
                    }
                }
                exit(EXIT_SUCCESS);
            default:
                break;
        }
    }

    // only parent process ---------------------------------------
    close(fd[READ_END]);
    close(fd_players[WRITE_END]);

    start_game(n, fd, fd_players, sem_players, sem_croupier);

    close(fd[WRITE_END]);
    close(fd_players[READ_END]);
     
    // closing semaphores and unmapping
    sem_destroy(sem_croupier);
    if (munmap(sem_croupier, sizeof(sem_t)) == -1) {
        fprintf(stderr, "error munmap sem_croupier from pparent");
        printf("errno value: %d\n", errno);
        exit(EXIT_FAILURE);
    }
    for (i=0; i<n; i++){
        sem_destroy(sem_players[i]);
        if (munmap(sem_players[i], sizeof(sem_t)) == -1) {
            fprintf(stderr, "parent: error munmap %d\n", i);
            exit(EXIT_FAILURE);
        }
    }
}
