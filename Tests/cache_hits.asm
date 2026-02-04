  addi $t0, $zero, 0        # base address 0 (aligned)
  addi $t1, $zero, 123
  sw   $t1, 0($t0)          # cache request 1 (miss)

  lw   $t2, 0($t0)          # request 2 (hit)
  lw   $t2, 0($t0)          # request 3 (hit)
  lw   $t2, 0($t0)          # request 4 (hit)

  addi $v0, $zero, 10
  syscall