#ifndef STRUCT_H
#define STRUCT_H

#include <string.h>
#include <stdio.h>


#include <stdlib.h>

// Structura unui nod din lista dublu înlănțuită
typedef struct dll_node_info {
    int address;  // adresa nodului
    int index;    // indexul nodului
    void *value;  // valoarea stocată în nod
} dll_node_info;

// Structura unui nod din lista dublu înlănțuită
typedef struct dll_node_t {
    void *data;              // pointer la datele stocate
    struct dll_node_t *prev; //  pointer la nodul anterior
    struct dll_node_t *next; // pointer la nodul următor
} dll_node_t;

// Structura listei dublu înlănțuite
typedef struct doubly_linked_list_t {
    dll_node_t *head;        // pointer la primul nod
    unsigned int data_size;  // dimensiunea datelor stocate
    unsigned int nr_nodes;   // numărul de noduri din listă
} linked_list_t;


// Structura unui nod din lista de blocuri alocate
typedef struct block_info {
    void *data; // pointer la datele stocate
    struct block_info *prev, *next; // pointer la nodul anterior și următor
} block_info;

// Structura de date cu informații suplimentare pentru nodurile alocate
typedef struct dll_block_data {
    int address; // Adresa nodului
    int size;    // Dimensiunea nodului
    int index;   // Număr pentru identificarea nodului părinte (bonus)
    void *value; // Valoarea stocată în nod
} dll_block_data;


// Structura de date la alegere care reține informații pentru dump_memory
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

// Macro
#define MAX_LENGTH 6000

#endif // STRUCT_H
