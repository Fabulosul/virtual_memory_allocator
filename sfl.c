// Tudor Robert-Fabian Anul 2023-2024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "structs.h"
#include "generic_list_functions/generic_list_functions.h"
#include "init_heap/init_heap.h"
#include "destroy_heap/destroy_heap.h"
#include "malloc/malloc.h"
#include "free_functions/free_functions.h"
#include "write/write.h"
#include "read/read.h"
#include "helper_functions/helper_functions.h"
#include "dump_memory/dump_memory.h"


// În main apelăm funcția corespunzătoare după comanda primită.
// De asemenea, tot aici facem transformările și parsările necesare
// pentru a face apelul funcțiilor
int main(void)
{
	memory_management *memory_info = calloc(1, sizeof(memory_management));
	if (!memory_info) {
		free(memory_info);
		printf("Malloc failed for memory_info\n");
	}
	linked_list_t **vector = NULL;
	while (1) {
		char command[16], address[20];
		unsigned int nr_lists;
		int nr_bytes_per_list, reconstruction_type, decimal_address;
		int nr_bytes;
		scanf("%s", command);
		if (strcmp(command, "INIT_HEAP") == 0) {
			scanf("%s", address);
			scanf("%u%d%d", &nr_lists, &nr_bytes_per_list,
				  &reconstruction_type);
			decimal_address = convert_hexa_to_decimal(address);
			vector = init_heap(decimal_address, nr_lists, nr_bytes_per_list,
							   memory_info);
		}

		if (strcmp(command, "MALLOC") == 0) {
			scanf("%d", &nr_bytes);
			my_malloc(nr_bytes, (&vector), memory_info);
		}

		if (strcmp(command, "FREE") == 0) {
			scanf("%s", address);
			decimal_address = convert_hexa_to_decimal(address);

			if (reconstruction_type == 0)
				my_free(&vector, &memory_info, decimal_address);
			else
				my_free_1(&vector, &memory_info, decimal_address);
		}

		if (strcmp(command, "READ") == 0) {
			scanf("%s", address);
			scanf("%d", &nr_bytes);
			decimal_address = convert_hexa_to_decimal(address);
			if (check_write_read(decimal_address, nr_bytes, memory_info) == 0) {
				printf("Segmentation fault (core dumped)\n");
				dump_memory(vector, memory_info);
				destroy_heap(&vector, &memory_info);
				return 0;
			}
			read(decimal_address, nr_bytes, memory_info, vector);
		}

		if (strcmp(command, "WRITE") == 0) {
			char *data;
			char command2[MAX_LENGTH];
			scanf("%s", address);
			fgets(command2, sizeof(command2), stdin);
			parse_write_command(command2, &data, &nr_bytes);
			decimal_address = convert_hexa_to_decimal(address);
			if (nr_bytes > (int)strlen(data))
				nr_bytes = strlen(data);
			if (check_write_read(decimal_address, nr_bytes, memory_info) == 0) {
				printf("Segmentation fault (core dumped)\n");
				dump_memory(vector, memory_info);
				destroy_heap(&vector, &memory_info);
				free(data);
				return 0;
			}
			write(decimal_address, data, nr_bytes, memory_info, vector);
			free(data);
		}

		if (strcmp(command, "DUMP_MEMORY") == 0)
			dump_memory(vector, memory_info);

		if (strcmp(command, "DESTROY_HEAP") == 0) {
			destroy_heap(&vector, &memory_info);
			return 0;
		}
	}

	return 0;
}
