#Copyright Tudor Robert-Fabian 312CAa 2023-2024                                 
# compiler setup                                                                
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

# define targets                                                                
TARGETS = sfl

build: sfl.c
	$(CC) $(CFLAGS) sfl.c -o sfl 

run_sfl: $(TARGETS) 
	./sfl

clean:
	rm -f $(TARGETS) *.o

.PHONY: pack clean
