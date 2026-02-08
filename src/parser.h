#ifndef PARSER_H
#define PARSER_H

#include "sim.h"
#include <stddef.h>

typedef struct {
  uint32_t addr;
  uint8_t* bytes;
  size_t len;
} DataInit;

typedef struct {
  Instr* program;
  size_t count;

  DataInit* data;
  size_t data_count;
} Program;

Program parse_asm_file(const char* path);
void free_program(Program* p);

#endif
