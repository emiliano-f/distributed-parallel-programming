#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>

#define TRUE 1                                                            
#define FALSE 0                                                           
#define MASTER 0
#define TAG 0

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

void clear(int winners[], int top) {
    int i;
    for (i=0; i<top; i++) {
        winners[i] = FALSE;
    }
}

int main(int argc, char **argv) {
    double cards[10] = {AS, DOS, TRES, CUATRO, CINCO, SEIS,
                            SIETE, SOTA, CABALLO, REY};       
    int rank, size, msg;
    int i, pos_card, player;
    double card;
    MPI_Status status;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        srand(time(NULL));
        size = size-1;
        int in_game = size;
        // index i: process i+1. (index 0, process 1)
        int responses[in_game], winners[in_game];
        double scores[in_game];

        for (i=0; i<in_game; i++) {
            responses[i] = TRUE;
            scores[i] = 0;
            winners[i] = FALSE;
        }

        while (TRUE) {
            // give cards
            for (i=0; i<size; i++) {
                // i+1 player process
                player = i+1;
                if (responses[i] == FALSE) {
                    continue;
                }
                pos_card = (rand() % (9 + 1));
                card = cards[pos_card];
                scores[i] += card;
                MPI_Send(&card, 1, MPI_DOUBLE, player, TAG, MPI_COMM_WORLD);
            }

            printf("Waiting resp\n");
            // responses
            for (i=0; i<size; i++) {
                player = i+1;
                if (responses[i] == FALSE) {
                    continue;
                }
                MPI_Probe(MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
                if (status.MPI_SOURCE == MPI_UNDEFINED){
                    printf("Error");
                }
                MPI_Recv(&msg, 1, MPI_INT, status.MPI_SOURCE, 
                         TAG, MPI_COMM_WORLD, &status);
                printf("Proceso %d recibió mensaje de proceso %d: %d\n", 
                        rank, status.MPI_SOURCE, msg);
                if (msg == FALSE) {
                    responses[((int) status.MPI_SOURCE)-1] = msg; 
                    in_game--;
                }
            }

            // game over
            printf("In game: %d\n", in_game);
            if (in_game == 0){

                //scores
                printf("Scores: \n");
                for (i=0; i<size; i++) printf("\tPlayer %d: %f\n", i+1, scores[i]);
                // winner
                double tmp, rad = 7;
                for (i=0; i<size; i++){
                    tmp = fabs(TARGET - scores[i]);
                    if (tmp < rad) {              
                        rad = tmp;
                        clear(winners, i);
                        winners[i] = TRUE;   
                    }                         
                    else if (tmp == rad) {   
                        winners[i] = TRUE;
                    }                      
                }
                //printf
                printf("Winners: ");
                for (i=0; i<size; i++) {
                    if (winners[i] == TRUE) {
                        printf("%d ", i+1);
                    }
                }
                printf("\n");
                break;
            }
        }
    }

    else {
        double score = 0;
        while (TRUE) {
            MPI_Recv(&card, 1, MPI_DOUBLE, MASTER, TAG, 
                     MPI_COMM_WORLD, &status);
            score += card;
            printf("Process %d. Dato leido %f, total %f\n", rank, card, score);
            if (score >= TARGET || TARGET - score <= 0.5){
                //no continua
                msg = FALSE;
                MPI_Send(&msg, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD);
                break;
            }                                           
            else {                                     
                //continua                            
                msg = TRUE;
                MPI_Send(&msg, 1, MPI_INT, MASTER, TAG, MPI_COMM_WORLD);
            }
        } 
    }
    MPI_Finalize();
}
