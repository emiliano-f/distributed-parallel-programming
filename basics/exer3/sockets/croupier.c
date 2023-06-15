#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <semaphore.h>
#include <math.h>
#include "response.h"
#include "croupier.h"

#define WRITE 1
#define READ 0
#define BUFFER_SIZE 256

#define TRUE 1
#define FALSE 0
#define AS "1"
#define DOS "2"
#define TRES "3"
#define CUATRO "4"
#define CINCO "5"
#define SEIS "6" 
#define SIETE "7"
#define SOTA "0.5"
#define CABALLO "0.5"
#define REY "0.5"
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
    
char *linked_to_string(struct linked_t *node) {
    // Primero, contamos el tamaño total necesario para la cadena
    int total_size = 0;
    struct linked_t *curr = node;
    while (curr != NULL) {
        total_size += snprintf(NULL, 0, " %d", curr->winner); // Obtiene el tamaño de la cadena que se añadirá
        curr = curr->next;
    }
    
    // A continuación, reservamos memoria suficiente para la cadena completa
    char *str = malloc((total_size + 1) * sizeof(char)); // Añadimos 1 para el caracter nulo
    str[0] = '\0'; // Inicializamos la cadena con un carácter nulo
    
    // Agregamos cada elemento de la lista a la cadena
    curr = node;
    while (curr != NULL) {
        char curr_str[12]; // Suficiente para almacenar un número entero de 32 bits en decimal
        snprintf(curr_str, sizeof(curr_str), " %d", curr->winner); // Convierte el número a una cadena de caracteres
        strcat(str, curr_str); // Agrega la cadena al final de la cadena principal
        curr = curr->next;
    }
    
    return str;
}

void free_linked(struct linked_t **node){
    // liberar memoria es una operacion costosa
    // esta forma de implementar (liberar cada que actualizo 
    // el rad) es ineficiente
    void * tmp;
    while (*node != NULL){
        tmp = (*node)->next;
        free(*node);
        *node = tmp;
    }
}

int start_game(int n, int fd[], int fd_players[], sem_t *semaphores[], sem_t *sem_croupier){
    char *cards[10] = {AS, DOS, TRES, CUATRO, CINCO, SEIS,
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
            if (sem_getvalue(semaphores[i], &value) == -1){
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
    while (players_in_game > 0) {
        substract = 0;
        // give cards
        for (i=0; i<n; i++) {
            if (in_game[i]) {
                pos_card = (rand() % (9 + 1));
                write(fd[WRITE], cards[pos_card], BUFFER_SIZE);
                if (sem_post(semaphores[i]) == -1) {
                    fprintf(stderr, "error liberando proceso %d\n", i);
                    exit(EXIT_FAILURE);
                }
                //printf("liberé a %d, y el puntaje que le di es %s\n", i, cards[pos_card]);
                scores[i] += atof(cards[pos_card]);
                if (sem_wait(sem_croupier) == -1){
                    fprintf(stderr, "croupier error decrementing %d\n", i);
                    exit(EXIT_FAILURE);
                }
            }
        }

        // responses
        for (i=0; i<players_in_game; i++) {
            read(fd_players[READ], buffer, sizeof(Response));
            rta = (Response *) buffer;
            in_game[rta->id] = rta->res;
            if (!(rta->res)) substract += 1;
            memset(buffer, 0, BUFFER_SIZE);
        }
        //printf("se retiraron %d jugadores, quedan %d\n", substract, players_in_game-substract);
        players_in_game -= substract;
    }

    //scores
    printf("scores: \n");
    for (i=0; i<n; i++) printf("player %d: %f\n", i, scores[i]);

    // winner
    struct linked_t *winners = NULL;
    float tmp, rad = 7;
    for (i=0; i<n; i++){
        tmp = fabs(TARGET - scores[i]);
        if (tmp < rad) {
            rad = tmp;
            free_linked(&winners);
            insert(&winners, i);
        }
        else if (tmp == rad) {
            insert(&winners, i);
        }
    }
    char* win = linked_to_string(winners);
    printf("Winner(s): %s\n", win);
    free(win);
    free_linked(&winners);
}









