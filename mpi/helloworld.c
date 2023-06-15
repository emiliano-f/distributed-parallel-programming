#include <stdio.h>
#include <mpi.h>

int main (int argc, char ** argv) {
    char name[MPI_MAX_PROCESSOR_NAME];
    int resultlen, rank, size;

    MPI_Init(&argc, &argv);
    MPI_Get_processor_name(name, &resultlen);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    printf("Hola Mundo! Nombre proceso: %c, Nro %d de %d\n", name, rank, size);
    MPI_Finalize();
}
