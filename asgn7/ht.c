#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ht.h"
#include "salts.h"
#include "speck.h"

// Below is the struct definition for a hash table. The hash table
// contains a salt that will be passed to the given hash function,
// hash() — more on this in §5. Each hash table entry is a Node that
// contains a unique word and its count: the number of times is
// appears in a sample of text. The node ADT will be discussed in
// §6. Since we are resolving hash collisions with open addressing, it
// only makes sense to store these nodes in an array.
  
struct HashTable {
  uint64_t salt[2];	     // The salt to use for the hash function.
  uint32_t size;             // The number of slots in the hash table.
  uint32_t insertions;	     // The number of insertions made.
  Node **slots;		     // The array of hash table items.
};

// The constructor for a hash table. The size parameter denotes the
// number of slots that the hash table can index up to. The salt for
// the hash table is provided in salts.h.

HashTable *ht_create(uint32_t size) {
  HashTable *ht = (HashTable *)malloc(sizeof(HashTable));

  assert(ht);
  ht->salt[0] = SALT_HASHTABLE_LO;
  ht->salt[1] = SALT_HASHTABLE_HI;
  ht->size = size;
  ht->insertions = 0;
  ht->slots = (Node **)calloc(sizeof(Node *), size);
  assert(ht->slots);
  return ht;
}

// The destructor for a hash table. This should free any remaining
// nodes left in the hash table. Remember to set the pointer to the
// freed hash table to NULL.

void ht_delete(HashTable **ht) {
  assert(ht);
  assert(*ht);

  if ((*ht)->slots && (*ht)->size > 0) {
    for (uint32_t i = 0; i < (*ht)->size; i++) {
      if ((*ht)->slots[i]) {
	node_delete(&(*ht)->slots[i]);
      }
    }
    (*ht)->size = 0;
    free((*ht)->slots);
    (*ht)->slots = NULL;
  }
  free(*ht);
  *ht = NULL;
}

// Returns the hash table’s size, the number of slots it can index up to.

uint32_t ht_size(HashTable *ht) {
  assert(ht);
  return ht->size;
}

// Searches for an entry, a node, in the hash table that contains
// word. If the node is found, the pointer to the node is
// returned. Otherwise, a NULL pointer is returned.

Node *ht_lookup(HashTable *ht, char *word) {
  assert(ht);

  if (ht->insertions == 0) {
    return NULL;
  }

  // Get hash value and, from that, the initial index to probe.

  uint32_t h = hash(ht->salt, word);
  uint32_t hh = h % ht->size;

  // Linear probing.

  for (uint32_t i = 0; i < ht->size; i++) {
    Node *node = ht->slots[(hh + i) % ht->size];

    // If we encounter a NULL slot, then the word cannot be in the
    // table.

    if (!node) {
      return NULL;
    }

    // However, just because the slot isn't NULL means that *this*
    // word is in the slot...

    if (strcmp(node->word, word) == 0) {
      return node;
    }
  }
  return NULL;
}

// Inserts the specified word into the hash table. If the word already
// exists in the hash table, increment its count by 1. Otherwise,
// insert a new node containing word with its count set to 1. Again,
// since we’re using open addressing, it’s possible that an insertion
// fails if the hash table is filled. To indicate this, return a
// pointer to the inserted node if the insertion was successful, and
// return NULL if unsuccessful.

Node *ht_insert(HashTable *ht, char *word) {
  assert(ht);
  assert(word);
  assert(*word);

  Node *node = ht_lookup(ht, word);

  if (!node) {
    uint32_t h = hash(ht->salt, word);
    uint32_t hh = h % ht->size;

    for (uint32_t i = 0; i < ht->size; i++) {
      node = ht->slots[(hh + i) % ht->size];
      if (!node) {
	node = node_create(word);
	assert(node);
	ht->slots[(hh + i) % ht->size] = node;
	break;
      }
    }
    if (!node) {
      return NULL;
    }
    ht->insertions++;
  }
  node->count++;
  return node;
}

// A debug function to print out the contents of a hash table. Write
// this immediately after the constructor.

void ht_print(HashTable *ht) {
  assert(ht);

  printf("(HashTable *)%p->{ salt=[0x%16lx, 0x%16lx], size=%u, insertions=%u, slots=[",
	 (void *) ht, ht->salt[0], ht->salt[1], ht->size, ht->insertions
	);
  // char const *comma = "";
  // for (uint32_t i = 0; i < ht->size; i++) {
  //   Node *n = ht->slots[i];
  // 
  //   if (n) {
  //     printf("%s[%d]", comma, i);
  //     node_print(n);
  //     comma = ", ";
  //   }
  // }
  printf("...]");
  printf(" }");
}

uint32_t ht_insertion_count(HashTable *ht) {
  assert(ht);
  return ht->insertions;
}

// 4 Iterating Over Hash Tables

// You will need some mechanism to iterate over all the entries in a
// hash table. High-level languages such as Python and Rust natively
// provide iterators to do exactly this. C, on the other hand,
// provides no such utility. This means that you will need to
// implement your own hash table iterator type. This will be called
// HashTableIterator and should be defined in ht.c. The hash table and
// the hash table iterator should be implemented in the same file. You
// must use the following struct definition for your hash table
// iterator.

struct HashTableIterator {
  HashTable *table;	       // The hash table to iterate over.
  uint32_t slot;	       // The current slot the iterator is on.
  uint32_t remaining;	       // The number of insertions remaining to be returned
};

// Storing each hash table entry in its own array index makes the hash
// table iterator implementation almost trivial. All an iterator needs
// to do is keep track of which slot it has iterated up to in the hash
// table. The following sections are the functions that must be
// implemented for your hash table iterator interface.

// Creates a new hash table iterator. This iterator should iterate
// over the ht. The slot field of the iterator should be initialized
// to 0.

HashTableIterator *hti_create(HashTable *ht) {
  assert(ht);

  HashTableIterator *hti = (HashTableIterator *)malloc(sizeof(HashTableIterator));
  assert(hti);
  hti->table = ht;
  hti->slot = 0;
  hti->remaining = ht->insertions;
  return hti;
}

// Deletes a hash table iterator. You should not delete the table
// field of the iterator, as you may need to iterate over that hash
// table at a later time.

void hti_delete(HashTableIterator **hti) {
  assert(hti);
  assert(*hti);
  (*hti)->table = NULL;
  free(*hti);
}

// Returns the pointer to the next valid entry in the hash table. This
// may require incrementing the slot field of the iterator multiple
// times to get to the next valid entry. Return NULL if the iterator
// has iterated over the entire hash table.

// Example hash table iterator usage.

#if 0 
	HashTable *ht = ht_create(4);
	ht_insert(ht, "hello");
	ht_insert(ht, "world");
	HashTableIterator *hti = hti_create(ht);
	Node *n = NULL;
	while ((n = ht_iter(hti)) != NULL) {
	  print("%s\n", n->word);
	}
	hti_delete(&hti);
	ht_delete(&ht);
#endif

// The above code should print either "hello" then "world" or "world"
// then "hello" depending on how the hash function works.

Node *ht_iter(HashTableIterator *hti) {
  assert(hti);
  assert(hti->table);

  if (hti->table->insertions) {
    for (/* Empty */ ; hti->remaining > 0 && hti->slot < hti->table->size; hti->slot = (hti->slot + 1) % hti->table->size) {
      Node *node = hti->table->slots[hti->slot];
      if (node) {
	hti->slot = (hti->slot + 1) % hti->table->size;
	hti->remaining--;
	return node;
      }
    }
  }
  return NULL;
}

