	.ORG 0x0000004C # $sp = 0x00000100, wc_instr = 23, wc_data = 5
	lw   $4 , 0($0)	
	addi $29, $29, -12   	# 0xFFFFFFF4
	sw   $31, 0($29)
	sw   $17, 4($29)
	sw   $4 , 8($29)
	slti $8 , $4 , 1     	# 0x00000001
	beq  $8 , $0 , 2     
	sll  $2 , $4 , 1 
	j    0x00000022			# PC => 0x00000088
	addi $4 , $4 , -1    	# 0xFFFFFFFF
	bne  $0 , $0 , 12    
	add  $1 , $0 , $4 
	addi $4 , $4 , -1    	# 0xFFFFFFFF
	bne  $0 , $0 , 12    
	add  $2 , $2 , $17
	lw   $31, 0($29)	
	lw   $4 , 8($29)	
	lw   $17, 4($29)	
	addi $29, $29, 12    	# 0x0000000C
	sw   $2 , 4($0)
	lw   $19, 4($0)	
	halt
	halt
