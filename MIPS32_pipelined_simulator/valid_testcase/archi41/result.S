	.ORG 0x00000000 # $sp = 0x00000000, wc_instr = 21, wc_data = 6
	addi $1 , $0 , 128   	# 0x00000080
	lw   $8 , 0($0)	
	lw   $9 , 0($8)	
	lw   $10, 4($8)	
	add  $13, $9 , $10
	add  $13, $13, $13
	addi $8 , $8 , 4     	# 0x00000004
	sw   $13, 0($8)
	bne  $1 , $8 , -7    
	halt
	halt
	halt
	halt
	halt
	halt
	halt
	halt
	halt
	halt
	halt
	halt
