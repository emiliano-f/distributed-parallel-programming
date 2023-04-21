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
#define HIT_VALUE 7.0

int become_player(int id, int fd[], int fd_player[], sem_t semaphore){
    
    // waiting for all process (desde while)
    char buffer[BUFFER_SIZE];
    int score = 0, punt; 
    int in_game = TRUE;
    Response to_send;
    to_send.id = id;
    to_send.res = in_game;
    int aaa;
    sem_getvalue(&semaphore, &aaa);
    printf("proceso %d, y mi semaforo vale %d\n", id, aaa);

    while (in_game){
        printf("estoy aca uwu proceso %d\n", id);
        if (sem_wait(&semaphore) == -1){
            fprintf(stderr, "error decrementing %d\n", id);
            exit(EXIT_FAILURE);
        }
        printf("me liberaron %d\n", id);
        read(fd[READ], buffer, BUFFER_SIZE); 
        punt = atoi(buffer);
        printf("Proc %d, leido %f, total %f", id, punt, score + punt);
        score += punt;

        if (score >= HIT_VALUE || HIT_VALUE - score <= 0.5){
            //no continua
            in_game = FALSE;
            to_send.res = in_game; 
            write(fd_player[WRITE], &to_send, sizeof(to_send));
        }
        else {
            //continua
            write(fd_player[WRITE], &to_send, sizeof(to_send));
        } 

        memset(buffer, 0, BUFFER_SIZE);

    }
    return 0;
}
