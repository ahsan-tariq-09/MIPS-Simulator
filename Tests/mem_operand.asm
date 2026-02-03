  addi $t0, $zero, 100      # base address
  addi $t1, $zero, 42
  sw   $t1, 0($t0)

  addi $t1, $zero, 99
  sw   $t1, 4($t0)

  lw   $t2, 0($t0)          # 42
  lw   $t3, 4($t0)          # 99

  addi $v0, $zero, 1
  add  $a0, $t2, $zero
  syscall

  addi $v0, $zero, 1
  add  $a0, $t3, $zero
  syscall

  addi $v0, $zero, 10
  syscall