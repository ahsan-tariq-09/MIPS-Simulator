#ifndef CACHE_H
#define CACHE_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  bool valid;
  uint32_t tag;
} CacheLine;

typedef struct {
  CacheLine* lines;
  uint32_t num_lines;   // must be power of 2
  uint32_t block_size;  // bytes, power of 2
  uint64_t requests;
  uint64_t hits;
  bool enabled;
} Cache;

Cache cache_create(uint32_t num_lines, uint32_t block_size, bool enabled);
void cache_free(Cache* c);

// returns true if hit, false if miss
bool cache_access(Cache* c, uint32_t addr);

void cache_print_stats(Cache* c);

#endif