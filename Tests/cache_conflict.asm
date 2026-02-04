  addi $t0, $zero, 0
  addi $t1, $zero, 64

  addi $t2, $zero, 7
  sw   $t2, 0($t0)     # req 1 miss (fills index 0 tag 0)

  sw   $t2, 0($t1)     # req 2 miss (evicts index 0, tag 1)

  lw   $t3, 0($t0)     # req 3 miss again (tag 0 was evicted)

  addi $v0, $zero, 10
  syscall