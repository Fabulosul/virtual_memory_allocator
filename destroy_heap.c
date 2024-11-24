#include "destroy_heap.h"

// Funcție care dezalocă toate resursele și încheie programul
void destroy_heap(linked_list_t ***vector_ptr,
				  memory_management **memory_info_ptr)
{
	if (!*vector_ptr)
		return;
	memory_management *memory_info = *memory_info_ptr;
	linked_list_t **vector = *vector_ptr;
	for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
		dll_node_t *current_node = vector[i]->head->prev;
		while (current_node != vector[i]->head) {
			current_node = current_node->prev;
			free(((dll_node_info *)current_node->next->data)->value);
			free(current_node->next->data);
			free(current_node->next);
		}
		free(((dll_node_info *)current_node->data)->value);
		free(current_node->data);
		free(current_node);
		free(vector[i]);
	}
	free(vector);
	if (memory_info->nr_allocated_blocks > 0) {
		block_info *current = memory_info->allocated_blocks->prev;
		while (current != memory_info->allocated_blocks) {
			current = current->prev;
			free(((dll_block_data *)current->next->data)->value);
			free(current->next->data);
			free(current->next);
		}
		free(((dll_block_data *)current->data)->value);
		free(current->data);
		free(current);
	}
	free(memory_info);
	*memory_info_ptr = memory_info;
	*vector_ptr = vector;
}
