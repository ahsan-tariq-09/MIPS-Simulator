  addi $t0, $zero, 10
  addi $t1, $zero, -3
  add  $t2, $t0, $t1     # 7

  addi $v0, $zero, 1
  add  $a0, $t2, $zero
  syscall

  addi $v0, $zero, 10
  syscall