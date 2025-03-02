#include "write.h"
#include "../dump_memory/dump_memory.h"

// Funcție care scrie nr_bytes la address în memoria alocată
void write(int address, char data_parsed[], int nr_bytes,
		   memory_management *memory_info, linked_list_t **vector)
{
	char *data_ptr = data_parsed;
	int length = strlen(data_parsed);
	if (length < nr_bytes)
		nr_bytes = length;
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
			memcpy(((dll_block_data *)current_node->data)->value +
				   difference, data_ptr, nr_bytes);
			return;
		}
		memcpy(((dll_block_data *)current_node->data)->value +
			   difference, data_ptr, remaining_size_first_block);
		data_ptr = data_ptr + remaining_size_first_block;
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
				memcpy(((dll_block_data *)current_node->data)->value, data_ptr,
					   remaining_bytes +
					   ((dll_block_data *)current_node->data)->size);
				return;
			}
			memcpy(((dll_block_data *)current_node->data)->value, data_ptr,
				   ((dll_block_data *)current_node->data)->size);
			data_ptr = data_ptr +
			((dll_block_data *)current_node->data)->size;
			current_address = ((dll_block_data *)current_node->data)->address +
			((dll_block_data *)current_node->data)->size;
			current_node = current_node->next;
		}
		if (nr_bytes < 0) {
			printf("Segmentation fault (core dumped)\n");
			dump_memory(vector, memory_info);
		}
	} else {
		printf("Segmentation fault (core dumped)\n");
		dump_memory(vector, memory_info);
	}
}