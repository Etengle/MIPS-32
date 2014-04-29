	.ORG 0x00000064 # $sp = 0x00000040, wc_instr = 24, wc_data = 5
	addi $8 , $0 , 20    	# 0x00000014
	bne  $0 , $8 , 8     
	halt
	halt
	halt
	halt
	halt
	halt
	halt
	halt
	add  $9 , $8 , $29
	lh   $10, 2($0)
	sll  $10, $10, 16
	slt  $11, $10, $0 
	bne  $11, $0 , 5     
	lb   $12, 3($8)
	sll  $12, $12, 16
	add  $13, $10, $12
	addi $8 , $8 , -4    	# 0xFFFFFFFC
	j    0x0000001A			# PC => 0x00000068
	lw   $12, 0($8)	
	add  $13, $10, $12
	addi $8 , $8 , -4    	# 0xFFFFFFFC
	j    0x0000001A			# PC => 0x00000068
