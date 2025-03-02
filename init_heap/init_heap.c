#include "../init_heap/init_heap.h"
#include "../generic_list_functions/generic_list_functions.h"

// Funcție care alocă memorie pentru vectorul de liste dublu înlănțuite
linked_list_t **init_heap(int start_address, unsigned int nr_lists,
						  int nr_bytes_per_list,
						  memory_management *memory_info)
{
	linked_list_t **vector = calloc(nr_lists, sizeof(linked_list_t *));
	if (!vector) {
		free(vector);
		printf("Calloc failed for vector\n");
	}
	memory_info->total_memory = nr_lists * nr_bytes_per_list;
	memory_info->total_allocated_memory = 0;
	memory_info->nr_free_blocks = 0;
	memory_info->nr_allocated_blocks = 0;
	memory_info->nr_malloc_calls = 0;
	memory_info->nr_fragmentations = 0;
	memory_info->nr_free_calls = 0;
	memory_info->allocated_blocks = NULL;
	memory_info->nr_lists = nr_lists;
	int current_address = start_address;
	int node_size = 8;
	int index = 0;
	for (unsigned int i = 0; i < nr_lists; i++) {
		vector[i] = calloc(1, sizeof(linked_list_t));
		if (!vector[i]) {
			printf("Calloc failed for vector[%u]\n", i);
			for (unsigned int j = 0; j < i; j++)
				free(vector[j]);
			free(vector);
			return NULL;
		}
		vector[i]->nr_nodes = 0;
		vector[i]->data_size = node_size;
		unsigned int nr_nodes = nr_bytes_per_list / node_size;
		memory_info->nr_free_blocks += nr_nodes;
		for (unsigned int j = 0; j < nr_nodes; j++) {
			dll_node_t *current_node = dll_allocate_nth_node(vector[i], j,
															current_address,
															index);
			((dll_node_info *)(current_node->data))->index = index;
			index++;
			current_address += vector[i]->data_size;
		}
		node_size = node_size * 2;
	}
	return vector;
}