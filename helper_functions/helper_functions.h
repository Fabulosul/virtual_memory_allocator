#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

#include "../structs.h"

block_info *remove_node_block_info(memory_management *memory_info, block_info *current_node);

void insert_node_ordonated_vector(linked_list_t *list, dll_node_t *new_node);

void insert_element_ordonated_allocated_blocks(memory_management *memory_info, block_info *new_node);

int convert_hexa_to_decimal(char hexnumber[]);

int check_write_read(int address, int nr_bytes, memory_management *memory_info);

void parse_write_command(const char *command, char **string, int *number);

#endif // HELPER_FUNCTIONS_H
