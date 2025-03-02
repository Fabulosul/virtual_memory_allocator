# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

# Output executable
TARGET = sfl

# Source files with correct paths
SRCS = destroy_heap/destroy_heap.c \
       dump_memory/dump_memory.c \
       free_functions/free_functions.c \
       generic_list_functions/generic_list_functions.c \
       helper_functions/helper_functions.c \
       init_heap/init_heap.c \
       malloc/malloc.c \
       read/read.c \
       write/write.c \
       sfl.c

# Header files (for dependencies)
HEADERS = destroy_heap/destroy_heap.h \
          dump_memory/dump_memory.h \
          free_functions/free_functions.h \
          generic_list_functions/generic_list_functions.h \
          helper_functions/helper_functions.h \
          init_heap/init_heap.h \
          malloc/malloc.h \
          read/read.h \
          structs.h \
          write/write.h

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Rule for compiling source files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET)

# Phony targets
.PHONY: all clean
