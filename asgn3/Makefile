SHELL := /bin/sh
CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: sorting

sorting: sorting.o
	$(CC) -o sorting sorting.o -lm

sorting.o: sorting.c
	$(CC) $(CFLAGS) -c sorting.c

clean:
	rm -f sorting *.o

format:
	clang-format -i -style=file *.[ch]
