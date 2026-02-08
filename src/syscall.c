#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>

void handle_syscall(CPUState* cpu, Memory* mem) {
  int32_t code = cpu->regs[2];

  switch (code) {
    case 1:
      printf("%d", cpu->regs[4]);
      fflush(stdout);
      break;

    case 4: { // print string
      uint32_t addr = (uint32_t)cpu->regs[4];
      for (;;) {
        uint8_t ch;
        if (!mem_load_byte(mem, addr, &ch)) {
          fprintf(stderr, "\n[syscall] bad string address 0x%08x\n", addr);
          cpu->running = false;
          return;
        }
        if (ch == 0) break;
        putchar((char)ch);
        addr++;
      }
      fflush(stdout);
      break;
    }

    case 5: {
      int x;
      if (scanf("%d", &x) != 1) {
        fprintf(stderr, "\n[syscall] failed to read int\n");
        cpu->running = false;
        return;
      }
      cpu->regs[2] = x;
      break;
    }

    case 10:
      cpu->running = false;
      break;

    default:
      fprintf(stderr, "\n[syscall] unsupported syscall %d\n", code);
      cpu->running = false;
      break;
  }
}