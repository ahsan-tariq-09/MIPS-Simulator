#include "util.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char* trim(char* s) {
  if (!s) return s;
  while (isspace((unsigned char)*s)) s++;
  if (*s == 0) return s;
  char* end = s + strlen(s) - 1;
  while (end > s && isspace((unsigned char)*end)) end--;
  end[1] = '\0';
  return s;
}

void strip_comment(char* s) {
  if (!s) return;
  for (size_t i = 0; s[i]; i++) {
    if (s[i] == '#') { s[i] = '\0'; return; }
  }
}

bool starts_with(const char* s, const char* prefix) {
  return strncmp(s, prefix, strlen(prefix)) == 0;
}

bool parse_int32(const char* s, int32_t* out) {
  if (!s || !out) return false;
  char* end = NULL;
  long val = strtol(s, &end, 0); // supports decimal and 0x...
  if (end == s || *end != '\0') return false;
  if (val < INT32_MIN || val > INT32_MAX) return false;
  *out = (int32_t)val;
  return true;
}

// Minimal register table. Add more aliases if needed.
int reg_number(const char* tok) {
  if (!tok || tok[0] != '$') return -1;

  // numeric: $0..$31
  if (isdigit((unsigned char)tok[1])) {
    int n = atoi(tok + 1);
    if (n >= 0 && n < 32) return n;
    return -1;
  }

  // common names
  if (!strcmp(tok, "$zero")) return 0;
  if (!strcmp(tok, "$at")) return 1;
  if (!strcmp(tok, "$v0")) return 2;
  if (!strcmp(tok, "$v1")) return 3;
  if (!strcmp(tok, "$a0")) return 4;
  if (!strcmp(tok, "$a1")) return 5;
  if (!strcmp(tok, "$a2")) return 6;
  if (!strcmp(tok, "$a3")) return 7;

  if (!strcmp(tok, "$t0")) return 8;
  if (!strcmp(tok, "$t1")) return 9;
  if (!strcmp(tok, "$t2")) return 10;
  if (!strcmp(tok, "$t3")) return 11;
  if (!strcmp(tok, "$t4")) return 12;
  if (!strcmp(tok, "$t5")) return 13;
  if (!strcmp(tok, "$t6")) return 14;
  if (!strcmp(tok, "$t7")) return 15;

  if (!strcmp(tok, "$s0")) return 16;
  if (!strcmp(tok, "$s1")) return 17;
  if (!strcmp(tok, "$s2")) return 18;
  if (!strcmp(tok, "$s3")) return 19;
  if (!strcmp(tok, "$s4")) return 20;
  if (!strcmp(tok, "$s5")) return 21;
  if (!strcmp(tok, "$s6")) return 22;
  if (!strcmp(tok, "$s7")) return 23;

  if (!strcmp(tok, "$t8")) return 24;
  if (!strcmp(tok, "$t9")) return 25;

  if (!strcmp(tok, "$sp")) return 29;
  if (!strcmp(tok, "$ra")) return 31;

  return -1;
}