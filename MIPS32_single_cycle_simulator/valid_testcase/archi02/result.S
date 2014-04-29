	.ORG 0x00000090 # $sp = 0x00000400, wc_instr = 60, wc_data = 1
	bne  $0 , $29, 5     
	halt
	halt
	halt
	halt
	halt
	addi $8 , $0 , -8    	# 0xFFFFFFF8
	addi $9 , $9 , 1     	# 0x00000001
	slti $1 , $8 , -1    	# 0xFFFFFFFF
	beq  $1 , $0 , 10    
	sb   $8 , 8($8)
	addi $10, $8 , 8     	# 0x00000008
	andi $10, $10, -4    	# 0xFFFFFFFC
	lw   $11, 0($10)	
	lhu  $11, 0($10)
	lh   $11, 0($10)
	lbu  $11, 0($10)
	lb   $11, 0($10)
	add  $8 , $8 , $9 
	j    0x0000002C			# PC => 0x000000B0
	addi $3 , $0 , -32746	# 0xFFFF8016
	sll  $3 , $3 , 7 
	ori  $31, $3 , -294  	# 0xFFFFFEDA
	lui  $31, -32768		# 0xFFFF8000
	xor  $19, $31, $0 
	sub  $0 , $0 , $19
	sub  $13, $0 , $19
	lui  $16, 32767 		# 0x00007FFF
	add  $0 , $16, $16
	add  $16, $16, $16
	lui  $10, -22229		# 0xFFFFA92B
	lui  $11, -14137		# 0xFFFFC8C7
	ori  $10, $10, 21717 	# 0x000054D5
	nori $11, $11, -17582	# 0xFFFFBB52
	and  $12, $11, $10
	or   $12, $11, $10
	nor  $12, $11, $10
	nand $12, $11, $10
	xor  $12, $10, $11
	slt  $3 , $11, $10
	slt  $3 , $10, $11
	slt  $3 , $10, $10
	srl  $3 , $10, 31
	srl  $3 , $11, 31
	sra  $3 , $10, 31
	sra  $3 , $11, 31
	sw   $10, 1020($0)
	lw   $19, 1020($0)	
	sh   $11, 1022($0)
	sb   $12, 1023($0)
	lw   $19, 1020($0)	
	jal  0x0000005A			# PC => 0x00000168
	addi $31, $31, 14    	# 0x0000000E
	addi $8 , $31, -2    	# 0xFFFFFFFE
	jr   $31
	add  $0 , $0 , $8 
	andi $0 , $0 , 256   	# 0x00000100
	beq  $0 , $0 , -57   
	lui  $17, -32768		# 0xFFFF8000
	lw   $0 , 1($17)	
