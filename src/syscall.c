#include "syscall.h"
#include <stdio.h>
#include <stdlib.h>

void handle_syscall(CPUState* cpu) {
  // Convention: $v0 is regs[2], $a0 is regs[4]
  int32_t code = cpu->regs[2];

  switch (code) {
    case 1: // print int in $a0
      printf("%d", cpu->regs[4]);
      fflush(stdout);
      break;

    case 5: { // read int into $v0
      int x;
      if (scanf("%d", &x) != 1) {
        fprintf(stderr, "\n[syscall] failed to read int\n");
        cpu->running = false;
        return;
      }
      cpu->regs[2] = x;
      break;
    }

    case 10: // exit
      cpu->running = false;
      break;

    default:
      fprintf(stderr, "\n[syscall] unsupported syscall code=%d\n", code);
      cpu->running = false;
      break;
  }
}