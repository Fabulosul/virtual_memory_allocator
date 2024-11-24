#include "malloc.h"
#include "helper_functions.h"
#include "generic_list_functions.h"

// Funție care tratează cazul care în urma alocării de memorie
// ne rămâne fragment din nodul inițial și trebuie
// reintrodus în vector
void remaining_space_case_malloc(linked_list_t ***vector_ptr,
								 memory_management *memory_info,
								 int remaining_space, int remaining_address,
								 block_info *new_node)
{
	linked_list_t **vector = *vector_ptr;
	memory_info->nr_fragmentations++;
	int poz = -1;
	for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
		if (vector[i]->data_size >= (unsigned int)remaining_space) {
			poz = i;
			break;
		}
	}
	if (poz == -1 || vector[poz]->data_size != (unsigned int)remaining_space) {
		memory_info->nr_lists++;
		vector = realloc(vector, (memory_info->nr_lists) *
									 sizeof(linked_list_t *));
		if (!vector) {
			printf("Realloc failed for vector\n");
			free(vector);
			return;
		}
		vector[memory_info->nr_lists - 1] = calloc(1, sizeof(linked_list_t));
		if (!vector[memory_info->nr_lists - 1]) {
			printf("Malloc failed for vector[i]\n");
			free(vector[memory_info->nr_lists - 1]);
			return;
		}
		linked_list_t *list = vector[memory_info->nr_lists - 1];
		list->nr_nodes = 1;
		list->data_size = remaining_space;
		list->head = calloc(1, sizeof(dll_node_t));
		if (!list->head) {
			printf("Calloc failed for list->head\n");
			free(list->head);
			return;
		}
		list->head->data = calloc(1, sizeof(dll_node_info));
		((dll_node_info *)list->head->data)->value = calloc(1, remaining_space);
		list->head->next = list->head;
		((dll_node_info *)list->head->data)->index =
		((dll_block_data *)(new_node->data))->index;
		list->head->prev = list->head;
		((dll_node_info *)list->head->data)->address = remaining_address;
		vector[memory_info->nr_lists - 1] = list;
		for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
			for (unsigned int j = i + 1; j < memory_info->nr_lists; j++) {
				if (vector[i]->data_size > vector[j]->data_size) {
					linked_list_t *aux = vector[i];
					vector[i] = vector[j];
					vector[j] = aux;
				}
			}
		}
	} else {
		dll_node_t *new = calloc(1, sizeof(dll_node_t));
		if (!new) {
			printf("Calloc failed for new\n");
			free(new);
			return;
		}
		new->data = calloc(1, sizeof(dll_node_info));
		((dll_node_info *)(new->data))->address = remaining_address;
		((dll_node_info *)(new->data))->value =
		calloc(1, vector[poz]->data_size);
		insert_node_ordonated_vector(vector[poz], new);
	}
	*vector_ptr = vector;
}

// Funcție care alocă memorie(noduri) din vector și le
// pune în lista de noduri alocate
void my_malloc(unsigned int size, linked_list_t ***vector_ptr,
			   memory_management *memory_info)
{
	linked_list_t **vector = *vector_ptr;
	if (size > vector[memory_info->nr_lists - 1]->data_size) {
		printf("Out of memory\n");
		return;
	}
	int poz = -1;
	for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
		if (vector[i]->data_size >= size && vector[i]->nr_nodes > 0) {
			poz = (int)i;
			break;
		}
	}
	memory_info->nr_free_blocks--;
	dll_node_t *allocated_node = dll_remove_nth_node(vector[poz], 0);
	block_info *new_node = calloc(1, sizeof(block_info));
	if (!new_node) {
		printf("Calloc failed for new_node\n");
		free(new_node);
		return;
	}
	new_node->data = calloc(1, sizeof(dll_block_data));
	((dll_block_data *)(new_node->data))->value = calloc(1, size);
	((dll_block_data *)(new_node->data))->size = size;
	((dll_block_data *)(new_node->data))->address =
	((dll_node_info *)(allocated_node->data))->address;
	((dll_block_data *)(new_node->data))->index =
	((dll_node_info *)(allocated_node->data))->index;
	insert_element_ordonated_allocated_blocks(memory_info, new_node);
	memory_info->nr_malloc_calls++;
	memory_info->total_allocated_memory +=
	((dll_block_data *)(new_node->data))->size;
	unsigned int remaining_space = vector[poz]->data_size - size;
	int remaining_address = ((dll_node_info *)(allocated_node->data))->address +
						   size;
	free(((dll_node_info *)(allocated_node->data))->value);
	free(allocated_node->data);
	free(allocated_node);
	if (vector[poz]->nr_nodes == 0) {
		vector[poz]->data_size =
		(unsigned int)vector[memory_info->nr_lists - 1]->data_size + 1;
		for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
			for (unsigned int j = i + 1; j < memory_info->nr_lists; j++) {
				if (vector[i]->data_size > vector[j]->data_size) {
					linked_list_t *aux = vector[i];
					vector[i] = vector[j];
					vector[j] = aux;
				}
			}
		}
		free(vector[memory_info->nr_lists - 1]);
		memory_info->nr_lists--;
		if (memory_info->nr_lists != 0) {
			vector = realloc(vector, (memory_info->nr_lists) *
					 sizeof(linked_list_t *));
			if (!vector) {
				printf("Realloc failed for vector\n");
				free(vector);
				return;
			}
		}
	}
	if (remaining_space != 0) {
		remaining_space_case_malloc(&vector, memory_info, remaining_space,
									remaining_address, new_node);
	}
	*vector_ptr = vector;
}