#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define LIMIT 10

int main(int argc, char** argv) {
    int rank, size;
    int ping_pong_count = 0;
    double start_time, end_time, elapsed_time;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    while (ping_pong_count < LIMIT) {
        if (rank == 0) {
            // El proceso 0 envía un mensaje al proceso 1
            start_time = MPI_Wtime();
            MPI_Send(&ping_pong_count, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);

            // El proceso 0 espera a recibir mensaje de 1
            MPI_Recv(&ping_pong_count, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            end_time = MPI_Wtime();
            elapsed_time = end_time - start_time;

            printf("Proceso %d recibió el mensaje %d de proceso 1. Tiempo transcurrido: %f\n",
                   rank, ping_pong_count, elapsed_time);
        } else {
            // El proceso 1 espera a recibir un mensaje del proceso 0
            MPI_Recv(&ping_pong_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            // El proceso 1 devuelve inmediatamente el mensaje al proceso 0 
            MPI_Send(&ping_pong_count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        }
        ping_pong_count++;
    }

    MPI_Finalize();
}
