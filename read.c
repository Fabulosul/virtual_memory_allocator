#include "read.h"
#include "dump_memory.h"

// Funcție care citește nr_bytes de la address din memoria alocată
void read(int address, int nr_bytes, memory_management *memory_info,
		  linked_list_t **vector)
{
	block_info *current_node = memory_info->allocated_blocks;
	int ok = 0;
	for (unsigned int i = 0; i < memory_info->nr_allocated_blocks; i++) {
		if ((((dll_block_data *)current_node->data)->address <= address &&
			 ((((dll_block_data *)current_node->data)->address +
			 ((dll_block_data *)current_node->data)->size - 1) >= address))) {
			ok = 1;
			break;
		}
		current_node = current_node->next;
	}
	if (ok == 1) {
		int difference =
		((dll_block_data *)current_node->data)->address - address;
		int remaining_size_first_block =
		((dll_block_data *)current_node->data)->size - difference;
		int remaining_bytes = nr_bytes - remaining_size_first_block;
		int current_address =
		((dll_block_data *)current_node->data)->address +
		((dll_block_data *)current_node->data)->size;
		if (remaining_bytes <= 0) {
			for (int i = 0; i < nr_bytes; i++)
				printf("%c", *((char *)(((dll_block_data *)
					   (current_node->data))->value + difference + i)));
			printf("\n");
			return;
		}
		for (int i = 0; i < remaining_size_first_block; i++)
			printf("%c", *((char *)(((dll_block_data *)
					(current_node->data))->value + difference + i)));
		current_node = current_node->next;
		while (nr_bytes != 0 && current_node) {
			if (current_address !=
				((dll_block_data *)current_node->data)->address) {
				printf("Segmentation fault (core dumped)\n");
				dump_memory(vector, memory_info);
				return;
			}
			remaining_bytes -= ((dll_block_data *)current_node->data)->size;
			if (remaining_bytes <= 0) {
				for (int i = 0; i < remaining_bytes +
					 ((dll_block_data *)current_node->data)->size; i++)
					printf("%c", *((char *)(((dll_block_data *)
						   current_node->data)->value + i)));
				printf("\n");
				return;
			}
			for (int i = 0; i < ((dll_block_data *)current_node->data)->size;
				 i++)
				printf("%c", *((char *)(((dll_block_data *)
					   (current_node->data))->value + i)));
			current_address =
			((dll_block_data *)current_node->data)->address +
			((dll_block_data *)current_node->data)->size;
			current_node = current_node->next;
		}
		if (nr_bytes < 0) {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(vector, memory_info);
		}
		printf("\n");
	} else {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(vector, memory_info);
	}
}