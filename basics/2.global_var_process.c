#include <stdio.h>
#include <unistd.h> // Para la función fork()

int main() {
    int pid = fork(); // Crear un nuevo proceso

    if (pid == 0) { // Este es el proceso hijo
        printf("Este es el proceso hijo. PID: %d\n", getpid());
    }
    else if (pid > 0) { // Este es el proceso padre
        printf("Este es el proceso padre. PID del hijo: %d\n", pid);
    }
    else { // Ocurrió un error al crear el proceso
        fprintf(stderr, "Error al crear el proceso\n");
        return 1;
    }

    return 0;
}
