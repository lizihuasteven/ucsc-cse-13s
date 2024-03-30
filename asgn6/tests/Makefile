TEST_TRIE = test_trie
TEST_TRIE_OBJ = test_trie.o
TEST_WORD = test_word
TEST_WORD_OBJ = test_word.o
TEST_IO = test_io
TEST_IO_OBJ = test_io.o

TESTS =$(TEST_TRIE) $(TEST_WORD) $(TEST_IO)
TEST_OBJS = $(TESTS:%=%.o)

SOURCES := $(wildcard ../*.c)
ASGN_SRC := ${shell grep -L "int main(" ${SOURCES}}
ASGN_OBJ = $(ASGN_SRC:%.c=%.o)

CC = clang
CHECK_CFLAGS = $(shell pkg-config --cflags check)
CHECK_LFLAGS = $(shell pkg-config --libs check)

all: build_src $(TESTS)
	touch testfile.txt writetest.txt pairs.txt words.txt
	-./$(TEST_TRIE)
	-./$(TEST_WORD)
	-./$(TEST_IO)

build_src:
	-make -C ../

$(TEST_IO): $(ASGN_OBJ) $(TEST_IO_OBJ)
	$(CC) -o $@ $^ $(CHECK_LFLAGS) -lm -lpthread
$(TEST_WORD): $(ASGN_OBJ) $(TEST_WORD_OBJ)
	$(CC) -o $@ $^ $(CHECK_LFLAGS) -lm -lpthread
$(TEST_TRIE): $(ASGN_OBJ) $(TEST_TRIE_OBJ)
	$(CC) -o $@ $^ $(CHECK_LFLAGS) -lm -lpthread

%.o: %.c
	$(CC) $(CHECK_CFLAGS) -c $< -o $@

clean:
	-rm -f $(TESTS) $(TEST_OBJS)
	-rm -f *.tap *.txt
	-make clean -C ../
