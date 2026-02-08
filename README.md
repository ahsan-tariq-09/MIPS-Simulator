# MIPS Simulator (C)

This project runs a subset of MIPS assembly directly from `.asm` files with labels and
basic `.data` support.

## Build & Run

```bash
make
```

### Run
```bash
./mipsim Tests/test.asm
```

If your file path has spaces, wrap it in quotes:
```bash
./mipsim "Tests/HW3 by Ahsan Tariq.asm"
```

### Provide input (stdin)
```bash
printf "4\n" | ./mipsim "Tests/HW3 by Ahsan Tariq.asm"
```

### Cache statistics (direct-mapped, data accesses only: lw/sw)
```bash
./mipsim Tests/test.asm --cache 64 16
```

* `64` = number of cache lines (power of 2)
* `16` = block size in bytes (power of 2)

Sample commands:
```bash
./mipsim Tests/test.asm --cache 64 4    # 64 lines × 4 bytes/line
./mipsim Tests/test.asm --cache 32 8    # 32 lines × 8 bytes/line
```

At the end, it prints:
* requests
* hits
* hit rate

## Supported instructions (subset)

R-type: add, addu, sub, and, or, slt, mul, div  
HI/LO: mult, mflo  
I-type: addi, andi, ori, lw, sw  
Load address: la  
Branches: beq, bne, bge, blt, bgt, ble  
Jumps: j, jal, jr (the pseudo‑instruction `b <label>` is treated as `j <label>`)  
Other: syscall, nop

## Supported syscalls
* 1: print integer in $a0
* 5: read integer into $v0
* 10: exit

## Data segment support
* `.data` with `.asciiz` string literals (stored starting at address 0x1000)
* `la` loads the resolved address of a data label into a register

## Notes
* Memory is 1 MiB; word-aligned lw/sw required.
* `mul`/`div` behave like R‑type instructions (rd, rs, rt).
* `mult` writes the 64-bit product to HI/LO; `mflo` reads LO into rd.
* Division by zero terminates the program with an error message.
* `$zero` is enforced to remain 0.
