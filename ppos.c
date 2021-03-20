#include "ppos.h"

// A tarefa em execução
task_t* current_task;

// A fila de tarefas
queue_t* queue;

// O despachante das tarefas
task_t dispatcher;

int id = 0;


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


void task_yield () {

    // Passa o controle paro o dispatcher
    // Chama o body do dispatcher

    if(current_task == NULL){
        current_task = &dispatcher;
        dispatcher_body();
    }
    task_switch(&dispatcher);

}



void ppos_init (){

    setvbuf (stdout, 0, _IONBF, 0) ;

    queue = NULL;

    // Cria a tarefa de dispatcher
    task_create (&dispatcher, dispatcher_body, "Dispatcher") ;
    dispatcher.is_dispatcher = 1;
    task_setprio(&dispatcher, -20);
    current_task = NULL;

} 

task_t* scheduler() {

    task_t* aux_ini = &dispatcher;
    task_t* next = aux_ini->next;
    task_t* aux_current = next->next;
    

    while(aux_current != aux_ini){ 
        if(aux_current->dinamic_prio < next->dinamic_prio){
            next = aux_current;    //armazena a maior prioridade
        }
        aux_current = aux_current->next; //percorre a fila
    }
    
    aux_current = aux_ini->next; //aponta novamente à primeira tarefa da fila

    while(aux_current != aux_ini){      //task aging com 'α = -1'
        aux_current->dinamic_prio -= 1; //atualiza a prioridade dinâmica de cada tarefa
        aux_current = aux_current->next;
    }

    next->dinamic_prio = next->prio; //reinicia a prioridade dinâmica
    return next;    

}

// define a prioridade estática de uma tarefa (ou a tarefa atual)
void task_setprio (task_t *task, int prio) {

    if(prio < -20) task->prio = -20;
    if(prio > 20) task->prio = 20;
    task->prio = prio;
    task->dinamic_prio = prio;
    
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
        /* salva o estado da tarefa atual e troca para a tarefa recebida */
        task_t* aux = current_task;
        current_task = task;
        swapcontext(&aux->context, &current_task->context);
    }

    return 0; //se deu tudo certo
}


void task_exit (int exitCode) {
    
    queue_remove((queue_t**) &dispatcher, (queue_t*) current_task);
    task_yield();
}

int task_create (task_t *task,          
                 void (*start_func)(void *),    
                 void *arg) 
{

    task->prio = 0; //prioridade default = 0
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

}