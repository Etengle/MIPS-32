	.ORG 0x00000200 # $sp = 0x00000350, wc_instr = 32, wc_data = 4
	addi $29, $29, -4    	# 0xFFFFFFFC
	sw   $31, 0($29)
	lw   $0 , 0($6)	
	addi $8 , $8 , 50    	# 0x00000032
	addi $9 , $9 , 37    	# 0x00000025
	jal  0x00000091			# PC => 0x00000244
	sw   $2 , 4($0)
	lw   $31, 0($29)	
	addi $29, $29, 4     	# 0x00000004
	addi $5 , $5 , 1     	# 0x00000001
	lw   $4 , 0($5)	
	jr   $31
	halt
	halt
	halt
	halt
	halt
	beq  $8 , $9 , 2     
	addi $2 , $0 , 1     	# 0x00000001
	jr   $31
	slt  $10, $8 , $9 
	beq  $10, $0 , 5     
	addi $9 , $9 , -3    	# 0xFFFFFFFD
	jal  0x00000091			# PC => 0x00000244
	lw   $31, 0($29)	
	addi $29, $29, 4     	# 0x00000004
	jr   $31
	addi $9 , $8 , 0     	# 0x00000000
	jal  0x00000091			# PC => 0x00000244
	lw   $31, 0($29)	
	addi $29, $29, 4     	# 0x00000004
	jr   $31
