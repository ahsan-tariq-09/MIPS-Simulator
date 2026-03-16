#include "cpu.h"
#include "syscall.h"
#include <stdio.h>

static inline void write_reg(CPUState* cpu, int r, int32_t v) {
  if (r == 0) return; // $zero is always 0
  cpu->regs[r] = v;
}

static bool branch_taken(Op op, int32_t a, int32_t b) {
  switch (op) {
    case OP_BEQ: return a == b;
    case OP_BNE: return a != b;
    case OP_BGE: return a >= b;
    case OP_BLT: return a <  b;
    case OP_BGT: return a >  b;
    case OP_BLE: return a <= b;
    default: return false;
  }
}

void cpu_init(CPUState* cpu) {
  cpu->pc = 0;
  for (int i = 0; i < NUM_REGS; i++) cpu->regs[i] = 0;

  cpu->hi = 0;
  cpu->lo = 0;

  cpu->regs[29] = (int32_t)(MEM_SIZE - 4); // $sp = top of memory
  cpu->running = true;
}

void run_program(CPUState* cpu, Instr* program, size_t count, Memory* mem, Cache* cache) {
  const uint32_t end_pc = (uint32_t)(count * 4);

  while (cpu->running) {
    if (cpu->pc % 4 != 0) {
      fprintf(stderr, "\n[cpu] PC not aligned: %u\n", cpu->pc);
      break;
    }
    if (cpu->pc >= end_pc) {
      fprintf(stderr, "\n[cpu] PC out of program range: %u\n", cpu->pc);
      break;
    }

    size_t i = cpu->pc / 4;
    Instr in = program[i];

    uint32_t next_pc = cpu->pc + 4;

    switch (in.op) {
      case OP_ADD:
        write_reg(cpu, in.rd, (int32_t)((int64_t)cpu->regs[in.rs] + (int64_t)cpu->regs[in.rt]));
        break;
      case OP_SUB:
        write_reg(cpu, in.rd, (int32_t)((int64_t)cpu->regs[in.rs] - (int64_t)cpu->regs[in.rt]));
        break;
      case OP_AND:
        write_reg(cpu, in.rd, cpu->regs[in.rs] & cpu->regs[in.rt]);
        break;
      case OP_OR:
        write_reg(cpu, in.rd, cpu->regs[in.rs] | cpu->regs[in.rt]);
        break;
      case OP_SLT:
        write_reg(cpu, in.rd, (cpu->regs[in.rs] < cpu->regs[in.rt]) ? 1 : 0);
        break;

      case OP_MUL:
        // 32-bit signed multiplication. Result goes into rd. No hi/lo handling.
        write_reg(cpu, in.rd, cpu->regs[in.rs] * cpu->regs[in.rt]);
        break;

      case OP_DIV:
        // 32-bit signed division. Trap on divide by zero.
        if (cpu->regs[in.rt] == 0) {
          fprintf(stderr, "\n[cpu] division by zero at pc=%u: %s\n", cpu->pc, in.raw);
          cpu->running = false;
          break;
        }
        write_reg(cpu, in.rd, cpu->regs[in.rs] / cpu->regs[in.rt]);
        break;

      case OP_ADDI:
        write_reg(cpu, in.rt, cpu->regs[in.rs] + in.imm);
        break;
      case OP_ANDI:
        write_reg(cpu, in.rt, cpu->regs[in.rs] & (int32_t)(uint32_t)in.imm);
        break;
      case OP_ORI:
        write_reg(cpu, in.rt, cpu->regs[in.rs] | (int32_t)(uint32_t)in.imm);
        break;

      case OP_ADDU: {
        uint32_t a = (uint32_t)cpu->regs[in.rs];
        uint32_t b = (uint32_t)cpu->regs[in.rt];
        write_reg(cpu, in.rd, (int32_t)(a + b));
       break;
      }

       case OP_MULT: {
         int64_t prod = (int64_t)cpu->regs[in.rs] * (int64_t)cpu->regs[in.rt];
         cpu->lo = (int32_t)(prod & 0xFFFFFFFF);
         cpu->hi = (int32_t)((prod >> 32) & 0xFFFFFFFF);
          break;
        }    

       case OP_MFLO:
         write_reg(cpu, in.rd, cpu->lo);
         break;

       case OP_LA:
      // la rt, label  (we store label addr in target_pc)
          write_reg(cpu, in.rt, (int32_t)in.target_pc);
          break;
       case OP_LW: {
        uint32_t addr = (uint32_t)(cpu->regs[in.rs] + in.imm);
        if (cache && cache->enabled) cache_access(cache, addr);
        int32_t val;
        if (!mem_load_word(mem, addr, &val)) {
          fprintf(stderr, "\n[cpu] lw failed at addr=0x%08x from: %s\n", addr, in.raw);
          cpu->running = false;
          break;
        }
        write_reg(cpu, in.rt, val);
        break;
      }

      case OP_SW: {
        uint32_t addr = (uint32_t)(cpu->regs[in.rs] + in.imm);
        if (cache && cache->enabled) cache_access(cache, addr);
        if (!mem_store_word(mem, addr, cpu->regs[in.rt])) {
          fprintf(stderr, "\n[cpu] sw failed at addr=0x%08x from: %s\n", addr, in.raw);
          cpu->running = false;
          break;
        }
        break;
      }

      case OP_BEQ: case OP_BNE: case OP_BGE: case OP_BLT: case OP_BGT: case OP_BLE:
        if (branch_taken(in.op, cpu->regs[in.rs], cpu->regs[in.rt])) {
          next_pc = in.target_pc;
        }
        break;

      case OP_J:
        next_pc = in.target_pc;
        break;

      case OP_JAL:
        // $ra = address of next instruction
        write_reg(cpu, 31, (int32_t)next_pc);
        next_pc = in.target_pc;
        break;

      case OP_JR:
        next_pc = (uint32_t)cpu->regs[in.rs];
        break;

      case OP_SYSCALL:
        handle_syscall(cpu, mem);
        break;

      case OP_NOP:
        break;

      default:
        fprintf(stderr, "\n[cpu] invalid instruction at pc=%u: %s\n", cpu->pc, in.raw);
        cpu->running = false;
        break;
    }

    cpu->pc = next_pc;
    cpu->regs[0] = 0; // enforce $zero no matter what
  }
}