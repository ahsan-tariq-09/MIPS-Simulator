#ifndef SIM_H
#define SIM_H

#include <stdint.h>
#include <stdbool.h>

#define NUM_REGS 32

// Simple memory size. Enough for typical class programs.
// If your class needs more, bump it.
#define MEM_SIZE (1u << 20) // 1 MiB

typedef enum {
  OP_ADD, OP_SUB, OP_AND, OP_OR, OP_SLT,
  OP_ADDI, OP_ANDI, OP_ORI,
  OP_LW, OP_SW,
  OP_BEQ, OP_BNE, OP_BGE, OP_BLT, OP_BGT, OP_BLE,
  OP_J, OP_JAL, OP_JR,
  OP_SYSCALL,
  OP_NOP,
  OP_INVALID
} Op;

typedef struct {
  Op op;

  // Common fields.
  int rd, rs, rt;
  int32_t imm;        // immediate / offset
  uint32_t target_pc; // absolute byte address for jumps/branches (resolved)
  char raw[128];      // original line for debugging
} Instr;

typedef struct {
  uint32_t pc;                 // byte address
  int32_t regs[NUM_REGS];      // MIPS regs as signed 32-bit
  bool running;
} CPUState;

#endif