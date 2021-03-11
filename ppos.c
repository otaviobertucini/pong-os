#include "ppos.h"

// The current executing task
task_t current_task;

// The queue of tasks
queue_t queue;

void ppos_init (){} 

// define a prioridade estática de uma tarefa (ou a tarefa atual)
void task_setprio (task_t *task, int prio) {}

// retorna a prioridade estática de uma tarefa (ou a tarefa atual)
int task_getprio (task_t *task) {

    if(task == NULL){
        task = &current_task;
    }

    return task->prio;

}

void task_yield () {}

void task_exit (int exitCode) {}

int task_create (task_t *task,			
                 void (*start_func)(void *),	
                 void *arg) 
{

    task->prio = 0;

    // queue_append(&queue, )
    printf("TAMANHO DA FILA: %d", queue_size(&queue));

    // Calls the body of the function passing it's argument
    start_func(arg);

}	