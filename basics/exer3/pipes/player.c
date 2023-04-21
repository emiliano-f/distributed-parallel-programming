#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "response.h"
#include "player.h"

#define TRUE 1
#define FALSE 0
#define READ 0
#define WRITE 1
#define BUFFER_SIZE 256
#define TARGET 7.5

int become_player(int id, int fd[], int fd_player[], sem_t *semaphore, sem_t *sem_croupier){
    
    // waiting for all process (desde while)
    char buffer[BUFFER_SIZE];
    float score = 0, punt = 0; 
    int in_game = TRUE;
    Response to_send;
    to_send.id = id;
    to_send.res = in_game;

    while (in_game){
        if (sem_wait(semaphore) == -1){
            fprintf(stderr, "error decrementing %d\n", id);
            exit(EXIT_FAILURE);
        }
        read(fd[READ], buffer, BUFFER_SIZE); 
        if (sem_post(sem_croupier) == -1) {
            fprintf(stderr, "error liberando croupier %d\n", id);
            exit(EXIT_FAILURE);
        }
        punt = atof(buffer);
        printf("Proc %d, leido %f, total %f\n", id, punt, score + punt);
        score += punt;

        if (score >= TARGET || TARGET - score <= 0.5){
            //no continua
            in_game = FALSE;
            to_send.res = in_game; 
            write(fd_player[WRITE], &to_send, sizeof(to_send));
        }
        else {
            //continua
            write(fd_player[WRITE], &to_send, sizeof(to_send));
        } 
        printf("proceso %d en el juego: %d\n", id, in_game);
        memset(buffer, 0, BUFFER_SIZE);

    }
    return 0;
}
