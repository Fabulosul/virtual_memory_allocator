#ifndef FREE_FUNCTIONS_H
#define FREE_FUNCTIONS_H

#include <stdio.h>
#include <stdlib.h>

#include "../generic_list_functions/generic_list_functions.h"
#include "../structs.h"

void free_no_index_in_list_case(dll_node_t *new_node, int size,
                                memory_management *memory_info,
                                linked_list_t ***vector_ptr);


void my_free(linked_list_t ***vector_ptr, memory_management **memory_info_ptr,
             int address);
             

void free_realloc_vector(int list_node, memory_management *memory_info,
                         linked_list_t ***vector_ptr);

void free_merge_both_lateral_nodes(linked_list_t ***vector_ptr,
                                   memory_management **memory_info_ptr,
                                   int list_left_node, int list_right_node,
                                   unsigned int size, dll_node_t *new_node);

void free_merge_lateral_nodes_case(linked_list_t ***vector_ptr,
                                   memory_management **memory_info_ptr,
                                   int list_left_node, int list_right_node,
                                   unsigned int size, dll_node_t *new_node);

void free_search_node_by_index(linked_list_t ***vector_ptr,
                               memory_management **memory_info_ptr,
                               int *list_left_node_ptr,
                               int *list_right_node_ptr,
                               unsigned int size, dll_node_t **new_node_ptr,
                               dll_node_t **left_node_ptr,
                               dll_node_t **right_node_ptr);

void my_free_1(linked_list_t ***vector_ptr, memory_management **memory_info_ptr,
               int address);

#endif // FREE_FUNCTIONS_H