	.ORG 0x00000270 # $sp = 0x00000300, wc_instr = 41, wc_data = 2
	addi $29, $29, -4    	# 0xFFFFFFFC
	sw   $31, 0($29)
	lw   $4 , 0($0)	
	and  $16, $16, $0 
	jal  0x000000A8			# PC => 0x000002A0
	lw   $31, 0($29)	
	addi $29, $29, 4     	# 0x00000004
	halt
	halt
	halt
	halt
	halt
	and  $8 , $8 , $0 
	addi $8 , $8 , 1     	# 0x00000001
	bne  $8 , $4 , 2     
	addi $16, $16, 1     	# 0x00000001
	jr   $31
	andi $8 , $4 , 1     	# 0x00000001
	bne  $8 , $0 , 10    
	addi $29, $29, -8    	# 0xFFFFFFF8
	addi $16, $16, 1     	# 0x00000001
	sw   $31, 4($29)
	sw   $0 , 0($29)
	srl  $4 , $4 , 1 
	jal  0x000000A8			# PC => 0x000002A0
	lw   $4 , 0($29)	
	lw   $31, 4($29)	
	addi $29, $29, 8     	# 0x00000008
	jr   $31
	addi $29, $29, -8    	# 0xFFFFFFF8
	addi $16, $16, 1     	# 0x00000001
	sw   $31, 4($29)
	sw   $0 , 0($29)
	sll  $8 , $4 , 1 
	add  $4 , $4 , $8 
	addi $4 , $4 , 1     	# 0x00000001
	jal  0x000000A8			# PC => 0x000002A0
	lw   $4 , 0($29)	
	lw   $31, 4($29)	
	addi $29, $29, 8     	# 0x00000008
	jr   $31
