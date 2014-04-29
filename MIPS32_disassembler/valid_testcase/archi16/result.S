	.ORG 0x00000200 # $sp = 0x00000400, wc_instr = 104, wc_data = 9
	jal  0x00000081			# PC => 0x00000204
	j    0x00000082			# PC => 0x00000208
	lhu  $2 , 0($0)
	bne  $2 , $0 , 11    
	ori  $5 , $0 , 136   	# 0x00000088
	lbu  $3 , 2($0)
	slt  $6 , $5 , $3 
	bne  $6 , $0 , 5     
	lb   $4 , 3($0)
	slti $6 , $4 , -128  	# 0xFFFFFF80
	bne  $6 , $0 , 2     
	addi $7 , $0 , 1     	# 0x00000001
	beq  $0 , $0 , 17    
	addi $7 , $0 , -1    	# 0xFFFFFFFF
	beq  $0 , $0 , 15    
	ori  $5 , $0 , 136   	# 0x00000088
	lbu  $3 , 2($0)
	slt  $6 , $5 , $3 
	bne  $6 , $0 , 3     
	lb   $4 , 3($0)
	slti $6 , $4 , -128  	# 0xFFFFFF80
	beq  $6 , $0 , 2     
	addi $7 , $0 , 2     	# 0x00000002
	beq  $0 , $0 , 6     
	bne  $3 , $5 , 4     
	addi $5 , $0 , -128  	# 0xFFFFFF80
	bne  $4 , $5 , 2     
	andi $7 , $0 , 0     	# 0x00000000
	beq  $0 , $0 , 1     
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
	slt  $0 , $0 , $0 
	sll  $0 , $0 , 0 
	srl  $0 , $0 , 0 
	sra  $0 , $0 , 0 
	addi $0 , $0 , 0     	# 0x00000000
	lui  $0 , 0     		# 0x00000000
	andi $0 , $0 , 0     	# 0x00000000
	ori  $0 , $0 , 0     	# 0x00000000
	nori $0 , $0 , 0     	# 0x00000000
	slti $0 , $0 , 0     	# 0x00000000
	lw   $0 , 0($0)	
	lh   $0 , 0($0)
	lhu  $0 , 0($0)
	lb   $0 , 0($0)
	lbu  $0 , 0($0)
	lw   $1 , 12($0)	
	lw   $2 , 16($0)	
	lw   $3 , 20($0)	
	lw   $4 , 24($0)	
	lw   $5 , 28($0)	
	lw   $6 , 32($0)	
	slt  $7 , $3 , $2 
	sll  $7 , $4 , 5 
	sll  $7 , $4 , 10
	sll  $7 , $4 , 31
	sll  $7 , $4 , 20
	srl  $7 , $1 , 1 
	srl  $7 , $1 , 5 
	srl  $7 , $1 , 10
	srl  $7 , $1 , 31
	sra  $7 , $1 , 1 
	sra  $7 , $1 , 5 
	sra  $7 , $1 , 10
	sra  $7 , $1 , 31
	srl  $7 , $1 , 0 
	slti $7 , $4 , -1    	# 0xFFFFFFFF
	slti $7 , $4 , 0     	# 0x00000000
	slti $7 , $4 , 11    	# 0x0000000B
	lui  $7 , -1    		# 0xFFFFFFFF
	lui  $7 , 1     		# 0x00000001
	sw   $2 , 40($0)
	lw   $7 , 40($0)	
	lh   $7 , 40($0)
	lhu  $7 , 40($0)
	lb   $7 , 40($0)
	lbu  $7 , 40($0)
	add  $7 , $1 , $1 
	addi $7 , $2 , 1     	# 0x00000001
	add  $7 , $1 , $3 
	sub  $7 , $0 , $2 
	sub  $7 , $0 , $3 
	add  $7 , $3 , $3 
	andi $7 , $1 , -1    	# 0xFFFFFFFF
	ori  $7 , $0 , -1    	# 0xFFFFFFFF
	nori $7 , $0 , -1    	# 0xFFFFFFFF
	lw   $7 , 12($0)	
	lh   $7 , 12($0)
	lhu  $7 , 12($0)
	lw   $7 , 2($2)	
	add  $4 , $1 , $1 
	halt
	halt
	halt
	halt
	halt
