#include "ppos_disk.h"

int disk_mgr_init(int *numBlocks, int *blockSize) {}

// leitura de um bloco, do disco para o buffer
int disk_block_read(int block, void *buffer) {}

// escrita de um bloco, do buffer para o disco
int disk_block_write(int block, void *buffer) {}