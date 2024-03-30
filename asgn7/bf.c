#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bf.h"
#include "bv.h"
#include "speck.h"
#include "salts.h"

// The following struct defines the BloomFilter ADT. The three salts
// will be stored in the primary, secondary, and tertiary fields. Each
// salt is 128 bits in size. To hold these 128 bits, we use an array
// of two uint64_ts.

struct BloomFilter {
  uint64_t primary[2];		// Primary hash function salt. 
  uint64_t secondary[2];	// Secondary hash function salt.
  uint64_t tertiary[2];		// Tertiary hash function salt.
  BitVector *filter;
};

// The constructor for a Bloom filter. The primary, secondary, and
// tertiary salts that should be used are provided in salts.h. Note
// that you will also have to implement the bit vector ADT for your
// Bloom filter, as it will serve as the array of bits necessary for a
// proper Bloom filter. Bit vectors will be discussed in §8.

BloomFilter *bf_create(uint32_t size) {
  BloomFilter *bf = (BloomFilter *)malloc(sizeof(BloomFilter));
  bf->primary[0] = SALT_PRIMARY_LO;
  bf->primary[1] = SALT_PRIMARY_HI;
  bf->secondary[0] = SALT_SECONDARY_LO;
  bf->secondary[1] = SALT_SECONDARY_HI;
  bf->tertiary[0] = SALT_TERTIARY_LO;
  bf->tertiary[1] = SALT_TERTIARY_HI;
  bf->filter = bv_create(size);
  return bf;
}

// The destructor for a Bloom filter. As with all other destructors,
// it should free any memory allocated by the constructor and null out
// the pointer that was passed in.

void bf_delete(BloomFilter **bf) {
  assert(bf);
  assert(*bf);
  bv_delete(&(*bf)->filter);
  (*bf)->filter = NULL;
  free(*bf);
  *bf = NULL;
}

// Returns the size of the Bloom filter. In other words, the number of
// bits that the Bloom filter can access.  Hint: this is the length of
// the underlying bit vector.

uint32_t bf_size(BloomFilter *bf) {
  assert(bf);
  assert(bf->filter);
  return bv_length(bf->filter);
}

// Takes word and inserts it into the Bloom filter. This entails
// hashing word with each of the three salts for three indices, and
// setting the bits at those indices in the underlying bit vector.

void bf_insert(BloomFilter *bf, char *oldspeak) {
  assert(bf);
  assert(bf->filter);
  assert(oldspeak);

  uint32_t bfSize = bf_size(bf);

  uint32_t h = hash(bf->primary, oldspeak);
  bv_set_bit(bf->filter, h % bfSize);

  h = hash(bf->secondary, oldspeak);
  bv_set_bit(bf->filter, h % bfSize);

  h = hash(bf->tertiary, oldspeak);
  bv_set_bit(bf->filter, h % bfSize);
}

// Probes the Bloom filter for word. Like with bf_insert(), word is
// hashed with each of the three salts for three indices. If all the
// bits at those indices are set, return true to signify that word was
// most likely added to the Bloom filter. Else, return false.

bool bf_probe(BloomFilter *bf, char *oldspeak) {
  assert(bf);
  assert(bf->filter);
  assert(oldspeak);

  uint32_t bfSize = bf_size(bf);

  uint32_t h = hash(bf->primary, oldspeak);
  if (!bv_get_bit(bf->filter, h % bfSize)) {
    return false;
  }

  h = hash(bf->secondary, oldspeak);
  if (!bv_get_bit(bf->filter, h % bfSize)) {
    return false;
  }

  h = hash(bf->tertiary, oldspeak);
  return bv_get_bit(bf->filter, h % bfSize);
}

// A debug function to print out the bits of a Bloom filter. This will
// ideally utilize the debug print function you implement for your bit
// vector.

void bf_print(BloomFilter *bf) {
  assert(bf);
  assert(bf->filter);

  printf("(BloomFilter *)%p->{ "
	 "primary=[0x%16lx, 0x%16lx], "
	 "secondary=[0x%16lx, 0x%16lx], "
	 "tertiary=[0x%16lx, 0x%16lx], "
	 "vector=",
	 (void *) bf,
	 bf->primary[0], bf->primary[1],
	 bf->secondary[0], bf->secondary[1],
	 bf->tertiary[0], bf->tertiary[1]
	);
  // bv_print(bf->filter);
  printf("...");
  printf(" }");
}
