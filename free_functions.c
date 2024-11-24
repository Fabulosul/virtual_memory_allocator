#include "free_functions.h"
#include "helper_functions.h"

// Funcție care tratează cazul în care se elibereză un nod
// și nu găsim un nod cu același index în listă și
// punem nodul dezalocat după dimensiune și adresă în vector
void free_no_index_in_list_case(dll_node_t *new_node, int size,
								memory_management *memory_info,
								linked_list_t ***vector_ptr)
{
	linked_list_t **vector = *vector_ptr;
	int poz = -1;
	for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
		if (vector[i]->data_size >=
			(unsigned int)size && vector[i]->nr_nodes > 0) {
			poz = i;
			break;
		}
	}
	if (poz != -1 && vector[poz]->data_size == (unsigned int)size) {
		insert_node_ordonated_vector(vector[poz], new_node);
	} else {
		if (memory_info->nr_lists == 0) {
			memory_info->nr_lists++;
		} else {
			memory_info->nr_lists++;
			vector = realloc(vector, (memory_info->nr_lists) *
					 sizeof(linked_list_t *));
		}

		if (!vector) {
			printf("Realloc failed for vector\n");
			free(vector);
			return;
		}
		vector[memory_info->nr_lists - 1] =
		calloc(1, sizeof(linked_list_t));
		if (!vector[memory_info->nr_lists - 1]) {
			printf("Calloc failed for vector[i]\n");
			free(vector[memory_info->nr_lists - 1]);
			return;
		}

		vector[memory_info->nr_lists - 1]->nr_nodes = 1;
		vector[memory_info->nr_lists - 1]->data_size = size;
		vector[memory_info->nr_lists - 1]->head = new_node;
		vector[memory_info->nr_lists - 1]->head->next =
		vector[memory_info->nr_lists - 1]->head;
		vector[memory_info->nr_lists - 1]->head->prev =
		vector[memory_info->nr_lists - 1]->head;
		for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
			for (unsigned int j = i + 1; j < memory_info->nr_lists; j++) {
				if (vector[i]->data_size > vector[j]->data_size) {
					linked_list_t *aux = vector[i];
					vector[i] = vector[j];
					vector[j] = aux;
				}
			}
		}
	}
	*vector_ptr = vector;
}

// Funcție de free pentru tipul de reconstituire 0
void my_free(linked_list_t ***vector_ptr, memory_management **memory_info_ptr,
			 int address)
{
	memory_management *memory_info = *memory_info_ptr;
	linked_list_t **vector = *vector_ptr;
	block_info *current_node = memory_info->allocated_blocks;

	if (!current_node || memory_info->nr_allocated_blocks == 0) {
		printf("Invalid free\n");
		*vector_ptr = vector;
		memory_info->allocated_blocks = NULL;
		return;
	}
	while (current_node->next != memory_info->allocated_blocks) {
		if (address >= ((dll_block_data *)current_node->data)->address &&
			address <= ((dll_block_data *)current_node->next->data)->address)
			break;
		current_node = current_node->next;
	}

	if (address ==
		((dll_block_data *)memory_info->allocated_blocks->prev->data)->address)
		current_node = memory_info->allocated_blocks->prev;

	if (((dll_block_data *)current_node->next->data)->address == address)
		current_node = current_node->next;

	if (((dll_block_data *)current_node->data)->address == address) {
		block_info *freed_node =
		remove_node_block_info(memory_info, current_node);
		memory_info->total_allocated_memory -=
		((dll_block_data *)(freed_node->data))->size;
		memory_info->nr_free_calls++;
		dll_node_t *new_node = calloc(1, sizeof(dll_node_t));
		if (!new_node) {
			printf("Calloc failed for new_node\n");
			free(new_node);
			return;
		}
		new_node->data = calloc(1, sizeof(dll_node_info));
		((dll_node_info *)(new_node->data))->value =
		calloc(1, ((dll_block_data *)(freed_node->data))->size);
		((dll_node_info *)(new_node->data))->address =
		((dll_block_data *)(freed_node->data))->address;
		unsigned int size = ((dll_block_data *)(freed_node->data))->size;
		free(((dll_block_data *)(freed_node->data))->value);
		free(freed_node->data);
		free(freed_node);
		free_no_index_in_list_case(new_node, size, memory_info, &vector);
	} else {
		printf("Invalid free\n");
	}
	*memory_info_ptr = memory_info;
	*vector_ptr = vector;
}

// Funcție care redimensionează vectorul în cazul în urma
// operațiilor de FREE/MALLOC avem o listă goală
void free_realloc_vector(int list_node, memory_management *memory_info,
						 linked_list_t ***vector_ptr)
{
	linked_list_t **vector = *vector_ptr;
	vector[list_node]->data_size =
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
	*vector_ptr = vector;
}

// Funcție care tratează cazul de unire a ambelor noduri laterale
// în urma operației de FREE
void free_merge_both_lateral_nodes(linked_list_t ***vector_ptr,
								   memory_management **memory_info_ptr,
								   int list_left_node, int list_right_node,
								   unsigned int size, dll_node_t *new_node)
{
	linked_list_t **vector = *vector_ptr;
	memory_management *memory_info = *memory_info_ptr;
	dll_node_t *curr = NULL;
	if (list_left_node != -1 && list_right_node != -1) {
		int left_size = vector[list_left_node]->data_size;
		int right_size = vector[list_right_node]->data_size;
		dll_node_t *merged_node = calloc(1, sizeof(dll_node_t));
		if (!merged_node) {
			printf("Calloc failed for merged_node\n");
			free(merged_node);
			return;
		}
		merged_node->data = calloc(1, sizeof(dll_node_info));
		((dll_node_info *)(merged_node->data))->value =
		calloc(1, vector[list_left_node]->data_size + size +
				vector[list_right_node]->data_size);
		((dll_node_info *)(merged_node->data))->index =
		((dll_node_info *)(new_node->data))->index;
		((dll_node_info *)(merged_node->data))->address =
		((dll_node_info *)(new_node->data))->address -
		vector[list_left_node]->data_size;
		free_no_index_in_list_case(merged_node, left_size + size + right_size,
								   memory_info, &vector);
		dll_node_t *old_node_left = dll_remove_node(vector[list_left_node],
									((dll_node_info *)(new_node->data))
									->address - left_size);
		free(((dll_node_info *)(old_node_left->data))->value);
		free(old_node_left->data);
		free(old_node_left);
		if (vector[list_left_node]->nr_nodes == 0)
			free_realloc_vector(list_left_node, memory_info, &vector);
		for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
			curr = vector[i]->head;
			while (curr->next != vector[i]->head) {
				if (((dll_node_info *)curr->data)->index ==
					((dll_node_info *)new_node->data)->index) {
					if (((dll_node_info *)new_node->data)->address + size ==
						(unsigned int)((dll_node_info *)curr->data)->address)
						list_right_node = i;
				}
				curr = curr->next;
			}
			if (((dll_node_info *)vector[i]->head->prev->data)->index ==
				((dll_node_info *)new_node->data)->index) {
				if (((dll_node_info *)new_node->data)->address + size ==
					(unsigned int)((dll_node_info *)vector[i]->head->prev
					->data)->address)
					list_right_node = i;
			}
		}
		dll_node_t *old_node_right = dll_remove_node(vector[list_right_node],
									 ((dll_node_info *)(new_node->data))
									 ->address + size);
		free(((dll_node_info *)old_node_right->data)->value);
		free(old_node_right->data);
		free(old_node_right);
		if (vector[list_right_node]->nr_nodes == 0)
			free_realloc_vector(list_right_node, memory_info, &vector);
		free(((dll_node_info *)new_node->data)->value);
		free(new_node->data);
		free(new_node);
		*memory_info_ptr = memory_info;
		*vector_ptr = vector;
		return;
	}
	*memory_info_ptr = memory_info;
	*vector_ptr = vector;
}

// Funcție care face reconstituirea blocurilor pentru bonus
// în funcție de cele 4 cazuri existente
void free_merge_lateral_nodes_case(linked_list_t ***vector_ptr,
								   memory_management **memory_info_ptr,
								   int list_left_node, int list_right_node,
								   unsigned int size, dll_node_t *new_node)
{
	linked_list_t **vector = *vector_ptr;
	memory_management *memory_info = *memory_info_ptr;
	if (list_left_node == -1 && list_right_node == -1) {
		free_no_index_in_list_case(new_node, size, memory_info, &vector);
		*memory_info_ptr = memory_info;
		*vector_ptr = vector;
		return;
	}
	if (list_left_node != -1 && list_right_node == -1) {
		int left_size = vector[list_left_node]->data_size;
		dll_node_t *merged_node = calloc(1, sizeof(dll_node_t));
		if (!merged_node) {
			printf("Calloc failed for merged_node\n");
			free(merged_node);
			return;
		}
		merged_node->data = calloc(1, sizeof(dll_node_info));
		((dll_node_info *)merged_node->data)->value =
		calloc(1, size + vector[list_left_node]->data_size);
		((dll_node_info *)merged_node->data)->index =
		((dll_node_info *)new_node->data)->index;
		dll_node_t *old_node = dll_remove_node(vector[list_left_node],
							   ((dll_node_info *)new_node->data)->address -
							   left_size);
		((dll_node_info *)merged_node->data)->address =
		((dll_node_info *)old_node->data)->address;
		free_no_index_in_list_case(merged_node, size +
									left_size,
									memory_info, &vector);
		free(((dll_node_info *)old_node->data)->value);
		free(old_node->data); free(old_node);
		if (vector[list_left_node]->nr_nodes == 0)
			free_realloc_vector(list_left_node, memory_info, &vector);
		free(((dll_node_info *)new_node->data)->value);
		free(new_node->data); free(new_node);
		*memory_info_ptr = memory_info;
		*vector_ptr = vector;
		return;
	}
	if (list_left_node == -1 && list_right_node != -1) {
		int right_size = vector[list_right_node]->data_size;
		dll_node_t *merged_node = calloc(1, sizeof(dll_node_t));
		if (!merged_node) {
			printf("Calloc failed for merged_node\n");
			free(merged_node);
			return;
		}
		merged_node->data = calloc(1, sizeof(dll_node_info));
		((dll_node_info *)(merged_node->data))->value =
		calloc(1, size + vector[list_right_node]->data_size);
		((dll_node_info *)(merged_node->data))->address =
		((dll_node_info *)(new_node->data))->address;
		((dll_node_info *)(merged_node->data))->index =
		((dll_node_info *)(new_node->data))->index;
		free_no_index_in_list_case(merged_node, size + right_size,
								   memory_info, &vector);
		dll_node_t *old_node = dll_remove_node(vector[list_right_node],
							   ((dll_node_info *)(new_node->data))->address +
								size);
		free(((dll_node_info *)(old_node->data))->value);
		free(old_node->data); free(old_node);
		if (vector[list_right_node]->nr_nodes == 0)
			free_realloc_vector(list_right_node, memory_info, &vector);
		free(((dll_node_info *)(new_node->data))->value);
		free(new_node->data); free(new_node);
		*memory_info_ptr = memory_info;
		*vector_ptr = vector;
		return;
	}
	// dacă avem de unit ambele noduri(stânga-dreapta) apelăm
	// funcția corespunzătoare
	free_merge_both_lateral_nodes(&vector, &memory_info, list_left_node,
								  list_right_node, size, new_node);
	*memory_info_ptr = memory_info; *vector_ptr = vector;
}

// Funcție care caută noduri laterale in vectorul de liste
// pentru a face reconstrucția de la bonus și reține
// poziția acelor noduri din vector
void free_search_node_by_index(linked_list_t ***vector_ptr,
							   memory_management **memory_info_ptr,
								int *list_left_node_ptr,
								int *list_right_node_ptr,
								unsigned int size, dll_node_t **new_node_ptr,
								dll_node_t **left_node_ptr,
								dll_node_t **right_node_ptr)
{
	linked_list_t **vector = *vector_ptr;
	memory_management *memory_info = *memory_info_ptr;
	// lista din care face parte nodul din stânga
	int list_left_node = *list_left_node_ptr;
	// lista din care face parte nodul din dreapta
	int list_right_node = *list_right_node_ptr;
	dll_node_t *new_node = *new_node_ptr;
	// nodul din stânga pentru reconstituire
	dll_node_t *left_node = *left_node_ptr;
	// nodul din dreapta pentru reconstituire
	dll_node_t *right_node = *right_node_ptr;
	dll_node_t *curr = NULL;
	for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
		curr = vector[i]->head;
		while (curr->next != vector[i]->head) {
			if (((dll_node_info *)curr->data)->index ==
				((dll_node_info *)new_node->data)->index) {
				if (((dll_node_info *)new_node->data)->address + size ==
					(unsigned int)((dll_node_info *)curr->data)->address) {
					right_node = curr;
					list_right_node = i;
				}
				if (((dll_node_info *)new_node->data)->address ==
					(int)(((dll_node_info *)curr->data)->address +
							vector[i]->data_size)) {
					left_node = curr;
					list_left_node = i;
				}
			}
			curr = curr->next;
		}

		if (((dll_node_info *)vector[i]->head->prev->data)->index ==
			((dll_node_info *)new_node->data)->index) {
			if (((dll_node_info *)new_node->data)->address + size ==
				(unsigned int)((dll_node_info *)vector[i]->head
				->prev->data)->address) {
				right_node = vector[i]->head->prev->data;
				list_right_node = i;
			}
			if (((dll_node_info *)new_node->data)->address ==
				(int)(((dll_node_info *)vector[i]->head->prev->data)
						->address + vector[i]->data_size)) {
				left_node = vector[i]->head->prev->data;
				list_left_node = i;
			}
		}
	}
	*memory_info_ptr = memory_info;
	*vector_ptr = vector;
	*list_left_node_ptr = list_left_node;
	*list_right_node_ptr = list_right_node;
	*new_node_ptr = new_node;
	*left_node_ptr = left_node;
	*right_node_ptr = right_node;
}

// Funcție de free pentru tipul de reconstituire 1
void my_free_1(linked_list_t ***vector_ptr,
			   memory_management **memory_info_ptr, int address)
{
	memory_management *memory_info = *memory_info_ptr;
	linked_list_t **vector = *vector_ptr;
	block_info *current_node = memory_info->allocated_blocks;
	if (!current_node || memory_info->nr_allocated_blocks == 0) {
		printf("Invalid free\n");
		*vector_ptr = vector; memory_info->allocated_blocks = NULL;
		return;
	}
	while (current_node->next != memory_info->allocated_blocks) {
		if (address >= ((dll_block_data *)current_node->data)->address &&
			address <= ((dll_block_data *)current_node->next->data)->address) {
			break;
		}
		current_node = current_node->next;
	}
	if (address ==
		((dll_block_data *)memory_info->allocated_blocks->prev->data)->address)
		current_node = memory_info->allocated_blocks->prev;
	if (((dll_block_data *)current_node->next->data)->address ==
		address)
		current_node = current_node->next;
	if (((dll_block_data *)current_node->data)->address == address) {
		block_info *freed_node =
		remove_node_block_info(memory_info, current_node);
		memory_info->total_allocated_memory -=
		((dll_block_data *)(freed_node->data))->size;
		memory_info->nr_free_calls++;
		dll_node_t *new_node = calloc(1, sizeof(dll_node_t));
		if (!new_node) {
			printf("Calloc failed for new_node\n");
			free(new_node);
			return;
		}
		new_node->data = calloc(1, sizeof(dll_node_info));
		((dll_node_info *)new_node->data)->value =
		calloc(1, ((dll_block_data *)(freed_node->data))->size);
		((dll_node_info *)new_node->data)->address =
		((dll_block_data *)(freed_node->data))->address;
		((dll_node_info *)new_node->data)->index =
		((dll_block_data *)(freed_node->data))->index;
		unsigned int size = ((dll_block_data *)(freed_node->data))->size;
		int vector_poz = -1;
		for (unsigned int i = 0; i < memory_info->nr_lists; i++) {
			dll_node_t *node = vector[i]->head;
			for (unsigned int j = 0; j < vector[i]->nr_nodes; j++) {
				if (((dll_node_info *)node->data)->index ==
					((dll_node_info *)new_node->data)->index) {
					vector_poz = i; break;
				}
				node = node->next;
			}
			if (((dll_node_info *)node->data)->index ==
				((dll_node_info *)new_node->data)->index) {
				vector_poz = i; break;
			}
		}
		if (vector_poz == -1) {
			free(((dll_block_data *)(freed_node->data))->value);
			free(freed_node->data); free(freed_node);
			free_no_index_in_list_case(new_node, size, memory_info, &vector);
			*memory_info_ptr = memory_info; *vector_ptr = vector;
			return;
		}
		free(((dll_block_data *)(freed_node->data))->value);
		free(freed_node->data); free(freed_node);
		dll_node_t *left_node = NULL; dll_node_t *right_node = NULL;
		int list_left_node = -1; int list_right_node = -1;
		free_search_node_by_index(&vector, &memory_info, &list_left_node,
								  &list_right_node, size, &new_node,
								  &left_node, &right_node);
		free_merge_lateral_nodes_case(&vector, &memory_info, list_left_node,
									  list_right_node, size, new_node);
	} else {
		printf("Invalid free\n");
	}
	*memory_info_ptr = memory_info; *vector_ptr = vector;
}