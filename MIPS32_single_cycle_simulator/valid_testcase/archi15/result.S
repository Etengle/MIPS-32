	.ORG 0x0000000C # $sp = 0x00000400, wc_instr = 28, wc_data = 3
	addi $29, $29, -4    	# 0xFFFFFFFC
	sw   $31, 0($29)
	lui  $9 , 32767 		# 0x00007FFF
	ori  $9 , $9 , -1    	# 0xFFFFFFFF
	addi $0 , $9 , 1     	# 0x00000001
	addi $8 , $9 , 1     	# 0x00000001
	addi $8 , $8 , -1    	# 0xFFFFFFFF
	lb   $10, 0($0)
	lbu  $11, 0($0)
	add  $11, $11, $10
	lh   $12, 4($0)
	lhu  $13, 4($0)
	add  $13, $13, $12
	lw   $14, 8($0)	
	slt  $0 , $14, $12
	srl  $15, $14, 1 
	sub  $14, $14, $11
	sub  $13, $12, $11
	add  $15, $9 , $11
	bne  $14, $13, 6     
	lh   $0 , 2($9)
	halt
	halt
	halt
	halt
	halt
	add  $13, $0 , $14
	j    0x00000016			# PC => 0x00000058
