#ifndef MALLOC_H
#define MALLOC_H

#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

void remaining_space_case_malloc(linked_list_t ***vector_ptr,
                                 memory_management *memory_info,
                                 int remaining_space, int remaining_address,
                                 block_info *new_node);

void my_malloc(unsigned int size, linked_list_t ***vector_ptr,
               memory_management *memory_info);

#endif // MALLOC_H
