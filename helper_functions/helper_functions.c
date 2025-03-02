#include "helper_functions.h"

// Funcție care elimină un nod din lista de noduri alocate
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

// Funcție care înserează un nod în o listă din vector
// crescător dupa adresă
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

// Funcție care înserează un nod în o listă din vector,
// crescător dupa adresă
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



// Funcție care transformă un număr hexadecimal într-unul decimal
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

// Funcție care verifică dacă putem citi/scrie nr_bytes de la address
// în memoria alocată
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

// Funcție care parsează informațiile primite după WRITE
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