	.ORG 0x00000150 # $sp = 0x00000300, wc_instr = 16, wc_data = 6
	lw   $0 , 0($0)	
	addi $4 , $0 , 1     	# 0x00000001
	sub  $1 , $4 , $0 
	sll  $4 , $4 , 31
	add  $5 , $4 , $4 
	lui  $4 , 32767 		# 0x00007FFF
	lui  $5 , -10844		# 0xFFFFD5A4
	srl  $5 , $5 , 16
	add  $4 , $4 , $5 
	lhu  $0 , 21924($4)
	addi $1 , $0 , -1    	# 0xFFFFFFFF
	halt
	halt
	halt
	halt
	halt
