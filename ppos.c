#include "ppos.h"

void ppos_init (){

    

} 

// define a prioridade estática de uma tarefa (ou a tarefa atual)
void task_setprio (task_t *task, int prio) {}

// retorna a prioridade estática de uma tarefa (ou a tarefa atual)
int task_getprio (task_t *task) {



}

void task_yield () {}

void task_exit (int exitCode) {}

int task_create (task_t *task,			
                 void (*start_func)(void *),	
                 void *arg) 
{

    task->prio = 0;
    start_func(arg);

}	