**Tudor Robert-Fabian**  
**Year 2023-2024**  

## Segregated Free Lists  

### Description:  

* The project aims to develop a memory allocator using a data structure called "Segregated Free Lists." This structure is represented by an array of doubly linked lists, with each list managing memory blocks of the same size. The purpose of the memory allocator is to provide various functionalities such as allocation, deallocation, reading, and writing memory based on received commands.  

* For memory allocation, the **MALLOC** command is followed by the number of bytes we want to allocate. The allocator searches the array of lists for an available memory block of the appropriate size. If none is found, it may split larger blocks to fulfill the request.  

* Memory deallocation is performed using the **FREE** command, which receives the starting address of the memory block to be freed. This block is marked as free and reinserted into the array of lists.  

* Available operations include **READ** and **WRITE**, which allow accessing and modifying memory content at a specific address for a specified length.  

* Another function that needs to be implemented is displaying the current memory state (**DUMP_MEMORY**).  

* The project also involves careful error management, such as handling out-of-memory situations and preventing access to unallocated memory (Segmentation fault).  

## Explanations:  

### init_heap  

* The function allocates memory for the array of doubly linked lists referred to in the project as "Segregated Free Lists." It receives the starting address of the memory area, the number of lists in the array, and the size of each list. Additionally, it initializes the array of lists and the corresponding memory blocks for each list using calls to the **dll_allocate_nth_node** function. Finally, the function returns the array of lists.  

### dump_memory  

* The function displays information about the current state of allocated and unallocated memory, as well as the used and available memory blocks.  

### my_malloc  

* The function starts by checking whether there is enough space in the array of lists for the requested allocation. This verification involves traversing each list in the array to determine if an available block of sufficient size exists.  

* If a suitable space is found, the function removes a node from the corresponding list and uses this memory block to create a new allocated block, updating its information accordingly.  

* If, after the requested allocation, some space remains unused, the function reintroduces this remaining block into the array of lists.  

* To perform these operations, the function uses two auxiliary functions: **insert_node_ordonated_vector** and **insert_element_ordonated_allocated_blocks**. These auxiliary functions are responsible for inserting nodes and memory blocks in an ordered manner into their respective data structures.  

### my_free - *(reconstruction type 0)*  

* The function traverses the list of allocated blocks to locate the block containing the specified address. If the address corresponds to the start of a node in the array of lists, the function removes the block from the allocated list, and its occupied memory is marked as free. Additionally, it creates a new block of the same size as the freed one.  

* If the block’s size already exists in the array, the function inserts it into the corresponding list. Otherwise, the array is resized, and a new list is created for the block size, where it is then added.  

### my_free_1 - *(reconstruction type 1)*  

* The function traverses the list of allocated blocks to locate the block containing the specified address. If the address corresponds to the start of a node in the array of lists, the function removes the block from the allocated list, and its occupied memory is marked as free. Additionally, the function creates a new block of the same size as the removed one (**new_node**).  

* The function then searches the array for blocks that have the same index as the freed block and are adjacent to it (either on the left, right, or both).  

* If no adjacent blocks are found, **new_node** is inserted into the list in order based on size and address. If one or two adjacent blocks exist, a new node (**merged_node**) is initialized by merging **new_node** with the adjacent block(s). **new_node** is then deallocated, and **merged_node** is inserted into the array based on its new size and address.  

### destroy_heap  

* This function is responsible for freeing all allocated resources and terminating the program. It traverses the node lists in the array and the allocated block list, freeing all allocated memory.  

### write  

* The function handles writing a specified number of bytes at a given address in the allocated memory list. It traverses the allocated memory blocks to check whether the specified address falls within one of them.  

* If there are not enough allocated bytes from the given address onward, the function displays an error message and calls **dump_memory**. If a block containing the address and sufficient memory is found, the function performs the write operation at that address.  

### read  

* The function handles reading a specified number of bytes from a given address in the allocated memory list. It traverses the allocated memory blocks to check whether the specified address falls within one of them.  

* If the message to be written is smaller than **nr_bytes**, only the available bytes are written.  

* If there are not enough allocated bytes from the given address onward, the function displays an error message and calls **dump_memory**. If a block containing the address and sufficient memory is found, the function reads **nr_bytes** from that address.  

### main  

* The function processes various commands, parsing or transforming them to call the main functions of the program. These operations include memory initialization (**INIT_HEAP**), memory diagnostics (**DUMP_MEMORY**), memory allocation (**MALLOC**), memory deallocation (**FREE**), writing to memory (**WRITE**), and reading from memory (**READ**).  

## Comments on the Project:  

### What did you learn from completing this project?  

* How memory works in the background and how certain memory operations are performed.  

* A better understanding of a data structure (*Segregated Free Lists*).  

* How to better understand and implement a **resizable array**.  
