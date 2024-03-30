#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "node.h"

// The constructor for a node. You will want to make a copy of the
// word that is passed in. This will require allocating memory and
// copying over the characters for the word. You may find strdup()
// useful.

Node *node_create(char *word) {
  Node *n = (Node *)malloc(sizeof(Node));

  assert(n);
  n->word = strdup(word);
  n->count = 0;
  return n;
}

// The destructor for a node. Since you have allocated memory for
// word, remember to free the memory allocated to that as well. The
// pointer to the node should be set to NULL.

void node_delete(Node **n) {
  free((*n)->word);
  (*n)->word = NULL;
  free(*n);
  *n = NULL;
}

// A debug function to print out the contents of a node.

void node_print(Node *n) {
  printf("(Node *)%p->{ word=\"%s\", count=%d }",
	 (void *) n, n->word, n->count
	);
}
