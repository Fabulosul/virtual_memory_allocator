# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -Werror -g

# Output executable
TARGET = sfl

# Source files
SRCS = destroy_heap.c \
       dump_memory.c \
       free_functions.c \
       generic_list_functions.c \
       helper_functions.c \
       init_heap.c \
       malloc.c \
       read.c \
       write.c \
	   sfl.c

# Header files (optional: for dependencies)
HEADERS = destroy_heap.h \
          dump_memory.h \
          free_functions.h \
          generic_list_functions.h \
          helper_functions.h \
          init_heap.h \
          malloc.h \
          read.h \
          structs.h \
          write.h

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
