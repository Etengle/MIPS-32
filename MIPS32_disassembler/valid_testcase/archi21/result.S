	.ORG 0x000000B4 # $sp = 0x00000300, wc_instr = 42, wc_data = 5
	lw   $5 , 0($0)	
	lw   $6 , 4($0)	
	addi $6 , $6 , -1    	# 0xFFFFFFFF
	add  $5 , $5 , $5 
	bne  $0 , $6 , -3    
	sw   $5 , 32($0)
	lw   $2 , 32($0)	
	lw   $7 , 8($0)	
	lbu  $6 , 12($0)
	add  $3 , $7 , $0 
	addi $29, $29, -4    	# 0xFFFFFFFC
	sw   $31, 0($29)
	jal  0x00000049			# PC => 0x00000124
	add  $28, $5 , $0 
	lw   $31, 0($29)	
	addi $29, $29, 4     	# 0x00000004
	add  $15, $2 , $3 
	and  $0 , $2 , $3 
	sra  $0 , $3 , 14
	lb   $20, 16($0)
	lbu  $21, 16($0)
	sub  $22, $21, $20
	lw   $9 , -4($0)	
	halt
	halt
	halt
	halt
	halt
	slt  $4 , $6 , $0 
	beq  $4 , $0 , 2     
	or   $27, $3 , $0 
	jr   $31
	addi $29, $29, -8    	# 0xFFFFFFF8
	sw   $31, 4($29)
	sw   $6 , 0($29)
	addi $6 , $6 , -1    	# 0xFFFFFFFF
	jal  0x00000049			# PC => 0x00000124
	lw   $6 , 0($29)	
	lw   $31, 4($29)	
	addi $29, $29, 8     	# 0x00000008
	sll  $3 , $3 , 2 
	jr   $31
