#include <string.h>

#include "speck.h"

#include <stdint.h>

#define ROUNDS 22
#define ALPHA 8
#define BETA 3

uint32_t hash(uint64_t *salt, char *key) {
  size_t key_len = strlen(key);
  uint32_t b = key_len / sizeof(uint64_t) + 1;
  uint64_t k[b];
  uint64_t q[b];
  memset(q, 0, sizeof(q));
  strcpy((char *)q, key);
  uint64_t *w = (uint64_t *)q;

  // Expand key
  k[0] = salt[0];
  for (size_t i = 0; i < b - 1; i++) {
    k[i + 1] = w[i];
  }
  k[b - 1] = key_len;

  // Initialize state
  uint32_t y = salt[1];
  uint32_t x[b];
  x[0] = y;
  for (size_t i = 0; i < b - 1; i++) {
    x[i + 1] = k[i];
  }

  // Perform rounds
  for (uint32_t i = 0; i < ROUNDS; i++) {
    x[0] = (x[0] + y) ^ (i & 0xFFFFFFFF);
    y = (y + x[0]) ^ (i << 8);
    for (size_t j = 0; j < b; j++) {
      uint32_t tmp = x[j];
      x[j] = (y >> ALPHA | y << (32 - ALPHA)) + x[(j + b - 1) % b];
      y = (tmp >> BETA | tmp << (32 - BETA)) ^ x[j];
    }
  }

  return y;
}
