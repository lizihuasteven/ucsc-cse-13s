SHELL := /bin/sh
CC = clang
CFLAGS = -Wall -Wextra -Werror -Wpedantic

all: life

life: life.o
	$(CC) -lncurses -o life life.o -lm

life.o: life.c
	$(CC) $(CFLAGS) -c life.c

clean:
	rm -f life *.o

format:
	clang-format -i -style=file *.[ch]
