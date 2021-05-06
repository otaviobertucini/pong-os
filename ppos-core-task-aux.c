#include "ppos.h"
#include "ppos-core-globals.h"
#include "ppos-core-task-aux.h"

// ****************************************************************************
// Coloque aqui as suas modificações, p.ex. includes, defines variáveis,
// estruturas e funções

/*
Função que será chamada a cada 1ms.
Caso a tarefa atual seja o dispatcher não faz nada.
Caso não seja e a tarefa já tenha ficado 20ms em execução,
dá o processador para o dispathcer escolher a próxima tarefa.
*/
void handler_tick(int signum)
{
    printf("tick: %d", taskExec->tickcounter);
    systemTime += 1;
    if (taskExec == NULL || taskExec->is_dispatcher)
    {           // ver se é dispatcher via flag
        return; // se for retorna
    }
    taskExec->tickcounter -= 1; // taskExec->contador decrementa
    taskExec->processing_time += 1;
    if (taskExec->tickcounter <= 0 && preemption)
    {
        task_switch(&taskDisp);
    }
    return;
}

/*
Função que será executada quando o dispatcher está em execução.
Enquanto a fila de tarefas não for vazia, chama o escalodador para 
escolher qual a próxima tarefa a ser executada. 
Uma vez escolhida a tarefa, dá 20ms de processador para ela.
*/
// void bodyDispatcher()
// {

//     task_t *next = NULL;
//     while (queue_size((queue_t *)readyQueue) > 1)
//     {
//         next = scheduler();
//         next->tickcounter = 20;
//         if (next != NULL)
//         {
//             task_switch(next);
//         }
//     }
//     if (IS_CONTAB)
//     {
//         printf("Task %d exit: execution time %d ms, processor time %d ms, %d activations\n", taskExec->id + 1, systime() - taskExec->creation_time, taskExec->processing_time, taskExec->activations);
//     }
//     return;
// }

// ****************************************************************************