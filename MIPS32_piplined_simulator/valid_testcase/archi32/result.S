	.ORG 0x00000004 # $sp = 0x00000200, wc_instr = 148, wc_data = 9
	addi $2 , $0 , 0     	# 0x00000000
	lhu  $2 , 0($0)
	bne  $2 , $0 , 1     
	beq  $2 , $0 , 2     
	sub  $2 , $2 , $2 
	beq  $2 , $0 , -3    
	andi $7 , $0 , 0     	# 0x00000000
	beq  $0 , $0 , 0     
	addi $7 , $0 , -2    	# 0xFFFFFFFE
	sh   $7 , 4($0)
	lh   $8 , 4($0)
	add  $0 , $0 , $0 
	sub  $0 , $0 , $0 
	and  $0 , $0 , $0 
	or   $0 , $0 , $0 
	xor  $0 , $0 , $0 
	nor  $0 , $0 , $0 
	nand $0 , $0 , $0 
	sll  $0 , $0 , 0 
	lw   $0 , 0($0)	
	beq  $0 , $0 , 0     
	lh   $0 , 0($0)
	lhu  $0 , 0($0)
	lb   $0 , 0($0)
	lbu  $0 , 0($0)
	sll  $0 , $0 , 0 
	srl  $0 , $0 , 0 
	sra  $0 , $0 , 0 
	addi $0 , $0 , -32768	# 0xFFFF8000
	lui  $0 , -32768		# 0xFFFF8000
	slt  $0 , $0 , $0 
	andi $0 , $0 , -32768	# 0xFFFF8000
	ori  $0 , $0 , -32768	# 0xFFFF8000
	nori $0 , $0 , -32768	# 0xFFFF8000
	slti $0 , $0 , 32767 	# 0x00007FFF
	slti $0 , $0 , -32768	# 0xFFFF8000
	sll  $0 , $0 , 0 
	lw   $5 , 28($0)	
	lw   $6 , 32($0)	
	addi $2 , $0 , 16    	# 0x00000010
	lw   $2 , 0($2)	
	addi $3 , $0 , 20    	# 0x00000014
	lw   $3 , 0($3)	
	slt  $7 , $3 , $2 
	lw   $4 , 24($0)	
	sll  $7 , $4 , 5 
	sll  $7 , $4 , 10
	sll  $7 , $4 , 31
	sll  $7 , $4 , 20
	lw   $1 , 12($0)	
	srl  $7 , $1 , 1 
	srl  $1 , $7 , 5 
	srl  $7 , $1 , 10
	srl  $1 , $7 , 31
	lw   $1 , 12($0)	
	sra  $7 , $1 , 1 
	lw   $1 , 12($0)	
	sra  $1 , $7 , 5 
	lw   $1 , 12($0)	
	sra  $7 , $1 , 10
	lw   $1 , 12($0)	
	sra  $7 , $1 , 31
	lw   $1 , 12($0)	
	srl  $7 , $1 , 0 
	slti $7 , $4 , -1    	# 0xFFFFFFFF
	slti $4 , $7 , 0     	# 0x00000000
	slti $7 , $4 , 11    	# 0x0000000B
	lui  $7 , -1    		# 0xFFFFFFFF
	lui  $7 , 1     		# 0x00000001
	addi $7 , $2 , 0     	# 0x00000000
	sw   $7 , 40($0)
	beq  $7 , $2 , 0     
	j    0x0000004A			# PC => 0x00000128
	jal  0x0000004B			# PC => 0x0000012C
	sw   $7 , 40($0)
	lw   $7 , 40($0)	
	sw   $7 , 40($0)
	lh   $7 , 40($0)
	lhu  $7 , 40($0)
	lb   $7 , 40($0)
	lbu  $7 , 40($0)
	add  $7 , $1 , $1 
	addi $0 , $2 , 1     	# 0x00000001
	add  $0 , $1 , $3 
	sub  $7 , $0 , $2 
	sub  $0 , $0 , $3 
	sub  $0 , $1 , $3 
	add  $3 , $3 , $3 
	andi $7 , $1 , -1    	# 0xFFFFFFFF
	ori  $7 , $0 , -1    	# 0xFFFFFFFF
	nori $7 , $0 , -1    	# 0xFFFFFFFF
	lw   $7 , 12($0)	
	lh   $7 , 12($0)
	lhu  $7 , 12($0)
	jal  0x00000070			# PC => 0x000001C0
	lw   $3 , 20($0)	
	add  $0 , $2 , $2 
	lw   $0 , 32767($2)	
	sub  $0 , $3 , $3 
	add  $0 , $2 , $2 
	beq  $1 , $1 , 0     
	add  $4 , $1 , $1 
	lw   $14, -4($29)	
	add  $0 , $14, $0 
	sub  $31, $0 , $0 
	beq  $14, $29, 24    
	halt
	halt
	halt
	halt
	halt
	addi $29, $29, -4    	# 0xFFFFFFFC
	sw   $31, 0($29)
	addi $29, $29, -4    	# 0xFFFFFFFC
	sw   $7 , 0($29)
	addi $29, $29, -1    	# 0xFFFFFFFF
	addi $29, $29, -3    	# 0xFFFFFFFD
	jal  0x0000007F			# PC => 0x000001FC
	addi $29, $29, 3     	# 0x00000003
	addi $29, $29, 2     	# 0x00000002
	addi $29, $29, -1    	# 0xFFFFFFFF
	lw   $7 , 0($29)	
	addi $29, $29, 8     	# 0x00000008
	addi $29, $29, -4    	# 0xFFFFFFFC
	lw   $24, 0($29)	
	jr   $24
	addi $29, $29, -4    	# 0xFFFFFFFC
	sw   $29, -4($29)
	sw   $31, 0($29)
	j    0x00000067			# PC => 0x0000019C
	lw   $31, 0($29)	
	addi $29, $29, 4     	# 0x00000004
	addi $25, $25, 110   	# 0x0000006E
	add  $16, $25, $25
	sub  $25, $16, $25
	ori  $16, $25, 10    	# 0x0000000A
	and  $25, $25, $25
	jal  0x0000008D			# PC => 0x00000234
	lw   $31, -4($29)	
	jr   $31
	addi $31, $31, 3     	# 0x00000003
	addi $31, $31, -3    	# 0xFFFFFFFD
	jr   $31
	halt
	halt
	halt
	halt
	halt
