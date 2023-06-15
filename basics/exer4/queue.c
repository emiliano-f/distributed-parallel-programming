#include "queue.h"
#include "common.h"
#include "structs.h"
// si no hay elementos, entonces tail == -1
// 
// hay un problema con queue
// cuando le paso el puntero Queue*, este contiene un campo
// void *queue, por lo que a ese campo apunta a un array de 
// un tipo desconocido
// el problema es que si yo quiero asignar (obtener) un valor
// no puedo indexar debido a que el indice no sabe qué rango de valores
// comprende los valores del array
// entonces el puntero void* queue deberia almacenr un array de punteros
// que tendria un tamaño fijo
// u obtengo el valor mediante aritmetica de punteros
//
void* get_queue(int len, Queue* queue){
    void* value; 
    int index;
    
    if ((*queue).tail != EMPTY) {
        index = (*queue).head;
        (*queue).head = (index+1) % len;
        value = &((*queue).queue[index]);
        // si era el último elemento en la queue
        if (queue->tail == index) {
            queue->tail = EMPTY;
        }
    }
    return value;
}

void add_queue(Queue* queue, int len, void* value) {
    if (queue->tail == EMPTY) {
        queue->tail = queue->head;
        queue->queue[queue->head] = value;
    }
    else {
        // queue llena
        int next = (queue->tail+1) %len;
        if (queue->head == next){
            perror("full buffer");
        }
        else { 
            queue->tail = next;
            queue->queue[next] = value;
        }
    }
}

int is_empty(Queue *queue) {
    int empty = (queue->tail == EMPTY)? TRUE : FALSE;
    return empty;
}
