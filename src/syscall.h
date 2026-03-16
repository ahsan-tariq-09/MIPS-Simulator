#ifndef SYSCALL_H
#define SYSCALL_H

#include "sim.h"
#include "memory.h"

void handle_syscall(CPUState* cpu, Memory* mem);

#endif
