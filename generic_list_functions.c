#include "generic_list_functions.h"

dll_node_t *dll_allocate_nth_node(linked_list_t *list, unsigned int nr, int current_address, int index) {
    if (!list)
        return NULL;
    unsigned int n = nr;
    if (n >= list->nr_nodes)
        n = list->nr_nodes;

    dll_node_t *new_node = calloc(1, sizeof(dll_node_t));
    if (!new_node) {
        printf("Calloc failed for new_node\n");
        return NULL;
    }
    new_node->data = calloc(1, sizeof(dll_node_info));
    ((dll_node_info *)(new_node->data))->value = calloc(1, list->data_size);
    ((dll_node_info *)(new_node->data))->address = current_address;
    ((dll_node_info *)(new_node->data))->index = index;

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

dll_node_t *dll_remove_nth_node(linked_list_t *list, unsigned int n) {
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

dll_node_t *dll_remove_node(linked_list_t *list, int address) {
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


