#ifndef PARSER_H
#define PARSER_H

#include "sim.h"
#include <stddef.h>

typedef struct {
  char name[64];
  uint32_t pc; // byte address
} Label;

typedef struct {
  Instr* program;
  size_t count;
} Program;

Program parse_asm_file(const char* path);
void free_program(Program* p);

#endif