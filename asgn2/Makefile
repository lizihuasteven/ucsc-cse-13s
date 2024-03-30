SHELL := /bin/sh
CC = clang
CFLAGS = -Wall -Wpedantic -Werror -Wextra

all: mathlib-test

mathlib-test: mathlib-test.o
	$(CC) -o mathlib-test mathlib-test.o -lm

monte_carlo.o: mathlib-test.c
	$(CC) $(CFLAGS) -c mathlib-test.c

clean:
	rm -f mathlib-test *.o

format:
	clang-format -i -style=file *.[ch]
