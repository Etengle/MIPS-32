	.ORG 0x00000000 # $sp = 0x00000400, wc_instr = 33, wc_data = 3
	addi $29, $29, -4    	# 0xFFFFFFFC
	sw   $31, 0($29)
	lw   $4 , 0($0)	
	lw   $5 , 4($0)	
	lw   $6 , 8($0)	
	jal  0x0000000F			# PC => 0x0000003C
	sw   $2 , 12($0)
	lw   $31, 0($29)	
	addi $29, $29, 4     	# 0x00000004
	lw   $3 , 12($0)	
	halt
	halt
	halt
	halt
	halt
	slt  $8 , $4 , $6 
	beq  $8 , $0 , 4     
	slt  $10, $6 , $4 
	beq  $10, $0 , 8     
	add  $2 , $5 , $0 
	jr   $31
	slt  $9 , $6 , $4 
	bne  $9 , $0 , 2     
	add  $3 , $6 , $0 
	jr   $31
	add  $3 , $4 , $0 
	jr   $31
	slt  $11, $5 , $6 
	bne  $11, $0 , 2     
	add  $2 , $5 , $0 
	jr   $31
	add  $2 , $6 , $0 
	jr   $31
