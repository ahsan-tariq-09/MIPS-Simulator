	# Collaboration statement:
# I didn't work with anyone.

.data
newline: .asciiz "\n"

.text
.globl main

main:
    # read integer n
    addi $v0, $zero, 5
    syscall
    addi $a0, $v0, 0        # arg n -> $a0

    jal  catalan            # v0 = catalan(n)

    # print result
    addi $a0, $v0, 0
    addi $v0, $zero, 1
    syscall

    # print newline
    la   $a0, newline
    addi $v0, $zero, 4
    syscall

    # exit
    addi $v0, $zero, 10
    syscall


# int catalan(int n)
# argument: $a0 = n
# return:   $v0 = catalan(n)
catalan:

    addi $sp, $sp, -20
    sw   $s3, 0($sp)
    sw   $s2, 4($sp)
    sw   $s1, 8($sp)
    sw   $s0, 12($sp)
    sw   $ra, 16($sp)

    addi $s0, $a0, 0        

    
    beq  $s0, $zero, catalan_base

    
    addi $s2, $zero, 0      
    addi $s1, $zero, 0      

catalan_loop:
    # while (i < n)
    slt  $t0, $s1, $s0      
    beq  $t0, $zero, catalan_done

    # x = catalan(i)
    addi $a0, $s1, 0
    jal  catalan
    addi $s3, $v0, 0        

    # y = catalan(n - 1 - i)
    addi $t1, $s0, -1       
    sub  $t1, $t1, $s1      
    addi $a0, $t1, 0
    jal  catalan
    addi $t2, $v0, 0        

    # prod = x * y
    mult $s3, $t2
    mflo $t3                

    # sum += prod
    addu $s2, $s2, $t3

    # i++
    addi $s1, $s1, 1
    j    catalan_loop

catalan_done:
    addi $v0, $s2, 0        
    j    catalan_epilogue

catalan_base:
    addi $v0, $zero, 1      # return 1

catalan_epilogue:
    lw   $ra, 16($sp)
    lw   $s0, 12($sp)
    lw   $s1, 8($sp)
    lw   $s2, 4($sp)
    lw   $s3, 0($sp)
    addi $sp, $sp, 20
    jr   $ra
