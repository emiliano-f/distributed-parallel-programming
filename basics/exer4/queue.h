#ifndef QUEUE_H
#define QUEUE_H
#include "structs.h"
void* get_queue(int len, Queue* queue);
void add_queue(Queue* queue, int len, void* value); 
int is_empty(Queue *queue); 
#endif
