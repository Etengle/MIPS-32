	.ORG 0x00000000 # $sp = 0x00000000, wc_instr = 24, wc_data = 1
	addi $1 , $0 , -1    	# 0xFFFFFFFF
	sw   $1 , 0($0)
	addi $2 , $0 , 12    	# 0x0000000C
	lw   $3 , 0($0)	
	add  $4 , $2 , $3 
	add  $4 , $4 , $3 
	addi $2 , $2 , -1    	# 0xFFFFFFFF
	sw   $2 , 4($0)
	beq  $2 , $4 , 2     
	addi $2 , $2 , -1    	# 0xFFFFFFFF
	j    0x00000008			# PC => 0x00000020
	addi $11, $0 , 1     	# 0x00000001
	addi $12, $0 , -2    	# 0xFFFFFFFE
	bne  $11, $12, 7     
	lw   $11, 0($0)	
	beq  $1 , $11, 6     
	halt
	halt
	halt
	halt
	halt
	j    0x0000000E			# PC => 0x00000038
	add  $1 , $12, $1 
	j    0x0000000E			# PC => 0x00000038
