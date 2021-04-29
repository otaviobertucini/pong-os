#include "ppos_disk.h"
#include "disk.h"
#include "ppos.h"
#include "ppos_data.h"
#include "queue.h"
#include "ppos-core-globals.h"

int has_interrupt;

task_t *disk_mgr_task;

semaphore_t *disk_sem;

void diskDriverBody(void *args)
{

    if (has_interrupt)
    {
        // ...
    }

    has_interrupt = 0;

    //Coloca na fila de dormindo a tarefa corrente -> apend_queue current_task fila_sleep
}

int disk_mgr_init(int *numBlocks, int *blockSize)
{

    // Inicia um semáforo de leituras
    if (sem_create(disk_sem, 1) < 0)
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
    if (task_create(disk_mgr_task, diskDriverBody, NULL) < 0)
    {
        printf("Deu ruim");
        return -1;
    };

    // Cria o disco em si
    if (disk_cmd(DISK_CMD_INIT, 0, 0) < 0)
    {
        return -1;
    }

    has_interrupt = 0;
    return 0;
}

void handler_disk()
{

    // ...

    has_interrupt = 1;

    // task_switch para tarefa disco (diskDriverBody)
}

// leitura de um bloco, do disco para o buffer
int disk_block_read(int block, void *buffer)
{
    // obtém o semáforo de acesso ao disco

    // inclui o pedido na fila_disco
    queue_append((queue_t **)disk_sem->queue, (queue_t *)taskExec);

    // if (gerente de disco está dormindo)
    // {
    //     // acorda o gerente de disco (põe ele na fila de prontas)
    // }

    // libera semáforo de acesso ao disco

    // suspende a tarefa corrente (retorna ao dispatcher)
    return 0;
}

// escrita de um bloco, do buffer para o disco
int disk_block_write(int block, void *buffer) {

    return 0;
}