# MIPS Simulator (C)

This project runs a subset of MIPS assembly directly from `.asm` files (labels supported).

## Build & Run

```bash
make

## Run (run make command first)
./mipsim Tests/test.asm

if you file has spaces put it in ""
-------------------------------------------------------------------------

##  With direct-mapped cache stats (data accesses only: lw/sw)
./mipsim Tests/test.asm --cache 64 16

64 = number of cache lines (power of 2)

16 = block size in bytes (power of 2)

Sample commands:
/mipsim Tests/test.asm --cache 64 4    # 64 lines × 4 bytes/line
./mipsim Tests/test.asm --cache 32 8    # 32 lines × 8 bytes/line

At the end, it prints:

requests
hits
hit rate

## Supported instructions (subset)

R-type: add sub and or slt mul  div
I-type: addi andi ori lw sw
Branches: beq bne bge blt bgt ble
Jumps: j jal jr  (the pseudo‑instruction `b <label>` is treated as `j <label>`)
syscall, nop

##Supported syscalls
1: print integer in $a0
5: read integer into $v0
10: exit

## Notes
Memory is 1 MiB, word-aligned lw/sw required.
Multiplication (mul rd, rs, rt) and division (div rd, rs, rt) are supported. Both
behave like R‑type instructions: they take three register operands and place the result in rd. Division by zero will terminate the program with an error message

$zero is enforced to remain 0.

