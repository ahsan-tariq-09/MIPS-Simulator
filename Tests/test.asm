	add	$t0, $zero, $zero	# x = 0
	addi	$t1, $zero, 10		# t1 = 10

loop:	bge	$t0, $t1, exit		# while (x < 10)

	# print x
	addi	$v0, $zero, 1
	add	$a0, $t0, $zero
	syscall

	addi	$t0, $t0, 1
	b	loop

exit:
	addi	$v0, $zero, 10
	syscall