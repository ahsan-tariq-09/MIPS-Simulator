#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stdbool.h>

typedef struct {
  uint8_t* bytes;
  uint32_t size;
} Memory;

Memory mem_create(uint32_t size);
void mem_free(Memory* m);

bool mem_load_word(Memory* m, uint32_t addr, int32_t* out);
bool mem_store_word(Memory* m, uint32_t addr, int32_t val);

#endif