#include "dump_memory.h"

// Funcție care afișează infomațiile cerute pentru comanda DUMP_MEMORY
void dump_memory(linked_list_t **vector, memory_management *memory_info)
{
	memory_info->nr_free_blocks = 0;
	printf("+++++DUMP+++++\n");
	printf("Total memory: %u bytes\n", memory_info->total_memory);
	printf("Total allocated memory: %u bytes\n",
		   memory_info->total_allocated_memory);
	printf("Total free memory: %u bytes\n", memory_info->total_memory -
		   memory_info->total_allocated_memory);
	for (unsigned int i = 0; i < memory_info->nr_lists; i++)
		memory_info->nr_free_blocks += vector[i]->nr_nodes;
	printf("Free blocks: %u\n", memory_info->nr_free_blocks);
	printf("Number of allocated blocks: %u\n",
		   memory_info->nr_allocated_blocks);
	printf("Number of malloc calls: %u\n", memory_info->nr_malloc_calls);
	printf("Number of fragmentations: %u\n", memory_info->nr_fragmentations);
	printf("Number of free calls: %u\n", memory_info->nr_free_calls);
	for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
		printf("Blocks with %u bytes - %u free block(s) :",
			   vector[i]->data_size, vector[i]->nr_nodes);
		dll_node_t *current_node = vector[i]->head;
		for (unsigned int j = 0; j < vector[i]->nr_nodes; j++) {
			printf(" 0x%x", ((dll_node_info *)(current_node->data))->address);
			current_node = current_node->next;
		}
		printf("\n");
	}

	block_info *current = memory_info->allocated_blocks;
	printf("Allocated blocks :");
	for (unsigned int i = 0; i < memory_info->nr_allocated_blocks; i++) {
		printf(" (0x%x - %u)", ((dll_block_data *)(current->data))->address,
			   ((dll_block_data *)(current->data))->size);
		current = current->next;
	}
	printf("\n");
	printf("-----DUMP-----\n");
}