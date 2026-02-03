#include "parser.h"
#include "cpu.h"
#include "memory.h"
#include "cache.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void usage(const char* prog) {
  fprintf(stderr,
    "Usage: %s <file.asm> [--cache LINES BLOCK]\n"
    "Example: %s prog.asm --cache 64 16\n",
    prog, prog);
}

int main(int argc, char** argv) {
  if (argc < 2) { usage(argv[0]); return 1; }

  const char* asm_path = argv[1];

  bool cache_on = false;
  uint32_t lines = 0, block = 0;

  if (argc == 5 && strcmp(argv[2], "--cache") == 0) {
    cache_on = true;
    lines = (uint32_t)strtoul(argv[3], NULL, 0);
    block = (uint32_t)strtoul(argv[4], NULL, 0);
  } else if (argc != 2) {
    usage(argv[0]);
    return 1;
  }

  Program p = parse_asm_file(asm_path);

  CPUState cpu;
  cpu_init(&cpu);

  Memory mem = mem_create(MEM_SIZE);

  Cache cache = cache_create(lines, block, cache_on);

  run_program(&cpu, p.program, p.count, &mem, &cache);

  cache_print_stats(&cache);

  cache_free(&cache);
  mem_free(&mem);
  free_program(&p);

  return 0;
}