	.ORG 0x00000000 # $sp = 0x00000000, wc_instr = 19, wc_data = 4
	sll  $0 , $0 , 0 
	lw   $2 , 0($0)	
	sw   $2 , 20($0)
	bne  $2 , $1 , 1     
	addi $2 , $2 , 1     	# 0x00000001
	addi $3 , $1 , 1     	# 0x00000001
	sll  $3 , $1 , 2 
	addi $1 , $0 , 1     	# 0x00000001
	addi $2 , $1 , 1     	# 0x00000001
	bne  $2 , $1 , 0     
	sw   $2 , 24($0)
	addi $31, $31, 56    	# 0x00000038
	jr   $31
	addi $6 , $6 , 1     	# 0x00000001
	halt
	halt
	halt
	halt
	halt
