#ifndef INIT_HEAP_H
#define INIT_HEAP_H

#include <stdio.h>
#include <stdlib.h>
#include "structs.h"

// Funcție care inițializează heap-ul
linked_list_t **init_heap(int start_address, unsigned int nr_lists,
                          int nr_bytes_per_list, memory_management *memory_info);

#endif