#include <stdio.h>
#include <mpi.h>

int main(int argc, char **argv){

    MPI_Init(&argc, &argv);
    int size, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &size);                                       
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    //------------------------------ (a) 
    int color = (rank >= 0 && rank < 4
                 || rank >= 8 && rank < 12) ? 1 : 0;
    int key = (color == 0) ? rank : 15 - rank;
    MPI_Comm new_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, key, &new_comm);

    //------------------------------ (b) 
     /*
    int color = (rank%2 == 0) ? 1 : 0;
    int key = (color == 0) ? rank : 15 - rank;
    MPI_Comm new_comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, key, &new_comm);
    */
}
