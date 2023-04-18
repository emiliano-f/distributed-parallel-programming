#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

// Para pasar los semáforos debo hacerlo a traves de un mecanismo
// de comunicacion, por lo que lo más facil sería no usar exec

pid_t create_child(int id, int fd[2]){

    pid_t pid = fork(); 
    if (pid < 0){
        fprintf(stderr, "fork process %d\n", id);
        exit(EXIT_FAILURE);
    }
    else if (pid == 0){
        close(fd[1]); //close write side
        
        // duplicacion del descriptor de archivo del read side
        // ie, la entrada estandar del proceso ya no es el teclado,
        // es el extremo de lectura del pipe
        if (fd[0] != STDIN_FILENO){
            if (dup2(fd[0], STDIN_FILENO) == -1);
                fprintf(stderr, "standard input closed %d\n", id);
                exit(EXIT_FAILURE);
            }
            if (close(fd[0] == -1){
                fprintf(stderr, "close read end failed %d\n", id);
                exit(EXIT_FAILURE);
            }
        }
        
        char id_str[3];
        sprintf(id_str, "%d", id);
        execlp("./player.o", "player.o", id_str, (char *) NULL);
        fprintf(stderr, "execlp process %d\n", id);
        exit(EXIT_FAILURE); 
    } 
    
    return pid;
}

int main(int argc, char *argv[]){
    if (argc < 2){
        perror("Error, se debe ingresar un argumento\n");
        exit(EXIT_FAILURE);
    }

    int n = atoi(argv[1]);
    pid_t pid[n];
    int i, fd[2];
        
    if (pipe(fd) < 0){ //fd[0] read, f[1] write
        perror("Pipe creation");
        exit(EXIT_FAILURE);
    }

    for (i=0; i<n; i++){
        pid[i] = create_child(i, fd);  
    }

    close(fd[0]);
    for (i = 0; i<n; i++){
    write(fd[1], "HelloWorld\n", 12);}
    close(fd[1]);
    
    for (i=0; i<n; i++){
        wait(NULL);
    }
    for (i=0; i<n; i++){
        kill(pid[i], SIGTERM);
    }
}
