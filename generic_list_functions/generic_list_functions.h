#ifndef GENERIC_LIST_FUNCTIONS_H
#define GENERIC_LIST_FUNCTIONS_H

#include <stdlib.h>
#include <stdio.h>

#include "../structs.h"


dll_node_t *dll_allocate_nth_node(linked_list_t *list, unsigned int nr, int current_address, int index);
dll_node_t *dll_remove_nth_node(linked_list_t *list, unsigned int n);
dll_node_t *dll_remove_node(linked_list_t *list, int address);

#endif // NODE_MANAGEMENT_H
