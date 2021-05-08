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

struct sigaction diskSign;

int diskFree;

request_t *requests;

// Escolher próximas requisição a ser atendida
// request_t* diskScheduler(){
//     // return taskMain
// }

void handler_disk()
{

    // ...
    printf("iashduashdasu\n");

    has_interrupt = 1;

    // task_switch para tarefa disco (diskDriverBody)
}

void diskDriverBody(void *args)
{
    request_t *next;
    while (1)
    {

        sem_down(&disk_sem);

        // printf("diskDriver\n");

        // O disk scheduler vai escolher uma requisição para
        // atende-la = next

        if (has_interrupt)
        {
            has_interrupt = 0;

            // task_resume(&next->task);
            task_resume(&taskMain);
            diskFree = 1;
            // ...
        }

        if (diskFree && queue_size((queue_t *)requests) > 0) // && queue_size((queue_t *)requests) > 0)
        {

            next = (request_t *)queue_remove((queue_t **)&requests, (queue_t *)requests);

            disk_cmd(DISK_CMD_READ, next->block, next->buffer);
            diskFree = 0;
            free(next);
        }

        // queue_remove((queue_t **)&suspended, (queue_t *)&taskMain);

        sem_up(&disk_sem);

        task_yield();
        //Coloca na fila de dormindo a tarefa corrente -> apend_queue current_task fila_sleep
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
    sigemptyset(&diskSign.sa_mask);
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
    return 0;
}

// leitura de um bloco, do disco para o buffer
int disk_block_read(int block, void *buffer)
{
    // obtém o semáforo de acesso ao disco
    sem_down(&disk_sem);

    request_t *request;
    request = malloc(sizeof(request_t));
    request->type = 'r';
    request->block = block;
    request->buffer = buffer;
    request->createdAt = systime();
    request->task = taskExec;
    request->next = NULL;
    request->prev = NULL;

    // inclui o pedido na fila_disco
    if (requests == NULL)
    {

        printf("tamanhia :\n");
    }
    queue_append((queue_t **)&requests, (queue_t *)request);

    if (disk_mgr_task.state == PPOS_TASK_STATE_SUSPENDED)
    {
        task_resume(&disk_mgr_task);
        // queue_remove((queue_t **)&suspended, (queue_t *)&disk_mgr_task);
    }

    // libera semáforo de acesso ao disco
    sem_up(&disk_sem);

    // suspende a tarefa corrente (retorna ao dispatcher)
    // if (taskExec == NULL)
    // {
    //     printf("morri aqui\n");
    // }

    task_suspend(taskExec, &disk_tasks);
    printf("tamanhia 2:\n");
    // printf("vazei create read\n");
    task_yield();
    // task_switch(&taskMain);

    return 0;
}

// escrita de um bloco, do buffer para o disco
int disk_block_write(int block, void *buffer)
{

    //int disk_cmd (DISK_CMD_WRITE, int block, void *buffer) ;
    return 0;
}

int before_barrier_create(barrier_t *b, int N) {}
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
int after_sem_destroy(semaphore_t *s) {}