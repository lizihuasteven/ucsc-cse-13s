#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pq.h"

typedef struct PriorityQueueNode {
  char *author;
  double dist;
} PriorityQueueNode;

struct PriorityQueue {
  uint32_t capacity;
  uint32_t size;
  PriorityQueueNode **nodes;
};

// The constructor for a priority queue that holds up to capacity
// elements. In the event that sufficient Memory cannot be allocated,
// the function must return NULL. Else, it must return a PriorityQueue
// *,or a pointer to an allocated PriorityQueue. The priority queue
// should initially contain no elements.

PriorityQueue *pq_create(uint32_t capacity) {
  PriorityQueue *pq = (PriorityQueue *)malloc(sizeof(PriorityQueue));
  assert(pq);
  pq->capacity = capacity;
  pq->size = 0;
  pq->nodes = (PriorityQueueNode **)calloc(sizeof(PriorityQueueNode *), pq->capacity);
  assert(pq->nodes);
  return pq;
}

// The destructor for a priority queue. Remember to set the pointer to
// NULL after the memory associated with the priority queue is
// freed. Anything left in the priority queue that hasn’t been
// dequeued should be freed as well.

void pq_delete(PriorityQueue **q) {
  assert(q);
  assert(*q);
  assert((*q)->nodes);
  for (uint32_t i = 0; i < (*q)->capacity; i++) {
    free((*q)->nodes[i]);
    (*q)->nodes[i] = NULL;
  }
  free((*q)->nodes);
  (*q)->nodes = NULL;
  free(*q);
  *q = NULL;
}

// Returns true if the priority queue is empty and false otherwise.

bool pq_empty(PriorityQueue *q) {
  assert(q);
  return q->size == 0;
}

// Returns true if the priority queue is full and false otherwise.

bool pq_full(PriorityQueue *q) {
  assert(q);
  return q->size == q->capacity;
}

// Returns the number of elements in the priority queue.

uint32_t pq_size(PriorityQueue *q) {
  assert(q);
  return q->size;
}

// Enqueue the author, dist pair into the priority queue. If the
// priority queue is full, return false. Otherwise, return true to
// indicate success.

int comparePriorityQueueNodes(PriorityQueueNode **l, PriorityQueueNode **r) {
  assert(l);
  assert(*l);
  assert(r);
  assert(*r);

  if ((*l)->dist < (*r)->dist) {
    return -1;
  }
  if ((*l)->dist > (*r)->dist) {
    return 1;
  }
  return 0;
}

bool enqueue(PriorityQueue *q, char *author, double dist) {
  assert(q);
  assert(q->nodes);
  assert(author);
  if (pq_full(q)) {
    return false;
  }
  PriorityQueueNode *node = (PriorityQueueNode *)malloc(sizeof(PriorityQueueNode));
  assert(node);
  node->author = strdup(author);
  node->dist = dist;
  q->nodes[q->size++] = node;
  qsort(q->nodes, q->size, sizeof(q->nodes[0]), (int (*)(void const *, void const *)) comparePriorityQueueNodes);
  return true;
}

// Dequeue the author, dist pair from the priority queue. The pointer
// to the author string is passed back with the author double
// pointer. The distance metric value is passed back with the dist
// pointer. If the priority queue is empty, return false. Otherwise,
// return true to indicate success.

bool dequeue(PriorityQueue *q, char **author, double *dist) {
  assert(q);
  assert(q->nodes);
  assert(author);
  assert(*author);
  assert(dist);
  if (pq_empty(q)) {
    return false;
  }
  PriorityQueueNode *node = q->nodes[0];
  uint32_t i;
  for (i = 0; i < (q->size - 1); i++) {
    q->nodes[i] = q->nodes[i + 1];
  }
  q->nodes[i] = NULL;

  *author = node->author;
  *dist = node->dist;
  free(node);
  return true;
}

// A debug function to print the priority queue.

void pq_print(PriorityQueue *q) {
  assert(q);
  assert(q->nodes);
  printf("(PriorityQueue *)%p->{ capacity=%d, size=%d, nodes=[", (void *)q, q->capacity, q->size);

  char const *comma = "";
  for (uint32_t i = 0; i < q->size; i++) {
    PriorityQueueNode *node = q->nodes[i];
    printf("%s{ author=\"%s\", dist=%g\n", comma, node->author, node->dist);
    comma = ", ";
  }
  printf("]}");
}
