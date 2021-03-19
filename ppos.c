#include "ppos.h"

// A tarefa em execução
task_t* current_task;

// A fila de tarefas
queue_t* queue;

// O despachante das tarefas
task_t dispatcher;

int id = 0;

void task_yield () {

    //verifica o tamanho da fila se>1 chama(swap) o dispatcher
    //verifica a prioridade e executa a mais priotária
    // Passa o controle paro o dispatcher

    // Chama o body do dispatcher

    if(current_task == NULL){
        current_task = &dispatcher;
        dispatcher_body();
    }
    task_switch(&dispatcher);
    // dispatcher_body();
}

void dispatcher_body() {

    task_t* next = NULL;
    while(queue_size(queue) > 1){

        next = scheduler();
        if(next != NULL){
            task_switch(next);
        }

    }

    return;

}

void ppos_init (){

    setvbuf (stdout, 0, _IONBF, 0) ;

    queue = NULL;

    // // Cria contexto de main

    // Cria a tarefa de dispatcher
    task_create (&dispatcher, dispatcher_body, "Dispatcher") ;
    dispatcher.is_dispatcher = 1;
    task_setprio(&dispatcher, -20);
    current_task = NULL;

} 

task_t* scheduler() {

    // Ordena as tarefas
    task_t* aux_ini = &dispatcher;
    task_t* next = aux_ini->next;
    task_t* aux_current = next->next;
    

    while(aux_current != aux_ini){
        if(aux_current->dinamic_prio < next->dinamic_prio){
            next = aux_current;
        }
        aux_current = aux_current->next;
    }
    
    aux_current = aux_ini->next;
    while(aux_current != aux_ini){
        aux_current->dinamic_prio -= 1;
        aux_current = aux_current->next;
    }

    next->dinamic_prio = next->prio;
    return next;    

}

// define a prioridade estática de uma tarefa (ou a tarefa atual)
void task_setprio (task_t *task, int prio) {

    int new_prio = prio;
    if(new_prio < -20){
        new_prio = -20;
    }

    if(new_prio > 20){
        new_prio = 20;
    }

    task->prio = new_prio;
    task->dinamic_prio = new_prio;

}

// retorna a prioridade estática de uma tarefa (ou a tarefa atual)
int task_getprio (task_t *task) {

    if(task == NULL){
        task = current_task;
    }

    return task->prio;

}

int task_switch (task_t *task) {
    if(task==NULL){
        return -1;
    }
    else{
        /* salva o estado da tarefa atual e 
        troca para a tarefa recebida */
        task_t* aux = current_task;
        current_task = task;
        swapcontext(&aux->context, &current_task->context);
    }

    return 0; //se deu tudo certo
}


void task_exit (int exitCode) {
    
    task_t* aux = &dispatcher;
    task_t** queueaux = &aux;

    queue_remove((queue_t*) queueaux, (queue_t*) current_task);
    task_yield();
}

int task_create (task_t *task,			
                 void (*start_func)(void *),	
                 void *arg) 
{

    task->prio = 0;
    task->dinamic_prio = 0;    
    task->is_dispatcher = 0;
    task->id = id;
    id = id + 1;

    getcontext (&task->context) ;

    char *stack = malloc (STACKSIZE) ;
    if (stack)
    {
        task->context.uc_stack.ss_sp = stack ;
        task->context.uc_stack.ss_size = STACKSIZE ;
        task->context.uc_stack.ss_flags = 0 ;
        task->context.uc_link = 0 ;
    }
    else
    {
        perror ("Erro na criação da pilha: ") ;
        exit (1) ;
    }

    makecontext (&task->context, (void*)(start_func), 1, arg) ;

    queue_append(&queue, (queue_t*) task);    
    // printf("TAMANHO DA FILA: %d", queue_size(queue));    

}