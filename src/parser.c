#include "parser.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINES  65536
#define MAX_LABELS 4096

static uint32_t label_lookup(Label* labels, size_t n, const char* name, int* ok) {
  for (size_t i = 0; i < n; i++) {
    if (strcmp(labels[i].name, name) == 0) {
      *ok = 1;
      return labels[i].pc;
    }
  }
  *ok = 0;
  return 0;
}

static void label_add(Label* labels, size_t* n, const char* name, uint32_t pc) {
  if (*n >= MAX_LABELS) {
    fprintf(stderr, "[parser] too many labels\n");
    exit(1);
  }
  for (size_t i = 0; i < *n; i++) {
    if (strcmp(labels[i].name, name) == 0) {
      fprintf(stderr, "[parser] duplicate label: %s\n", name);
      exit(1);
    }
  }
  strncpy(labels[*n].name, name, sizeof(labels[*n].name) - 1);
  labels[*n].pc = pc;
  (*n)++;
}

static Op op_from_mnemonic(const char* m) {
  if (!strcmp(m, "add")) return OP_ADD;
  if (!strcmp(m, "sub")) return OP_SUB;
  if (!strcmp(m, "and")) return OP_AND;
  if (!strcmp(m, "or"))  return OP_OR;
  if (!strcmp(m, "slt")) return OP_SLT;

  if (!strcmp(m, "addi")) return OP_ADDI;
  if (!strcmp(m, "andi")) return OP_ANDI;
  if (!strcmp(m, "ori"))  return OP_ORI;

  if (!strcmp(m, "lw")) return OP_LW;
  if (!strcmp(m, "sw")) return OP_SW;

  if (!strcmp(m, "beq")) return OP_BEQ;
  if (!strcmp(m, "bne")) return OP_BNE;

  // Pseudo-ish branches (common in teaching code)
  if (!strcmp(m, "bge")) return OP_BGE;
  if (!strcmp(m, "blt")) return OP_BLT;
  if (!strcmp(m, "bgt")) return OP_BGT;
  if (!strcmp(m, "ble")) return OP_BLE;

  if (!strcmp(m, "j"))   return OP_J;
  if (!strcmp(m, "jal")) return OP_JAL;
  if (!strcmp(m, "jr"))  return OP_JR;

  if (!strcmp(m, "b"))   return OP_J; // treat unconditional branch as jump

  if (!strcmp(m, "syscall")) return OP_SYSCALL;
  if (!strcmp(m, "nop")) return OP_NOP;

  return OP_INVALID;
}

// Split by commas and whitespace, but keep things like "4($t0)" intact.
static int tokenize(char* s, char* out[], int max) {
  int n = 0;
  for (char* p = s; *p && n < max;) {
    while (*p && (*p == ' ' || *p == '\t' || *p == ',')) p++;
    if (!*p) break;
    out[n++] = p;
    while (*p && *p != ' ' && *p != '\t' && *p != ',') p++;
    if (*p) { *p = '\0'; p++; }
  }
  return n;
}

static void parse_mem_operand(const char* tok, int32_t* offset, int* base_reg) {
  // format: imm($reg) e.g. 4($t0) or 0($sp)
  char buf[128];
  strncpy(buf, tok, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';

  char* lpar = strchr(buf, '(');
  char* rpar = strchr(buf, ')');
  if (!lpar || !rpar || rpar < lpar) {
    fprintf(stderr, "[parser] bad memory operand: %s\n", tok);
    exit(1);
  }
  *lpar = '\0';
  *rpar = '\0';

  char* off_s = trim(buf);
  char* reg_s = trim(lpar + 1);

  int32_t off = 0;
  if (strlen(off_s) == 0) off = 0;
  else if (!parse_int32(off_s, &off)) {
    fprintf(stderr, "[parser] bad offset in: %s\n", tok);
    exit(1);
  }

  int r = reg_number(reg_s);
  if (r < 0) {
    fprintf(stderr, "[parser] bad base reg in: %s\n", tok);
    exit(1);
  }

  *offset = off;
  *base_reg = r;
}

Program parse_asm_file(const char* path) {
  FILE* f = fopen(path, "r");
  if (!f) { perror("[parser] fopen"); exit(1); }

  char* lines[MAX_LINES];
  size_t line_count = 0;

  // Read all lines first
  char buf[512];
  while (fgets(buf, sizeof(buf), f)) {
    if (line_count >= MAX_LINES) {
      fprintf(stderr, "[parser] too many lines\n");
      exit(1);
    }
    lines[line_count] = strdup(buf);
    line_count++;
  }
  fclose(f);

  Label labels[MAX_LABELS];
  size_t nlabels = 0;

  // PASS 1: collect labels, count instructions
  uint32_t pc = 0;
  size_t inst_count = 0;

  for (size_t i = 0; i < line_count; i++) {
    strip_comment(lines[i]);
    char* s = trim(lines[i]);
    if (*s == '\0') continue;

    // ignore directives like .text .data for now
    if (s[0] == '.') continue;

    // handle label: "loop:"
    char* colon = strchr(s, ':');
    if (colon) {
      *colon = '\0';
      char* lname = trim(s);
      if (*lname) label_add(labels, &nlabels, lname, pc);
      s = trim(colon + 1);
      if (*s == '\0') continue; // label-only line
    }

    // At this point, we have an instruction line
    inst_count++;
    pc += 4;
  }

  Instr* program = (Instr*)calloc(inst_count, sizeof(Instr));
  if (!program) { fprintf(stderr, "[parser] OOM\n"); exit(1); }

  // PASS 2: parse instructions and resolve label targets
  pc = 0;
  size_t idx = 0;

  for (size_t i = 0; i < line_count; i++) {
    char original[512];
    strncpy(original, lines[i], sizeof(original) - 1);
    original[sizeof(original) - 1] = '\0';

    strip_comment(lines[i]);
    char* s = trim(lines[i]);
    if (*s == '\0') continue;
    if (s[0] == '.') continue;

    char* colon = strchr(s, ':');
    if (colon) {
      s = trim(colon + 1);
      if (*s == '\0') continue;
    }

    if (idx >= inst_count) { fprintf(stderr, "[parser] internal count mismatch\n"); exit(1); }

    Instr in;
    memset(&in, 0, sizeof(in));
    in.rd = in.rs = in.rt = -1;
    in.imm = 0;
    in.target_pc = 0;
    strncpy(in.raw, trim(original), sizeof(in.raw) - 1);

    char* toks[8];
    char tmp[512];
    strncpy(tmp, s, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';

    int nt = tokenize(tmp, toks, 8);
    if (nt <= 0) continue;

    in.op = op_from_mnemonic(toks[0]);
    if (in.op == OP_INVALID) {
      fprintf(stderr, "[parser] unknown mnemonic: %s\n", toks[0]);
      exit(1);
    }

    // Decode operands by opcode family
    switch (in.op) {
      case OP_ADD: case OP_SUB: case OP_AND: case OP_OR: case OP_SLT:
        // add rd, rs, rt
        if (nt != 4) { fprintf(stderr, "[parser] bad R-type: %s\n", in.raw); exit(1); }
        in.rd = reg_number(toks[1]);
        in.rs = reg_number(toks[2]);
        in.rt = reg_number(toks[3]);
        break;

      case OP_ADDI: case OP_ANDI: case OP_ORI:
        // addi rt, rs, imm
        if (nt != 4) { fprintf(stderr, "[parser] bad I-imm: %s\n", in.raw); exit(1); }
        in.rt = reg_number(toks[1]);
        in.rs = reg_number(toks[2]);
        if (!parse_int32(toks[3], &in.imm)) {
          fprintf(stderr, "[parser] bad immediate: %s\n", in.raw);
          exit(1);
        }
        break;

      case OP_LW: case OP_SW:
        // lw rt, off(base)
        if (nt != 3) { fprintf(stderr, "[parser] bad lw/sw: %s\n", in.raw); exit(1); }
        in.rt = reg_number(toks[1]);
        parse_mem_operand(toks[2], &in.imm, &in.rs); // rs = base
        break;

      case OP_BEQ: case OP_BNE: case OP_BGE: case OP_BLT: case OP_BGT: case OP_BLE: {
        // beq rs, rt, label
        if (nt != 4) { fprintf(stderr, "[parser] bad branch: %s\n", in.raw); exit(1); }
        in.rs = reg_number(toks[1]);
        in.rt = reg_number(toks[2]);
        int ok = 0;
        uint32_t tpc = label_lookup(labels, nlabels, toks[3], &ok);
        if (!ok) { fprintf(stderr, "[parser] unknown label: %s\n", toks[3]); exit(1); }
        in.target_pc = tpc;
        break;
      }

      case OP_J: case OP_JAL: {
        // j label
        if (nt != 2) { fprintf(stderr, "[parser] bad jump: %s\n", in.raw); exit(1); }
        int ok = 0;
        uint32_t tpc = label_lookup(labels, nlabels, toks[1], &ok);
        if (!ok) { fprintf(stderr, "[parser] unknown label: %s\n", toks[1]); exit(1); }
        in.target_pc = tpc;
        break;
      }

      case OP_JR:
        // jr rs
        if (nt != 2) { fprintf(stderr, "[parser] bad jr: %s\n", in.raw); exit(1); }
        in.rs = reg_number(toks[1]);
        break;

      case OP_SYSCALL:
      case OP_NOP:
        // no operands
        break;

      default:
        break;
    }

    // Validate register parse
    if ((in.rd < -1 || in.rd >= 32) ||
        (in.rs < -1 || in.rs >= 32) ||
        (in.rt < -1 || in.rt >= 32)) {
      fprintf(stderr, "[parser] bad register in: %s\n", in.raw);
      exit(1);
    }

    program[idx++] = in;
    pc += 4;
  }

  for (size_t i = 0; i < line_count; i++) free(lines[i]);

  Program p;
  p.program = program;
  p.count = inst_count;
  return p;
}

void free_program(Program* p) {
  if (!p) return;
  free(p->program);
  p->program = NULL;
  p->count = 0;
}