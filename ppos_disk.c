#include "ppos_disk.h"
#include "disk.h"
#include "ppos.h"
#include "ppos_data.h"
#include "queue.h"
#include "ppos-core-globals.h"


int has_interrupt;

task_t disk_mgr_task;

semaphore_t disk_sem;

task_t *disk_tasks;

task_t *processing;

struct sigaction diskSign;

int diskFree;

request_t *requests;

int TRUE = 1;

int algorithm = FCFS;

int head;
int head_moved;

int biggest;
int smallest;

void hasMoved(int prev, int next)
{
    
    int diff = abs(prev - next);
    head_moved += diff;
}

request_t *fcfs()
{
    return requests;
}

request_t *sstf()
{

    request_t *first = requests;
    request_t *current = requests;
    request_t *chosen = NULL;
    int smaller_diff = -1;

    do
    {

        int diff = abs(current->block - head);
        if (smaller_diff == -1 || diff < smaller_diff)
        {
            smaller_diff = diff;
            chosen = current;
        }
        current = current->next;

    } while (current != first);

    return chosen;
}

request_t *cscan()
{

    request_t *first = requests;
    request_t *current = requests;
    request_t *chosen = NULL;
    int smaller_diff = -1;

    do
    {
        int diff = current->block - head;
        if (diff > 0 && (smaller_diff == -1 || diff < smaller_diff))
        {
            smaller_diff = diff;
            chosen = current;
        }
        current = current->next;

    } while (current != first);

    if (chosen->block >= biggest)
    {
        head = smallest;
    }

    return chosen;
}

// Escolher próximas requisição a ser atendida
request_t *diskScheduler()
{

    request_t *chosen;

    switch (algorithm)
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


    hasMoved(head, chosen->block);
    head = chosen->block;
    return chosen;
}

void handler_disk()
{
    has_interrupt = 1;
}

void diskDriverBody(void *args)
{
    while (TRUE)
    {

        sem_down(&disk_sem);

        // O disk scheduler vai escolher uma requisição para atende-la
        //  = next

        if (has_interrupt)
        {
            has_interrupt = 0;
            // task_resume(processing);
            task_resume(disk_tasks);
            diskFree = 1;
        }

        // ----------------------------------------------

        // se livre e tem request
        if (diskFree && queue_size((queue_t *)requests) > 0)
        {
            request_t *next = diskScheduler();
            processing = next->task;
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
        // task_switch(&taskDisp);
    }
}

int disk_mgr_init(int *numBlocks, int *blockSize)
{

    // Inicia um semáforo de leituras
    if (sem_create(&disk_sem, 1) < 0)
    {
        return -1;
    }

    if (disk_cmd(DISK_CMD_INIT, 0, 0) < 0)
    {
        return -1;
    }

    // Retorna o número de blocos e o tamanho de cada bloco
    *numBlocks = disk_cmd(DISK_CMD_DISKSIZE, 0, 0);
    *blockSize = disk_cmd(DISK_CMD_BLOCKSIZE, 0, 0);

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

    diskSign.sa_handler = handler_disk;
    diskSign.sa_flags = 0;
    if (sigaction(SIGUSR1, &diskSign, 0) < 0)
    {
        perror("Erro em sigaction: ");
        exit(1);
    }

    has_interrupt = 0;
    diskFree = 1;
    requests = NULL;
    disk_tasks = NULL;
    head = 0;
    head_moved = 0;
    processing = NULL;
    biggest = -1;
    smallest = -1;
    return 0;
}

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
    // task_switch(&taskMain);
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