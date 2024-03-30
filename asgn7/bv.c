#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "bv.h"

struct BitVector {
  uint32_t length;
  uint8_t *vector;
};

// The constructor for a bit vector that holds length bits. In the
// even that sufficient memory cannot be allocated, the function must
// return NULL. Else, it must return a BitVector *, or a pointer to an
// allocated BitVector. Each bit of the bit vector should be
// initialized to 0.

BitVector *bv_create(uint32_t length) {
  BitVector *bv = (BitVector *)malloc(sizeof(BitVector));

  assert(bv);
  bv->length = length;

  uint32_t byteLength = (length / 8) + 1;

  bv->vector = (uint8_t *)calloc(sizeof(uint8_t), byteLength);
  assert(bv->vector);
  return bv;
}

// The destructor for a bit vector. Remember to set the pointer to
// NULL after the memory associated with the bit vector is freed.

void bv_delete(BitVector **bv) {
  assert(bv);
  assert(*bv);
  free((*bv)->vector);
  (*bv)->vector = NULL;
  free(*bv);
  *bv = NULL;
}

// Returns the length of a bit vector.

uint32_t bv_length(BitVector *bv) {
  assert(bv);
  return bv->length;
}

// Sets the ith bit in a bit vector. If i is out of range, return
// false. Otherwise, return true to indicate success.

bool bv_set_bit(BitVector *bv, uint32_t i) {
  assert(bv);
  assert(bv->vector);
  if (i < bv->length) {
    uint32_t byteIndex = i / 8;
    uint32_t bitOffset = i % 8;
    bv->vector[byteIndex] |= 1 << bitOffset;
    return true;
  }
  return false;
}

// Clears the i th bit in the bit vector. If i is out of range, return
// false. Otherwise, return true to indicate success.

bool bv_clr_bit(BitVector *bv, uint32_t i) {
  assert(bv);
  if (i < bv->length) {
    uint32_t byteIndex = i / 8;
    uint32_t bitOffset = i % 8;
    bv->vector[byteIndex] &= ~(1 << bitOffset);
    return true;
  }
  return false;
}

// Returns the i th bit in the bit vector. If i is out of range,
// return false. Otherwise, return false if the value of bit i is 0
// and return true if the value of bit i is 1.

bool bv_get_bit(BitVector *bv, uint32_t i) {
  assert(bv);
  if (i < bv->length) {
    uint32_t byteIndex = i / 8;
    uint32_t bitOffset = i % 8;
    return (bv->vector[byteIndex] & ~(1 << bitOffset)) != 0;
  }
  return false;
}

// A debug function to print the bits of a bit vector. That is,
// iterate over each of the bits of the bit vector. Print out either 0
// or 1 depending on whether each bit is set. You should write this
// immediately after the constructor.

void bv_print(BitVector *bv) {
  assert(bv);
  printf("(BitVector *)%p=[%d", (void *)bv, bv_get_bit(bv, 0));
  for (uint32_t i = 1; i < bv->length; i++) {
    if (i % 8 == 0) {
      printf(",");
    }
    printf("%d", bv_get_bit(bv, i));
  }
  printf("]");
}
