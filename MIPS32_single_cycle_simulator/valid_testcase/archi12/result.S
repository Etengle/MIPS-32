	.ORG 0x00000000 # $sp = 0x00000400, wc_instr = 12, wc_data = 3
	addi $29, $29, -4    	# 0xFFFFFFFC
	sw   $31, 0($29)
	lw   $4 , 8($0)	
	lw   $5 , 4($0)	
	jal  0x0000000A			# PC => 0x00000028
	sw   $2 , 8($0)
	lw   $31, 0($29)	
	addi $29, $29, 4     	# 0x00000004
	lw   $3 , 8($0)	
	halt
	sub  $2 , $4 , $5 
	jr   $31
