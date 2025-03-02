#ifndef DESTROY_HEAP_H
#define DESTROY_HEAP_H

#include <stdio.h>
#include <stdlib.h>

#include "../structs.h"

// Funcție care distruge heap-ul și eliberează memoria alocată
void destroy_heap(linked_list_t ***vector_ptr, memory_management **memory_info_ptr);

#endif // HEAP_H