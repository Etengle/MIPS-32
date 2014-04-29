	.ORG 0x00000360 # $sp = 0x00000400, wc_instr = 40, wc_data = 5
	lw   $17, 0($0)	
	addi $17, $17, 1     	# 0x00000001
	sb   $17, 0($0)
	beq  $17, $0 , -220  
	lw   $1 , 16($0)	
	addi $8 , $8 , 1     	# 0x00000001
	add  $9 , $0 , $0 
	addi $0 , $1 , 1     	# 0x00000001
	addi $9 , $9 , 1     	# 0x00000001
	slti $11, $9 , 3     	# 0x00000003
	bne  $11, $0 , -4    
	slti $11, $8 , 3     	# 0x00000003
	bne  $11, $0 , -8    
	jal  0x000000EF			# PC => 0x000003BC
	andi $10, $11, -1    	# 0xFFFFFFFF
	ori  $10, $10, -1    	# 0xFFFFFFFF
	nori $10, $10, -1    	# 0xFFFFFFFF
	j    0x000000F5			# PC => 0x000003D4
	halt
	halt
	halt
	halt
	halt
	lw   $11, 4($0)	
	sll  $11, $11, 16
	sra  $11, $11, 16
	srl  $11, $11, 16
	lui  $11, -1    		# 0xFFFFFFFF
	jr   $31
	addi $8 , $0 , 1     	# 0x00000001
	addi $16, $0 , 7     	# 0x00000007
	lw   $14, 1($16)	
	lh   $13, 7($16)
	add  $15, $14, $13
	sub  $15, $14, $8 
	addi $24, $0 , 1022  	# 0x000003FE
	sb   $24, 1($24)
	lbu  $24, 1($24)
	lw   $1 , 16($0)	
	lw   $0 , 1($1)	
