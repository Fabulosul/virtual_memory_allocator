#ifndef MEMORY_DUMP_H
#define MEMORY_DUMP_H

#include <stdio.h>
#include <stdlib.h>

#include "../structs.h"

// Funcție care afișează memoria alocată
void dump_memory(linked_list_t **vector, memory_management *memory_info);

#endif