#include "ppos_disk.h"
#include "disk.h"
#include "ppos.h"
#include "ppos_data.h"
#include "queue.h"
#include "ppos-core-globals.h"

#define FCFS 1
#define SSTF 2
#define CSCAN 3

#define READ 'r'
#define WRITE 'w'

typedef struct request_t {

    struct request_t *prev;	
    struct request_t *next;	
    unsigned char type;
    unsigned int createdAt;

    int block;
    void *buffer;

    task_t *task;

} request_t;

/* hardcoded disk_t */
int has_interrupt;      // Flag para registrar se há interrupção

task_t disk_mgr_task;   //Tarefa gerenciadora

semaphore_t disk_sem;   //Semáforo do disco

task_t* disk_tasks;     //Fila de tarefas

struct sigaction diskSign; //Struct para captura de sinais de interrupç]ap

int diskFree;           //Flag para verificar se o disco está livre

request_t *requests;    //Fila de requisições

/* Demais variávei globais */
int TRUE = 1;

int algorithm = FCFS;  //Define o algoritmo que será utilizado pelo escalonador

int head;               //Utilizado para o registro da posição atual da cabeça de leitura
int head_moved;         //Utilizado para o registro da quantidade de blocos que a cabeça de leitura moveu 

int biggest;            //Guarda a requisição com posição de leitura mais ao final do disco
int smallest;           //Guarda a requisição com posição de leitura mais ao incício do disco

int endDisk = 0;        //Flag para vericar se já chegou ao final do disco, ou à última requisição mais próxima do final do disco

/* Função que calcula a movimentação da cabeça de leitura do disco */
void hasMoved(int prev, int next)
{
    int diff = abs(prev - next); 
    head_moved += diff;
}

/* Função que define o modelo de escalonamento First Come First Served - 
Primeiro a Chegar Primeiro a ser Atendido - em que a primeira requisição 
da fila será atendida independedente da posição em quem se encontre a 
cabeça de leitura do disco. Retorna o ponteiro que aponta para a primeira 
requisição da fila de requisição.*/
request_t *fcfs()
{
    return requests;
}

/* Funçao que define o modelo de escalonamento Shortest Seek Time First - 
Menor Tempo de Busca Primeiro - em que a requisição atendida será aquela
que estiver mais próxia da caceça de leitura do disco, busca reduzir os
movimentos da cabeça de leitura. */
request_t *sstf()
{

    request_t *first = requests;
    request_t *current = requests;
    request_t *chosen = current;
    int smaller_diff = -1;

    do
    {

        int diff = abs(current->block - head);          //calcula a diferença absoluta(módulo) entre a posição de leitura atual(head)
        if (smaller_diff == -1 || diff < smaller_diff)  //e a posição a ser lida. Percorre a fila de requisições 
        {                                               //e define qual é a requisição com menor distância a ser atendida
            smaller_diff = diff;                        
            chosen = current;
        }
        current = current->next;            //percorre a fila de requisições

    } while (current != first);

    return chosen;                          //retorna a requisição escolhida
}

/* Funçao que define o modelo de escalonamento circular Scan em que a cabeça
de leitura percorre o disco em um único sentido, ao chegar na última requisição
mais ao final do disco, a cabeça de leitura retorna para atender a requisição
que estiver mais ao início do disco.   */
request_t *cscan()
{

    request_t *first = requests;
    request_t *current = requests;
    request_t *chosen = current;
    int smaller_diff = -1;

    do
    {
        int diff = current->block - head;                               //calcula a diferença entre a posição de leitura atual(head)
        if (diff > 0 && (smaller_diff == -1 || diff < smaller_diff))    //e a posição a ser lida, considerando apenas distâncias positivas,
        {                                                               //apenas em uma direção. Percorre a fila de requisições
            smaller_diff = diff;                                        //e define qual é a requisição com menor distância a ser atendida
            chosen = current;
        }
        current = current->next;

    } while (current != first);
    
    endDisk = 0;
    
    if (chosen->block >= biggest)
    {
        head = smallest;
        endDisk = 1;
    }

    return chosen;
}

/* Escalonador escolhe a próxima requisição a ser atendida, dependendo do tipo
de escalonamendo definido na variável global algorithm. Pode escolher entre 
First Come First Served, Shortest Seek Time First e Circular Scan */
request_t *diskScheduler()
{

    request_t *chosen;

    switch (algorithm)          //Decide qual algoritmo será utilizado de acordo com a variável globar algorithm
    {
    case FCFS:
        chosen = fcfs();
        break;
    case SSTF:
        chosen = sstf();
        break;
    case CSCAN:
        chosen = cscan();
        break;
    }


    hasMoved(head, chosen->block);          //atualiza em 'head_moved', através da função hasMoved, a movimentação da cabeça de leitura
    if(!endDisk)head = chosen->block;       //atualiza a posição da cabeça de leitura para a posição do bloco da requisição escolhida
                                            //quando 'endDisk == 1' a cabeça é atualizada para a mais próxima do início do disco,
                                            //dentro da função CSCAN.
    return chosen;
}

/* Tratador de interrupções */
void handler_disk()
{
    has_interrupt = 1;
}

/* Corpo do disco */
void diskDriverBody(void *args)
{
    while (TRUE)
    {
        sem_down(&disk_sem);

        // O disk scheduler vai escolher uma requisição para atende-la
        //  = next

        /* Se há interrupção resume a tarefa da fila de tarefas */
        if (has_interrupt)
        {
            has_interrupt = 0;
            task_resume(disk_tasks);
            diskFree = 1;   //Libera o disco
        }

        // ----------------------------------------------

        // se livre e tem request
        if (diskFree && queue_size((queue_t *)requests) > 0)
        {
            request_t *next = diskScheduler();
            // pega a primeira da fila de requisicoes (ver se está certo)
            next = (request_t *)queue_remove((queue_t **)&requests, (queue_t *)next);
            if (next->type == READ)
            {
                disk_cmd(DISK_CMD_READ, next->block, next->buffer);
            }
            else
            {
                disk_cmd(DISK_CMD_WRITE, next->block, next->buffer);
            }
            diskFree = 0;
            free(next);
        }

        sem_up(&disk_sem);
        // volta para o dispatcher (ver se está certo)
        task_yield();
    }
    printf("A CABEÇA DO DISCO MOVEU %d BLOCOS", head_moved); //não está chegando aqui porque o loop nunca termina
}

/* Inicializa o gerenciador de disco */
int disk_mgr_init(int *numBlocks, int *blockSize)
{

    // Inicia um semáforo de leituras
    if (sem_create(&disk_sem, 1) < 0)
    {
        return -1;
    }
    //inicializa o disco
    if (disk_cmd(DISK_CMD_INIT, 0, 0) < 0)
    {
        return -1;
    }

    // Retorna o número de blocos e o tamanho de cada bloco
    *numBlocks = disk_cmd(DISK_CMD_DISKSIZE, 0, 0);
    *blockSize = disk_cmd(DISK_CMD_BLOCKSIZE, 0, 0);

    //verifica erros com o número de blocos e tamanho de cada bloco
    if (*numBlocks < 0 || *blockSize < 0)
    {
        return -1;
    }

    // Cria a tarefa de disco
    if (task_create(&disk_mgr_task, diskDriverBody, NULL) < 0)
    {
        printf("Deu ruim");
        return -1;
    };

    // Define sinais de tratamento de interrção
    diskSign.sa_handler = handler_disk;
    diskSign.sa_flags = 0;
    if (sigaction(SIGUSR1, &diskSign, 0) < 0)
    {
        perror("Erro em sigaction: ");
        exit(1);
    }

    //inicializa variáveis
    has_interrupt = 0;
    diskFree = 1;
    requests = NULL;
    disk_tasks = NULL;
    head = 0;
    head_moved = 0;
    biggest = -1;
    smallest = -1;
    return 0;
}

/* Encapsula funções bloqueantes de requisições de leitura e escrita */
int process_request(unsigned char type, int block, void *buffer)
{

    if (biggest == -1 || block > biggest)
    {
        biggest = block;
    }

    if (smallest == -1 || block < smallest)
    {
        smallest = block;
    }

    // obtém o semáforo de acesso ao disco
    sem_down(&disk_sem);

    // cria a request
    request_t *request;
    request = malloc(sizeof(request_t));
    request->type = type;
    request->block = block;
    request->buffer = buffer;
    request->createdAt = systime();
    request->task = taskExec;
    request->next = NULL;
    request->prev = NULL;

    queue_append((queue_t **)&requests, (queue_t *)request);

    if (disk_mgr_task.state == PPOS_TASK_STATE_SUSPENDED)
    {
        task_resume(&disk_mgr_task);
    }
    // libera semáforo de acesso ao disco
    sem_up(&disk_sem);
    // suspende a tarefa corrente (retorna ao dispatcher)
    task_suspend(taskExec, &disk_tasks);
    task_yield();
    return 0;
}

// leitura de um bloco, do disco para o buffer
int disk_block_read(int block, void *buffer)
{

    process_request(READ, block, buffer);

    return 0;
}

// escrita de um bloco, do buffer para o disco
int disk_block_write(int block, void *buffer)
{

    process_request(WRITE, block, buffer);

    return 0;
}

/* Retorna a quantidade de blocos de movimentação da cabeça de leitura do disco.
Utilizada para testes nas funções main */
int get_head(){
  return head_moved;
}

/* int before_barrier_create(barrier_t *b, int N) {}
int after_barrier_create(barrier_t *b, int N) {}

int before_barrier_join(barrier_t *b) {}
int after_barrier_join(barrier_t *b) {}

int before_barrier_destroy(barrier_t *b) {}
int after_barrier_destroy(barrier_t *b) {}

int before_mutex_create(mutex_t *m) {}
int after_mutex_create(mutex_t *m) {}

int before_mutex_lock(mutex_t *m) {}
int after_mutex_lock(mutex_t *m) {}

int before_mutex_unlock(mutex_t *m) {}
int after_mutex_unlock(mutex_t *m) {}

int before_mutex_destroy(mutex_t *m) {}
int after_mutex_destroy(mutex_t *m) {}

int before_sem_create(semaphore_t *s, int value) {}
int after_sem_create(semaphore_t *s, int value) {}

int before_sem_down(semaphore_t *s) {}
int after_sem_down(semaphore_t *s) {}

int before_sem_up(semaphore_t *s) {}
int after_sem_up(semaphore_t *s) {}

int before_sem_destroy(semaphore_t *s) {}
int after_sem_destroy(semaphore_t *s) {} */