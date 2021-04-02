#include "ppos.h"
#include "ppos-core-globals.h"
#include "ppos_data.h"

#define IS_PREEMPTIVE 0
#define IS_CONTAB 0

// A tarefa em execução
task_t *current_task;

// Fila que armazenará as tarefas
queue_t *queue;

// O despachante das tarefas
task_t dispatcher;

// Structs de tempo e signals
struct sigaction action;
struct itimerval timer;

// Contador dos ids das tarefas
int id = 0;

// Contador de ticks do relógio passados
unsigned int my_clock = 0;

/*
Função que será chamada a cada 1ms.
Caso a tarefa atual seja o dispatcher não faz nada.
Caso não seja e a tarefa já tenha ficado 20ms em execução,
dá o processador para o dispathcer escolher a próxima tarefa.
*/
void handler_tick(int signum)
{
    my_clock += 1;
    if (current_task == NULL || current_task->is_dispatcher)
    {           // ver se é dispatcher via flag
        return; // se for retorna
    }
    current_task->tickcounter -= 1; // current_task->contador decrementa
    current_task->processing_time += 1;
    if (current_task->tickcounter <= 0 && IS_PREEMPTIVE)
    {
        task_switch(&dispatcher);
    }
    return;
}

/*
Função que será executada quando o dispatcher está em execução.
Enquanto a fila de tarefas não for vazia, chama o escalodador para 
escolher qual a próxima tarefa a ser executada. 
Uma vez escolhida a tarefa, dá 20ms de processador para ela.
*/
void dispatcher_body()
{

    task_t *next = NULL;
    while (queue_size(queue) > 1)
    {
        next = scheduler();
        next->tickcounter = 20;
        if (next != NULL)
        {
            task_switch(next);
        }
    }
    if (IS_CONTAB)
        printf("Task %d exit: execution time %d ms, processor time %d ms, %d activations\n", current_task->id + 1, systime() - current_task->creation_time, current_task->processing_time, current_task->activations);
    return;
}

/*
Função responsável por abstrair a troca de contextos entre tarefas.
Caso nenhuma tarefa tenha sido escolhida para ser executada, chama o 
dispatcher diretamente.
Caso uma tarefa já esteja executando, apenas troca o contexto para o 
escalonador.
*/
void task_yield()
{

    if (current_task == NULL)
    {
        // arma o temporizador ITIM//float mindist = distance(array[pa], array[&pb]);ER_REAL (vide man setitimer)

        if (setitimer(ITIMER_REAL, &timer, 0) < 0)
        {
            perror("Erro em setitimer: ");
            exit(1);
        }
        current_task = &dispatcher;
        dispatcher_body();
    }
    task_switch(&dispatcher);
}

void ppos_init()
{

    setvbuf(stdout, 0, _IONBF, 0);

    queue = NULL;

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

    // Cria a tarefa de dispatcher
    task_create(&dispatcher, dispatcher_body, "Dispatcher");
    dispatcher.is_dispatcher = 1;
    task_setprio(&dispatcher, -20);
    current_task = NULL;
}

/*
Aqui o escalonador vai passar por todas as tarefas e escolher a que tem
a maior prioridade. Em caso de empate, vale a que foi criada primeiro. 
Toda vez que o escalonador for chamado, a tarefa que foi escolhida
terá sua prioridade dinâmica restaurada para a prioridade original
 e as outras terão a prioridade dinâmica decrementada em 1.
*/
task_t *scheduler()
{
    task_t *aux_ini = &dispatcher;
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
        if (aux_current == &dispatcher)
        {
            aux_current = aux_current->next;
            continue; //percorre a fila
        }
        aux_current->dinamic_prio -= 1; //atualiza a prioridade dinâmica de cada tarefa
        aux_current = aux_current->next;
    }

    next->dinamic_prio = next->prio; //reinicia a prioridade dinâmica

    return next;
}

/*
Função chamada para setar a prioridade das tarefas.
Caso |prio| > 20 o valor será setado para 20.
*/
void task_setprio(task_t *task, int prio)
{
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
        task = current_task;
    return task->prio;
}

/*
Muda o contexto da tarefa atual para a tarefa passa por parâmetro.
*/
int task_switch(task_t *task)
{
    if (task == NULL)
        return -1;
    else
    {
        /* salva o estado da tarefa atual e troca para a tarefa recebida */
        task_t *aux = current_task;
        current_task = task;
        // incrementa o número de vezes que foi dada ao processador
        current_task->activations += 1;
        swapcontext(&aux->context, &current_task->context);
    }
    return 0; //se deu tudo certo
}

/*
Função chamada no final de cada tarefa.
Apenas remove ela da fila de tarefas e dá a execução ao dispatcher.
*/
void task_exit(int exitCode)
{
    if (IS_CONTAB)
        printf("Task %d exit: execution time %d ms, processor time %d ms, %d activations\n", current_task->id + 1, systime() - current_task->creation_time, current_task->processing_time, current_task->activations);
    //queue_remove((queue_t **)&dispatcher, (queue_t *)current_task);
    queue_remove(&queue, (queue_t *)current_task);
    task_yield();
    return;
}

// Retorna o valor do contador de ticks
unsigned int systime()
{
    return my_clock;
}

/*
Função chamada para criar tarefas.
Seta todas as propriedades com valores padrão e adiciona na fila.
*/
int task_create(task_t *task,
                void (*start_func)(void *),
                void *arg)
{
    task->prio = 0; //prioridade default = 0
    task->dinamic_prio = 0;
    task->is_dispatcher = 0;
    task->id = id;
    task->position = id;
    task->tickcounter = 0;
    task->processing_time = 0;
    task->activations = 0;
    task->creation_time = systime();
    id = id + 1;

    getcontext(&task->context);

    char *stack = malloc(STACKSIZE);
    if (stack)
    { // se for retorna
        task->context.uc_stack.ss_sp = stack;
        task->context.uc_stack.ss_size = STACKSIZE;
        task->context.uc_stack.ss_flags = 0;
        task->context.uc_link = 0;
    }
    else
    {
        perror("Erro na criação da pilha: ");
        exit(1);
    }

    makecontext(&task->context, (void *)(start_func), 1, arg);

    queue_append(&queue, (queue_t *)task);

    return 0;
}