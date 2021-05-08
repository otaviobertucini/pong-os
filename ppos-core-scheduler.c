#include "ppos.h"
#include "ppos-core-globals.h"
#include "ppos_data.h"
#include "ppos-core-task-aux.h"

// A tarefa em execução
task_t *taskExec;

// Fila que armazenará as tarefas
task_t *readyQueue;

// O despachante das tarefas
task_t taskDisp;

// Structs de tempo e signals
struct sigaction action;
struct itimerval timer;

// Contador dos ids das tarefas
long nextid = 0;

// Contador de ticks do relógio passados
unsigned int systemTime;

unsigned char preemption = 1;

// Structs de tempo e signals
/* struct sigaction action;
struct itimerval timer;
 */
/*
Função responsável por abstrair a troca de contextos entre tarefas.
Caso nenhuma tarefa tenha sido escolhida para ser executada, chama o 
dispatcher diretamente.
Caso uma tarefa já esteja executando, apenas troca o contexto para o 
escalonador.
*/
// void task_yield()
// {

//     if (taskExec == NULL)
//     {
//         // arma o temporizador ITIM//float mindist = distance(array[pa], array[&pb]);ER_REAL (vide man setitimer)

//         if (setitimer(ITIMER_REAL, &timer, 0) < 0)
//         {
//             perror("Erro em setitimer: ");
//             exit(1);
//         }
//         taskExec = &taskDisp;
//         bodyDispatcher();
//     }
//     task_switch(&taskDisp);
// }

// void ppos_init()
// {

//     setvbuf(stdout, 0, _IONBF, 0);

//     readyQueue = NULL;

//     nextid = 0;
//     systemTime = 0;

//     action.sa_handler = handler_tick;
//     sigemptyset(&action.sa_mask);
//     action.sa_flags = 0;
//     if (sigaction(SIGALRM, &action, 0) < 0)
//     {
//         perror("Erro em sigaction: ");
//         exit(1);
//     }

//     // ajusta valores do temporizador
//     timer.it_value.tv_usec = 1000;    // primeiro disparo, em micro-segundos
//     timer.it_value.tv_sec = 0;        // primeiro disparo, em segundos
//     timer.it_interval.tv_usec = 1000; // disparos subsequentes, em micro-segundos
//     timer.it_interval.tv_sec = 0;     // disparos subsequentes, em segundos

//     // Cria a tarefa de dispatcher
//     task_create(&taskDisp, bodyDispatcher, "Dispatcher");
//     taskDisp.is_dispatcher = 1;
//     task_setprio(&taskDisp, -20);
//     taskExec = NULL;
// }

/*
Aqui o escalonador vai passar por todas as tarefas e escolher a que tem
a maior prioridade. Em caso de empate, vale a que foi criada primeiro. 
Toda vez que o escalonador for chamado, a tarefa que foi escolhida
terá sua prioridade dinâmica restaurada para a prioridade original
 e as outras terão a prioridade dinâmica decrementada em 1.
*/
task_t *scheduler()
{

    // printf("entrei %d\n");
    task_t *aux_ini = &taskDisp;
    task_t *next = aux_ini->next;
    task_t *aux_current = next->next;

    while (aux_current != aux_ini)
    {
        if (aux_current->dinamic_prio < next->dinamic_prio)
        {
            next = aux_current; //armazena a maior prioridade
        }
        aux_current = aux_current->next; //percorre a fila
    }
    // }

    aux_current = aux_ini->next; //aponta novamente à primeira tarefa da fila

    //task aging com 'α = -1'
    while (aux_current != aux_ini)
    {
        if (aux_current == &taskDisp)
        {
            aux_current = aux_current->next;
            continue; //percorre a fila
        }
        aux_current->dinamic_prio -= 1; //atualiza a prioridade dinâmica de cada tarefa
        aux_current = aux_current->next;
    }

    next->dinamic_prio = next->prio; //reinicia a prioridade dinâmica
    // printf("shchehuua (prioridade %d)\n", task_getprio(next->next->next));

    // printf("entrei sched\n");
    // print_tcb(next);
    // printf("size: %d", queue_size(readyQueue));
    return next;
}

/*
Função chamada para setar a prioridade das tarefas.
Caso |prio| > 20 o valor será setado para 20.
*/
void task_setprio(task_t *task, int prio)
{
    if (task == NULL)
        task = taskExec;
    if (prio < -20)
        task->prio = -20;
    if (prio > 20)
        task->prio = 20;
    task->prio = prio;
    task->dinamic_prio = prio;
}

// retorna a prioridade estática de uma tarefa (ou a tarefa atual)
int task_getprio(task_t *task)
{
    if (task == NULL)
        task = taskExec;
    return task->prio;
}

/*
Muda o contexto da tarefa atual para a tarefa passa por parâmetro.
*/
// int task_switch(task_t *task)
// {
//     if (task == NULL)
//         return -1;
//     else
//     {
//         /* salva o estado da tarefa atual e troca para a tarefa recebida */
//         task_t *aux = taskExec;
//         taskExec = task;
//         // incrementa o número de vezes que foi dada ao processador
//         taskExec->activations += 1;
//         swapcontext(&aux->context, &taskExec->context);
//     }
//     return 0; //se deu tudo certo
// }

/*
Função chamada no final de cada tarefa.
Apenas remove ela da fila de tarefas e dá a execução ao dispatcher.
*/
// void task_exit(int exitCode)
// {
//     if (IS_CONTAB)
//     {
//         printf("Task %d exit: execution time %d ms, processor time %d ms, %d activations\n", taskExec->id + 1, systime() - taskExec->creation_time, taskExec->processing_time, taskExec->activations);
//     }
//     queue_remove((queue_t **)&readyQueue, (queue_t *)taskExec);
//     task_yield();
//     return;
// }

// Retorna o valor do contador de ticks
unsigned int systime()
{
    return systemTime;
}

/*
Função chamada para criar tarefas.
Seta todas as propriedades com valores padrão e adiciona na fila.
*/
// int task_create(task_t *task,
//                 void (*start_func)(void *),
//                 void *arg)
// {

//     task->prio = 0; //prioridade default = 0
//     task->dinamic_prio = 0;
//     task->is_dispatcher = 0;
//     task->id = nextid;
//     task->position = nextid;
//     task->tickcounter = 0;
//     task->processing_time = 0;
//     task->activations = 0;
//     task->creation_time = systime();
//     nextid = nextid + 1;

//     getcontext(&task->context);

//     char *stack = malloc(STACKSIZE);
//     if (stack)
//     { // se for retorna
//         task->context.uc_stack.ss_sp = stack;
//         task->context.uc_stack.ss_size = STACKSIZE;
//         task->context.uc_stack.ss_flags = 0;
//         task->context.uc_link = 0;
//     }
//     else
//     {
//         perror("Erro na criação da pilha: ");
//         exit(1);
//     }

//     makecontext(&task->context, (void *)(start_func), 1, arg);

//     queue_append((queue_t **)&readyQueue, (queue_t *)task);

//     return 0;
// }

void before_task_exit() {}
void after_task_exit() {}

void before_task_create(task_t *task)
{
    task->prio = 0; //prioridade default = 0
    task->dinamic_prio = 0;
    task->is_dispatcher = 0;
    task->id = nextid;
    task->position = nextid;
    task->tickcounter = 0;
    task->processing_time = 0;
    task->activations = 0;
    task->creation_time = systime();
    nextid = nextid + 1;
}

void after_task_create(task_t *task) {}

void before_task_switch(task_t *task)
{
    task->tickcounter = 20;
    // preemption = 1;
}
void after_task_switch(task_t *task) {}

void before_task_resume(task_t *task) {}
void after_task_resume(task_t *task)
{
    // printf("entrei after");
    // queue_remove((queue_t **)&)
}

void before_task_suspend(task_t *task) {}
void after_task_suspend(task_t *task) {}

void before_task_sleep() {}
void after_task_sleep() {}

void before_ppos_init()
{
    setvbuf(stdout, 0, _IONBF, 0);
}
void after_ppos_init()
{
    // PPOS_PREEMPT_DISABLE
    // preemption = 1;
    action.sa_handler = handler_tick;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    if (sigaction(SIGALRM, &action, 0) < 0)
    {
        perror("Erro em sigaction: ");
        exit(1);
    }

    // ajusta valores do temporizador
    timer.it_value.tv_usec = 1000;    // primeiro disparo, em micro-segundos
    timer.it_value.tv_sec = 0;        // primeiro disparo, em segundos
    timer.it_interval.tv_usec = 1000; // disparos subsequentes, em micro-segundos
    timer.it_interval.tv_sec = 0;     // disparos subsequentes, em segundos

    // printf("entrei rsrs %d", preemption);
    taskDisp.is_dispatcher = 1;
    task_setprio(&taskDisp, -20);
    // queue_append((queue_t **)&readyQueue, (queue_t *)&taskMain);
    queue_append((queue_t **)&readyQueue, (queue_t *)&taskDisp);
    // preemption = 1;
}

void before_task_yield()
{
}
void after_task_yield()
{
    printf("oieee\n");
    /*     if (taskExec == NULL)
    {
        // arma o temporizador ITIM//float mindist = distance(array[pa], array[&pb]);ER_REAL (vide man setitimer)

        if (setitimer(ITIMER_REAL, &timer, 0) < 0)
        {
            perror("Erro em setitimer: ");
            exit(1);
        }
        taskExec = &taskDisp;
        bodyDispatcher();
    }
    task_switch(&taskDisp); */
}

int before_task_join(task_t *task) {}
int after_task_join(task_t *task) {}