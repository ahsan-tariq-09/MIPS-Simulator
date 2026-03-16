#include "cache.h"
#include <stdlib.h>
#include <stdio.h>

static bool is_pow2(uint32_t x) { return x && ((x & (x - 1)) == 0); }

Cache cache_create(uint32_t num_lines, uint32_t block_size, bool enabled) {
  Cache c;
  c.enabled = enabled;
  c.requests = 0;
  c.hits = 0;
  c.num_lines = num_lines;
  c.block_size = block_size;
  c.lines = NULL;

  if (!enabled) return c;

  if (!is_pow2(num_lines) || !is_pow2(block_size)) {
    fprintf(stderr, "[cache] num_lines and block_size must be powers of two.\n");
    exit(1);
  }

  c.lines = (CacheLine*)calloc(num_lines, sizeof(CacheLine));
  return c;
}

void cache_free(Cache* c) {
  if (!c) return;
  free(c->lines);
  c->lines = NULL;
}

bool cache_access(Cache* c, uint32_t addr) {
  if (!c || !c->enabled) return false;

  c->requests++;

  uint32_t block_addr = addr / c->block_size;
  uint32_t index = block_addr & (c->num_lines - 1);
  uint32_t tag = block_addr / c->num_lines;

  CacheLine* line = &c->lines[index];
  if (line->valid && line->tag == tag) {
    c->hits++;
    return true;
  }
  line->valid = true;
  line->tag = tag;
  return false;
}

void cache_print_stats(Cache* c) {
  if (!c || !c->enabled) return;
  double hit_rate = (c->requests == 0) ? 0.0 : (100.0 * (double)c->hits / (double)c->requests);
  printf("\n[cache] requests=%llu hits=%llu hit_rate=%.2f%%\n",
         (unsigned long long)c->requests,
         (unsigned long long)c->hits,
         hit_rate);
}