#include "ppos.h"

// A tarefa em execução
task_t current_task;

// A fila de tarefas
queue_t queue;

// O despachante das tarefas
task_t dispatcher;

void ppos_init (){

    setvbuf (stdout, 0, _IONBF, 0) ;

    // Cria contexto de main

    // Cria a tarefa de dispatcher

} 

void task_yield () {

    // Passa o controle paro o dispatcher

    // Chama o body do dispatcher

}

void dispatcher_body() {

    // Vê se tem tarefas a serem executadas

    // Ordena as tarefas (escalonador) e retorna a tarefa a ser executada

    // Chama o body da primeira tarefa da fila
    // Acho que faz via task_switch

}

task_t* scheduler() {

    // Ordena as tarefas

    // Retorna a primeira da fila

}

// define a prioridade estática de uma tarefa (ou a tarefa atual)
void task_setprio (task_t *task, int prio) {

    task->prio = prio;

}

// retorna a prioridade estática de uma tarefa (ou a tarefa atual)
int task_getprio (task_t *task) {

    if(task == NULL){
        task = &current_task;
    }

    return task->prio;

}

int task_switch (task_t *task) {}


void task_exit (int exitCode) {}

int task_create (task_t *task,			
                 void (*start_func)(void *),	
                 void *arg) 
{

    task->prio = 0;

    // queue_append(&queue, )
    printf("TAMANHO DA FILA: %d", queue_size(&queue));

    // Calls the body of the function passing it's argument
    // start_func(arg);

}	