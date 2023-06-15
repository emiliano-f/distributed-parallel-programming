#ifndef CHECKOUT_H
#define CHECKOUT_H
#include <time.h>
#include "structs.h"

void pay_and_time(Vehicle* vehicle, struct timespec* sleep_time); 
void *run_checkout(void* things);
#endif
