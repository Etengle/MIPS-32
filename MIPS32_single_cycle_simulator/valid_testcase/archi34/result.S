	.ORG 0x00000000 # $sp = 0x00000400, wc_instr = 31, wc_data = 1
	lb   $10, 0($0)
	add  $17, $0 , $0 
	addi $18, $0 , 1     	# 0x00000001
	slti $11, $17, -4    	# 0xFFFFFFFC
	beq  $11, $18, 3     
	addi $17, $17, -1    	# 0xFFFFFFFF
	addi $10, $10, 1     	# 0x00000001
	j    0x00000003			# PC => 0x0000000C
	sw   $10, 4($0)
	lh   $12, 4($0)
	lw   $10, 0($0)	
	sll  $13, $10, 8 
	addi $13, $13, -1    	# 0xFFFFFFFF
	or   $14, $12, $13
	sw   $14, 8($0)
	lb   $14, 8($0)
	lbu  $15, 8($0)
	bne  $14, $15, 10    
	ori  $14, $14, 1     	# 0x00000001
	srl  $22, $14, 1 
	addi $3 , $22, 1     	# 0x00000001
	addi $5 , $0 , -4    	# 0xFFFFFFFC
	sw   $3 , 0($5)
	halt
	halt
	halt
	halt
	halt
	sub  $0 , $10, $14
	addi $19, $0 , 72    	# 0x00000048
	jr   $19
