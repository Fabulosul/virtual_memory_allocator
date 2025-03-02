#ifndef WRITE_H
#define WRITE_H


#include "../structs.h"


// Function prototype
void write(int address, char data_parsed[], int nr_bytes,
           memory_management *memory_info, linked_list_t **vector);

#endif // WRITE_H