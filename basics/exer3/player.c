#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]){
    
    char buffer[1024];
    ssize_t nbytes;
    
    nbytes = read(STDIN_FILENO, buffer, sizeof(buffer));

    if (nbytes == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    // Write to stdout
    if (write(STDOUT_FILENO, buffer, nbytes) != nbytes) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    return 0;
}
