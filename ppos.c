#include "ppos.h"

// A tarefa em execução
task_t* current_task;

// A fila de tarefas
queue_t* queue;

// O despachante das tarefas
task_t dispatcher;


void task_yield () {

    //verifica o tamanho da fila se>1 chama(swap) o dispatcher
    //verifica a prioridade e executa a mais priotária
    // Passa o controle paro o dispatcher

    // Chama o body do dispatcher

}

void dispatcher_body() {

    printf("entrei dispatcher!!!!!!\n") ;

    // Vê se tem tarefas a serem executadas

    // Se tem

    // Chama o scheduler

    // Acho que faz via task_switch

}

void ppos_init (){

    setvbuf (stdout, 0, _IONBF, 0) ;

    queue = NULL;

    // // Cria contexto de main

    // Cria a tarefa de dispatcher
    task_create (&dispatcher, dispatcher_body, "Dispatcher") ;
    dispatcher.is_dispatcher = 1;
    task_setprio(&dispatcher, -20);
    current_task = &dispatcher;

} 

task_t* scheduler() {

    // Ordena as tarefas

    // Envelhecimento

    // Retorna a primeira da fila && não é dispatcher

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

}

// retorna a prioridade estática de uma tarefa (ou a tarefa atual)
int task_getprio (task_t *task) {

    if(task == NULL){
        task = current_task;
    }

    return task->prio;

}

int task_switch (task_t *task) {

    // swapcontext()

}


void task_exit (int exitCode) {}

int task_create (task_t *task,			
                 void (*start_func)(void *),	
                 void *arg) 
{

    task->prio = 0;
    task->dinamic_prio = 0;    
    task->is_dispatcher = 0;

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
    printf("TAMANHO DA FILA: %d", queue_size(queue));    

}	