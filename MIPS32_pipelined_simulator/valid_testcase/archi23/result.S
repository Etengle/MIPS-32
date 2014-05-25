	.ORG 0x00000200 # $sp = 0x00000800, wc_instr = 18, wc_data = 3
	addi $5 , $5 , 0     	# 0x00000000
	addi $10, $10, 4     	# 0x00000004
	addi $15, $15, 8     	# 0x00000008
	lw   $8 , 0($5)	
	lw   $9 , 0($10)	
	addi $8 , $8 , 3     	# 0x00000003
	addi $8 , $8 , 3     	# 0x00000003
	sw   $8 , 0($15)
	lw   $8 , 0($15)	
	bne  $9 , $8 , -5    
	addi $8 , $8 , 9     	# 0x00000009
	jal  0x0000008C			# PC => 0x00000230
	addi $8 , $8 , 9     	# 0x00000009
	halt
	halt
	halt
	halt
	halt
