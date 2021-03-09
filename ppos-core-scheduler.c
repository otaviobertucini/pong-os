#include "ppos.h"
#include "ppos-core-globals.h"

task_t * scheduler() {
    // FCFS scheduler
    if ( readyQueue != NULL ) {
        return readyQueue;
    }
    return NULL;
}


void task_setprio (task_t *task, int prio) {
    // do something
}

int task_getprio (task_t *task) {
    return 0;
}