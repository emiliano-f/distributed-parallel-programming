#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include "response.h"
#include "croupier.h"

#define WRITE 1
#define READ 0
#define BUFFER_SIZE 256

#define TRUE 1
#define FALSE 0
#define AS 1
#define DOS 2
#define TRES 3
#define CUATRO 4
#define CINCO 5
#define SEIS 6 
#define SIETE 7
#define SOTA 0.5
#define CABALLO 0.5
#define REY 0.5
#define TARGET 7.5

struct linked_t {
    int winner;
    struct linked_t *next;
};

void insert(struct linked_t **node, int id) {
    struct linked_t *new = (struct linked_t*) malloc(sizeof(struct linked_t));
    
    new -> winner = id;
    new -> next = *node; 
    *node = new;
}
    
char *linked_to_string(struct linked_t *node){
    // Para imprimir la linkedlist es mas eficient recorrerla en lugar de convertirla a string
    // pero para aprender lo dejo así xd

    char *str = (char *) malloc(sizeof(char));
    char curr[4];
    str[0] = '\0';
    while (node != NULL){
        sprintf(curr, "%d", node->winner);

        // +1 para el \0 ya que strlen devuelve total sin \0
        // esto es terriblemente ineficiente, ya que 
        // realloc copia lo de str al nuevo bloque reservado
        // (si es que no encuentra lugar a continuacion para expandir)
        str = (char *)realloc(str, (strlen(str) + strlen(curr) + 1) * sizeof(char));
        strcat(str, curr);
        node = node->next;
    }
    return str;
}

void free_linked(struct linked_t *node){
    // liberar memoria es una operacion costosa
    // esta forma de implementar (liberar cada que actualizo 
    // el rad) es ineficiente
    void * tmp;
    while (node != NULL){
        tmp = node->next;
        free(node);
        node = tmp;
    }
}

int start_game(int n, int fd[], int fd_players[], pid_t pids[], sem_t semaphores[]){
    float cards[10] = {AS, DOS, TRES, CUATRO, CINCO, SEIS,
                        SIETE, SOTA, CABALLO, REY};
    
    // Espera a que todos los procesos hayan iniciado
    int value;    
    int i = 0;
    
    // Chequeamos estado de cada semáforo
    int inc = TRUE;
    int j=0;
    while (inc) {
        inc = FALSE;
        for (i=j; i<n; i++){
            if (sem_getvalue(&semaphores[i], &value) == -1){
                fprintf(stderr, "error getting value %d\n", i);
                exit(EXIT_FAILURE);
            }
            if (value == 1) {
                j = i;
                inc = TRUE; 
                continue;
            }
        }
    }

    // start game
    int players_in_game = n;
    float scores[n];
    int substract;
    int pos_card;
    char buffer[BUFFER_SIZE];
    int in_game[n];
    Response *rta;

    srand(time(0));

    // init
    for (i=0; i<n; i++) {
        // flags for players in game
        in_game[i] = TRUE;
        // scores to zero
        scores[i] = 0;
    }
    int aaa;
    while (players_in_game > 0) {
        // give cards
        for (i=0; i<n; i++) {
            if (in_game[i]) {
                pos_card = (rand() % (9 + 1));
                write(fd[WRITE], &cards[pos_card], sizeof(int));
                sem_getvalue(&semaphores[i], &aaa);
                printf("croupier aca, %d, semaforo vale %d\n", i, aaa);
                if (sem_post(&semaphores[i]) == -1) {
                    fprintf(stderr, "error liberando proceso %d\n", i);
                    exit(EXIT_FAILURE);
                }
                sem_getvalue(&semaphores[i], &aaa);
                printf("liberé a %d, y el semaforo vale %d\n", i, aaa);
                scores[i] += cards[pos_card];
            }
        }

        // responses
        for (i=0; i<players_in_game; i++) {
            printf("croupier esperando respuestas uwu %d\n", i);
            read(fd_players[READ], buffer, BUFFER_SIZE);
            rta = (Response *) buffer;
            in_game[rta->id] = rta->res;
            if (rta->res) substract += 1;
            memset(buffer, 0, BUFFER_SIZE);
        }
        players_in_game -= substract;
    }

    // winner
    struct linked_t *winners = NULL;
    float tmp, rad = 7;
    for (i=0; i<n; i++){
        tmp = abs(TARGET - scores[i]);
        if (tmp < rad) {
            rad = tmp;
            free_linked(winners);
            insert(&winners, i);
        }
        else if (tmp == rad) {
            insert(&winners, i);
        }
    }
    char* win = linked_to_string(winners);
    printf("Winner(s): %s\n", win);
    //free(win);
    free_linked(winners);
}









