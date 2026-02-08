#include "memory.h"
#include <stdlib.h>
#include <string.h>

Memory mem_create(uint32_t size) {
  Memory m;
  m.bytes = (uint8_t*)calloc(size, 1);
  m.size = size;
  return m;
}

void mem_free(Memory* m) {
  if (!m) return;
  free(m->bytes);
  m->bytes = NULL;
  m->size = 0;
}

static bool check_addr(Memory* m, uint32_t addr) {
  return m && m->bytes && (addr + 3) < m->size && (addr % 4 == 0);
}

// Little-endian word load/store.
bool mem_load_word(Memory* m, uint32_t addr, int32_t* out) {
  if (!check_addr(m, addr) || !out) return false;
  uint32_t b0 = m->bytes[addr + 0];
  uint32_t b1 = m->bytes[addr + 1];
  uint32_t b2 = m->bytes[addr + 2];
  uint32_t b3 = m->bytes[addr + 3];
  uint32_t u = (b0) | (b1 << 8) | (b2 << 16) | (b3 << 24);
  *out = (int32_t)u;
  return true;
}

bool mem_store_word(Memory* m, uint32_t addr, int32_t val) {
  if (!check_addr(m, addr)) return false;
  uint32_t u = (uint32_t)val;
  m->bytes[addr + 0] = (uint8_t)(u & 0xFF);
  m->bytes[addr + 1] = (uint8_t)((u >> 8) & 0xFF);
  m->bytes[addr + 2] = (uint8_t)((u >> 16) & 0xFF);
  m->bytes[addr + 3] = (uint8_t)((u >> 24) & 0xFF);
  return true;
}

static bool check_addr_byte(Memory* m, uint32_t addr) {
  return m && m->bytes && addr < m->size;
}

bool mem_load_byte(Memory* m, uint32_t addr, uint8_t* out) {
  if (!check_addr_byte(m, addr) || !out) return false;
  *out = m->bytes[addr];
  return true;
}

bool mem_store_byte(Memory* m, uint32_t addr, uint8_t val) {
  if (!check_addr_byte(m, addr)) return false;
  m->bytes[addr] = val;
  return true;
}

static bool check_addr_byte(Memory* m, uint32_t addr) {
  return m && m->bytes && addr < m->size;
}

bool mem_load_byte(Memory* m, uint32_t addr, uint8_t* out) {
  if (!check_addr_byte(m, addr) || !out) return false;
  *out = m->bytes[addr];
  return true;
}

bool mem_store_byte(Memory* m, uint32_t addr, uint8_t val) {
  if (!check_addr_byte(m, addr)) return false;
  m->bytes[addr] = val;
  return true;
}

static bool check_addr_byte(Memory* m, uint32_t addr) {
  return m && m->bytes && addr < m->size;
}

bool mem_load_byte(Memory* m, uint32_t addr, uint8_t* out) {
  if (!check_addr_byte(m, addr) || !out) return false;
  *out = m->bytes[addr];
  return true;
}

bool mem_store_byte(Memory* m, uint32_t addr, uint8_t val) {
  if (!check_addr_byte(m, addr)) return false;
  m->bytes[addr] = val;
  return true;
}

static bool check_addr_byte(Memory* m, uint32_t addr) {
  return m && m->bytes && addr < m->size;
}

bool mem_load_byte(Memory* m, uint32_t addr, uint8_t* out) {
  if (!check_addr_byte(m, addr) || !out) return false;
  *out = m->bytes[addr];
  return true;
}

bool mem_store_byte(Memory* m, uint32_t addr, uint8_t val) {
  if (!check_addr_byte(m, addr)) return false;
  m->bytes[addr] = val;
  return true;
}
