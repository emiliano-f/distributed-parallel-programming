#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <semaphore.h>
#include "player.h"
#include "croupier.h"

#define SEM_PROCESS 1
#define SEM_INIT_PLAYERS 0
#define READ_END 0
#define WRITE_END 1

void create_child(int id, int fd[], int fd_players[], pid_t* pid, sem_t semaphore){

    *pid = fork(); 
    if (*pid < 0){
        fprintf(stderr, "fork process %d\n", id);
        exit(EXIT_FAILURE);
    } else if (*pid == 0){ 
        close(fd[WRITE_END]); //close write side 
        close(fd_players[READ_END]);
        become_player(id, fd, fd_players, semaphore);
        close(fd[READ_END]);
        printf("process %d\n", *pid);
        close(fd[WRITE_END]);
        exit(EXIT_SUCCESS);
    } 
}

int main(int argc, char *argv[]){
    if (argc < 2){
        perror("Error, se debe ingresar un argumento\n");
        exit(EXIT_FAILURE);
    }
    
    int n = atoi(argv[1]);

    sem_t sem_players[n];
    pid_t pid[n];
    int i, fd[2], fd_players[2];
        
    if (pipe(fd) < 0 || pipe(fd_players) < 0){ //fd[0] read, f[1] write
        perror("Pipe creation");
        exit(EXIT_FAILURE);
    }

    // init sem
    for (i=0; i<n; i++){
        if (sem_init(&sem_players[i], SEM_PROCESS, SEM_INIT_PLAYERS) == -1){
            fprintf(stderr, "sem init error %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for (i=0; i<n; i++){
        create_child(i, fd, fd_players, &pid[i], sem_players[i]);  
    }

    close(fd[READ_END]);
    close(fd_players[WRITE_END]);

    for (i=0; i<n; i++){
        printf("%d\n", pid[i]);
    }
    start_game(n, fd, fd_players, pid, sem_players);

    close(fd[WRITE_END]);
    close(fd_players[READ_END]);
     
    // closing semaphores
    for (i=0; i<n; i++){
        sem_destroy(&sem_players[i]);
    }
    for (i=0; i<n; i++){
        kill(pid[i], SIGTERM);
    }
}
