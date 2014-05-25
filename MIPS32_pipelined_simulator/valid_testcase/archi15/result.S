	.ORG 0x00000000 # $sp = 0x00000400, wc_instr = 14, wc_data = 4
	lw   $0 , 0($0)	
	sb   $0 , 768($0)
	lw   $31, 0($0)	
	sb   $31, 768($0)
	addi $4 , $31, 0     	# 0x00000000
	beq  $4 , $31, 0     
	jal  0x00000007			# PC => 0x0000001C
	addi $4 , $31, 8     	# 0x00000008
	jr   $4 
	halt
	halt
	halt
	halt
	halt
