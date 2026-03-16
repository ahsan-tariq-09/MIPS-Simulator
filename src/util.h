#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdint.h>

char* trim(char* s);
void strip_comment(char* s);
bool starts_with(const char* s, const char* prefix);

bool parse_int32(const char* s, int32_t* out);
int reg_number(const char* tok); // "$t0" -> 8 etc.

#endif