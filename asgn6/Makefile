# 1. Makefile:
# • CFLAGS=-Wall -Wextra -Werror -Wpedantic must be included.
# • CC=clang must be specified.
# • make clean must remove all files that are compiler generated.
# • make encode should build your encode program.
# • make decode should build your decode program.
# • make should build both encode and decode, as should make all.
# • Your programs should have no memory leaks.

CFLAGS=-Wall -Wextra -Werror -Wpedantic
OPT=-g
CC=clang

all:	encode decode

encode:	encode.o io.o trie.o word.o
	$(CC) $(CFLAGS) $(OPT) -o encode encode.o io.o trie.o word.o

decode:	decode.o io.o trie.o word.o
	$(CC) $(CFLAGS) $(OPT) -o decode decode.o io.o trie.o word.o

encode.o:	encode.c code.h io.h trie.h word.h
	$(CC) $(CFLAGS) $(OPT) -c encode.c

decode.o:	decode.c code.h io.h trie.h word.h
	$(CC) $(CFLAGS) $(OPT) -c decode.c

io.o:	io.c io.h code.h endian.h
	$(CC) $(CFLAGS) $(OPT) -c io.c

trie.o:	trie.c trie.h
	$(CC) $(CFLAGS) $(OPT) -c trie.c

word.o:	word.c word.h
	$(CC) $(CFLAGS) $(OPT) -c word.c

clean:
	$(RM) encode decode *.o

format:
	clang-format -i -style=file *.[ch]
