	.ORG 0x00000270 # $sp = 0x00000001, wc_instr = 10, wc_data = 2
	addi $0 , $31, -1    	# 0xFFFFFFFF
	lui  $9 , -32768		# 0xFFFF8000
	sub  $0 , $31, $9 
	addi $10, $10, 1     	# 0x00000001
	sll  $0 , $0 , 0 
	beq  $29, $10, -7    
	add  $9 , $31, $9 
	sw   $10, 2($0)
	sw   $10, 0($0)
	halt
