// Tudor Robert-Fabian 312CAa Anul 2023-2024
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LENGTH 6000

//Structura de date cu informații suplimentare pentru nodurile din vector
typedef struct dll_node_info {
	int address; //adresa nodului
	int index; //număr cu care identificăm nodul părinte pentru bonus
	void *value; //valoarea stocată în nod
} dll_node_info;

//structura unui nod din o listă din vector
typedef struct dll_node_t {
	void *data;
	struct dll_node_t *prev, *next;
} dll_node_t;

//Structura unei liste din vector
typedef struct doubly_linked_list_t {
	dll_node_t *head;
	unsigned int data_size;
	unsigned int nr_nodes;
} linked_list_t;

//Structura unui nod din lista de blocuri alocate
typedef struct block_info {
	void *data;
	struct block_info *prev, *next;
} block_info;

//Structura de date cu informații suplimentare pentru nodurile alocate
typedef struct dll_block_data {
	int address; //adresa nodului
	int size;
	int index; //număr cu care identificăm nodul părinte pentru bonus
	void *value; //valoarea stocată în nod
} dll_block_data;

//Structura de date la alegere care reține informații pentru dump_memory
typedef struct memory_management {
	unsigned int total_memory;
	unsigned int total_allocated_memory;
	unsigned int nr_free_blocks;
	unsigned int nr_allocated_blocks;
	unsigned int nr_malloc_calls;
	unsigned int nr_fragmentations;
	unsigned int nr_free_calls;
	unsigned int nr_lists;
	block_info *allocated_blocks;
} memory_management;

//Funcție care alocă dinamic memorie pentru un nod nou
//și il pune pe poziția nr
dll_node_t *dll_allocate_nth_node(linked_list_t *list, unsigned int nr,
								  int current_address, int index)
{
	if (!list)
		return NULL;
	unsigned int n = nr;
	if (n >= list->nr_nodes)
		n = list->nr_nodes;

	dll_node_t *new_node = calloc(1, sizeof(dll_node_t));
	if (!new_node) {
		printf("Calloc failed for new_node\n");
		free(new_node);
		return NULL;
	}
	new_node->data = calloc(1, sizeof(dll_node_info));
	((dll_node_info *)(new_node->data))->value = calloc(1, list->data_size);
	((dll_node_info *)(new_node->data))->address = current_address;
	((dll_node_info *)(new_node->data))->index = index;
	index++;
	dll_node_t *current_node = list->head;
	for (unsigned int i = 0; i < n; i++) {
		if (current_node->next == list->head)
			break;
		current_node = current_node->next;
	}
	if (!current_node) {
		new_node->next = new_node;
		new_node->prev = new_node;
		list->head = new_node;
	} else {
		if (n == list->nr_nodes) {
			new_node->next = current_node->next;
			new_node->prev = current_node;
			current_node->next->prev = new_node;
			current_node->next = new_node;
		} else {
			if (n == 0)
				list->head = new_node;
			new_node->next = current_node;
			new_node->prev = current_node->prev;
			current_node->prev->next = new_node;
			current_node->prev = new_node;
		}
	}
	list->nr_nodes++;
	return new_node;
}

//Funcție care elimină al n-lea nod din o listă
dll_node_t *dll_remove_nth_node(linked_list_t *list, unsigned int n)
{
	if (!list)
		return NULL;

	if (n >= list->nr_nodes)
		n = list->nr_nodes;

	dll_node_t *current_node = list->head;
	for (unsigned int i = 0; i < n; i++) {
		if (current_node->next == list->head)
			break;
		current_node = current_node->next;
	}
	if (!current_node)
		return NULL;
	if (n == list->nr_nodes) {
		current_node->prev->next = current_node->next;
		current_node->next->prev = current_node->prev;
	} else {
		if (n == 0)
			list->head = current_node->next;
		current_node->next->prev = current_node->prev;
		current_node->prev->next = current_node->next;
	}

	list->nr_nodes--;
	return current_node;
}

//Funcție care elimină un nod din listă pe baza unei adrese date
dll_node_t *dll_remove_node(linked_list_t *list, int address)
{
	if (!list)
		return NULL;

	dll_node_t *current_node = list->head;
	for (unsigned int i = 0; i < list->nr_nodes; i++) {
		if (((dll_node_info *)current_node->data)->address == address)
			break;
		current_node = current_node->next;
	}
	if (!current_node)
		return NULL;
	if (current_node == list->head->prev) {
		current_node->prev->next = current_node->next;
		current_node->next->prev = current_node->prev;
	} else {
		if (current_node == list->head)
			list->head = current_node->next;
		current_node->next->prev = current_node->prev;
		current_node->prev->next = current_node->next;
	}
	list->nr_nodes--;
	return current_node;
}

//Funcție care elimină un nod din lista de noduri alocate
block_info *remove_node_block_info(memory_management *memory_info,
								   block_info *current_node)
{
	if (!current_node)
		return NULL;
	if (current_node == memory_info->allocated_blocks) {
		memory_info->allocated_blocks = current_node->next;
		current_node->next->prev = current_node->prev;
		current_node->prev->next = current_node->next;
	} else {
		if (current_node->next == memory_info->allocated_blocks) {
			current_node->prev->next = current_node->next;
			current_node->next->prev = current_node->prev;
		} else {
			current_node->next->prev = current_node->prev;
			current_node->prev->next = current_node->next;
		}
	}
	memory_info->nr_allocated_blocks--;
	return current_node;
}

//Funcție care alocă memorie pentru vectorul de liste dublu înlănțuite
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

//Funcție care afișează infomațiile cerute pentru comanda DUMP_MEMORY
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

//Funcție care înserează un nod în o listă din vector
//crescător dupa adresă
void insert_node_ordonated_vector(linked_list_t *list, dll_node_t *new)
{
	if (!list) {
		new->next = new;
		new->prev = new;
		list->head = new;
		list->nr_nodes++;
		return;
	}
	dll_node_t *current_node = list->head;
	if (((dll_node_info *)new->data)->address >
		((dll_node_info *)list->head->prev->data)->address) {
		new->next = list->head;
		new->prev = list->head->prev;
		list->head->prev->next = new;
		list->head->prev = new;
		list->nr_nodes++;
		return;
	}
	if (((dll_node_info *)new->data)->address <
		((dll_node_info *)list->head->data)->address) {
		new->next = list->head;
		new->prev = list->head->prev;
		list->head->prev->next = new;
		list->head->prev = new;
		list->head = new;
		list->nr_nodes++;
		return;
	}

	while (current_node->next != list->head) {
		if (((dll_node_info *)current_node->data)->address >=
			((dll_node_info *)new->data)->address)
			break;
		current_node = current_node->next;
	}
	new->next = current_node;
	new->prev = current_node->prev;
	current_node->prev->next = new;
	current_node->prev = new;

	list->nr_nodes++;
}

//Funcție care înserează un nod în o listă din vector,
//crescător dupa adresă
void insert_element_ordonated_allocated_blocks(memory_management *memory_info,
											   block_info *new)
{
	block_info *current_node = memory_info->allocated_blocks;
	if (!memory_info->allocated_blocks ||
		memory_info->nr_allocated_blocks == 0) {
		new->next = new;
		new->prev = new;
		memory_info->allocated_blocks = new;
		memory_info->nr_allocated_blocks++;
		memory_info->allocated_blocks = memory_info->allocated_blocks;
		return;
	}
	if (((dll_node_info *)new->data)->address >
		((dll_block_data *)memory_info->allocated_blocks->prev->data)
		->address) {
		new->next = memory_info->allocated_blocks;
		new->prev = memory_info->allocated_blocks->prev;
		memory_info->allocated_blocks->prev->next = new;
		memory_info->allocated_blocks->prev = new;
	} else {
		if (((dll_node_info *)new->data)->address <
			((dll_block_data *)memory_info->allocated_blocks->data)->address) {
			memory_info->allocated_blocks = new;
			new->next = current_node;
			new->prev = current_node->prev;
			current_node->prev->next = new;
			current_node->prev = new;
		} else {
			while (current_node->next != memory_info->allocated_blocks) {
				if (((dll_node_info *)new->data)->address <=
					((dll_block_data *)current_node->data)->address)
					break;
				current_node = current_node->next;
			}
			new->next = current_node;
			new->prev = current_node->prev;
			current_node->prev->next = new;
			current_node->prev = new;
		}
	}
	memory_info->nr_allocated_blocks++;
}

//Funție care tratează cazul care în urma alocării de memorie
//ne rămâne fragment din nodul inițial și trebuie
//reintrodus în vector
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

//Funcție care alocă memorie(noduri) din vector și le
//pune în lista de noduri alocate
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

//Funcție care tratează cazul în care se elibereză un nod
//și nu găsim un nod cu același index în listă și
//punem nodul dezalocat după dimensiune și adresă în vector
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

//Funcție de free pentru tipul de reconstituire 0
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

//Funcție care redimensionează vectorul în cazul în urma
//operațiilor de FREE/MALLOC avem o listă goală
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

//Funcție care tratează cazul de unire a ambelor noduri laterale
//în urma operației de FREE
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

//Funcție care face reconstituirea blocurilor pentru bonus
//în funcție de cele 4 cazuri existente
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
	//dacă avem de unit ambele noduri(stânga-dreapta) apelăm
	//funcția corespunzătoare
	free_merge_both_lateral_nodes(&vector, &memory_info, list_left_node,
								  list_right_node, size, new_node);
	*memory_info_ptr = memory_info; *vector_ptr = vector;
}

//Funcție care caută noduri laterale in vectorul de liste
//pentru a face reconstrucția de la bonus și reține
//poziția acelor noduri din vector
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
	//lista din care face parte nodul din stânga
	int list_left_node = *list_left_node_ptr;
	//lista din care face parte nodul din dreapta
	int list_right_node = *list_right_node_ptr;
	dll_node_t *new_node = *new_node_ptr;
	//nodul din stânga pentru reconstituire
	dll_node_t *left_node = *left_node_ptr;
	//nodul din dreapta pentru reconstituire
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

//Funcție de free pentru tipul de reconstituire 1
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

//Funcție care dezalocă toate resursele și încheie programul
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

//Funcție care transformă un număr hexadecimal într-unul decimal
int convert_hexa_to_decimal(char hexnumber[])
{
	int digit, decimalnumber = 0, power = 1;
	for (int i = (strlen(hexnumber) - 1); i >= 2; i--) {
		switch (hexnumber[i]) {
		case 'a':
			digit = 10;
			break;
		case 'b':
			digit = 11;
			break;
		case 'c':
			digit = 12;
			break;
		case 'd':
			digit = 13;
			break;
		case 'e':
			digit = 14;
			break;
		case 'f':
			digit = 15;
			break;
		default:
			digit = hexnumber[i] - '0';
		}
		decimalnumber = decimalnumber + digit * power;
		power = power * 16;
	}
	return decimalnumber;
}

//Funcție care verifică dacă putem citi/scrie nr_bytes de la address
//în memoria alocată
int check_write_read(int address, int nr_bytes, memory_management *memory_info)
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
		((dll_block_data *)(current_node->data))->address - address;
		int remaining_size_first_block =
		((dll_block_data *)(current_node->data))->size - difference;
		int remaining_bytes = nr_bytes - remaining_size_first_block;
		int current_address =
		((dll_block_data *)(current_node->data))->address +
		((dll_block_data *)(current_node->data))->size;
		if (remaining_bytes <= 0)
			return 1;
		current_node = current_node->next;
		while (nr_bytes != 0 && current_node) {
			if (current_address !=
				((dll_block_data *)current_node->data)->address) {
				return 0;
			}
			remaining_bytes -=
			((dll_block_data *)(current_node->data))->size;
			if (remaining_bytes <= 0)
				return 1;
			current_address =
			((dll_block_data *)current_node->data)->address +
			((dll_block_data *)current_node->data)->size;
			current_node = current_node->next;
		}
		if (nr_bytes < 0)
			return 0;
	} else {
		return 0;
	}
	return 1;
}

//Funcție care scrie nr_bytes la address în memoria alocată
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

//Funcție care citește nr_bytes de la address din memoria alocată
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

//Funcție care parsează informațiile primite după WRITE
void parse_write_command(const char *command, char **string, int *number)
{
	const char *start_quote = strchr(command, '"');
	const char *end_quote = strrchr(command, '"');
	int string_length = end_quote - start_quote - 1;

	*string = calloc((string_length + 1), sizeof(char));
	if (!(*string)) {
		free(string);
		return;
	}
	strncpy(*string, start_quote + 1, string_length);
	(*string)[string_length] = '\0';

	if (sscanf(end_quote + 1, "%d", number) != 1) {
		free(*string);
		return;
	}
}

//În main apelăm funcția corespunzătoare după comanda primită.
//De asemenea, tot aici facem transformările și parsările necesare
//pentru a face apelul funcțiilor
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
