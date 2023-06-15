#include <stdio.h>
#include <mpi.h>

void Bcast(int source, char *msg, int tam) {
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == source) {
        for (int dest = 0; dest < size; dest++) {
            if (dest != source) {
                MPI_Send(msg, tam, MPI_CHAR, dest, 0, MPI_COMM_WORLD);
            }
        }
    } else {
        MPI_Recv(msg, tam, MPI_CHAR, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }
}

int main(int argc, char **argv) {

}
