CC = clang
#CC = gcc
CFLAGS = -O0 -g -Wall -Wextra -Werror -Wpedantic -fsanitize=address

all:	identify

identify: bf.o bv.o ht.o identify.o node.o parser.o pq.o speck.o text.o
	$(CC) $(CFLAGS) -o identify bf.o bv.o ht.o identify.o node.o parser.o pq.o speck.o text.o

#  1. bf.h: Defines the interface for the Bloomfilter ADT. Do not
#  modify this.

bf.h:	bv.h

#  2. bf.c: Contains the implementation of the Bloomfilter ADT.

bf.o:	bf.c bf.h
	$(CC) $(CFLAGS) -c bf.c

#  3. bv.h: Defines the interface for the bitvector ADT. Do not modify
#  this.

bv.h:

#  4. bv.c: Contains the implementation of the bitvector ADT.

bv.o:	bv.c bv.h
	$(CC) $(CFLAGS) -c bv.c

#  5. ht.h: Defines the interface for the hash table ADT and the
#  hashtable iterator ADT. Do not modify this.

ht.h:	node.h

#  6. ht.c: Contains the implementation of the hash table ADT and the
#  hashtable iterator ADT.

ht.o:	ht.c ht.h
	$(CC) $(CFLAGS) -c ht.c

#  7. identify.c: Contains main() and the implementation of the author
#  identification program.

identify.o:	identify.c
	$(CC) $(CFLAGS) -c identify.c

#  8. metric.h: Defines the enumeration for the distance metrics and
#  their respective names stored in an array of strings. Do not modify
#  this.

metric.h:

#  9. node.h: Defines the interface for thenode ADT. Do not modify
#  this.

node.h:

# 10. node.c: Contains the implementation of the node ADT.

node.o:	node.c node.h
	$(CC) $(CFLAGS) -c node.c

# 11. parser.h: Defines the interface for the regex parsing module. Do
# not modify this.

parser.h:

# 12. parser.c: Contains the implementation of the regex parsing module.

parser.o:	parser.c parser.h
	$(CC) $(CFLAGS) -c parser.c

# 13. pq.h: Defines the interface for the priority queue ADT. Do not modify this.

pq.h:

# 14. pq.c: Contains the implementation for the priority queue ADT.

pq.o:	pq.c pq.h
	$(CC) $(CFLAGS) -c pq.c

# 15. salts.h: Defines the primary, secondary, and tertiary salts to be used in your Bloom filter implementation. Also defines the salt used by the hash table in your hash table implementation.

salts.h:

# 16. speck.h: Defines the interface for the hash function using the SPECK cipher. Do not modify this.

speck.h:

# 17. speck.c: Contains the implementation of the hash function using the SPECK cipher. Do not modify this.

speck.o:	speck.c speck.h
	$(CC) $(CFLAGS) -c speck.c

# 18. text.h: Defines the interface for the text ADT. Do not modify this.

text.h:

# 19. text.c: Contains the implementation for the text ADT.

text.o:	text.c text.h
	$(CC) $(CFLAGS) -c text.c

clean:
	$(RM) identify bf.o bv.o ht.o identify.o node.o parser.o pq.o speck.o text.o
	
format:
	clang-format -i -style=file *.[ch]
