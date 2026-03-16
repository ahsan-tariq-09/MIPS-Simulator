#ifndef CPU_H
#define CPU_H

#include <stddef.h>
#include "sim.h"
#include "memory.h"
#include "cache.h"

void cpu_init(CPUState* cpu);
void run_program(CPUState* cpu, Instr* program, size_t count, Memory* mem, Cache* cache);

#endif