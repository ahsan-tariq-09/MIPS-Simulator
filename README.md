# MIPS Simulator (C)

This project runs a subset of MIPS assembly directly from `.asm` files (labels supported).

## Build & Run

```bash
make

## Run
./mipsim program.asm
-------------------------------------------------------------------------

##  With direct-mapped cache stats (data accesses only: lw/sw)
./mipsim program.asm --cache 64 16

64 = number of cache lines (power of 2)

16 = block size in bytes (power of 2)

At the end, it prints:

requests
hits
hit rate

## Supported instructions (subset)

R-type: add sub and or slt
I-type: addi andi ori lw sw
Branches: beq bne bge blt bgt ble
Jumps: j jal jr
syscall, nop
b label treated as j label

##Supported syscalls
1: print integer in $a0
5: read integer into $v0
10: exit

## Notes
Memory is 1 MiB, word-aligned lw/sw required.

$zero is enforced to remain 0.

