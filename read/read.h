#ifndef READ_H
#define READ_H

#include "../structs.h"


// Funcție care citește din memorie
void read(int address, int nr_bytes, memory_management *memory_info,
          linked_list_t **vector);

#endif // READ_H
