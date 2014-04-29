	.ORG 0x00000000 # $sp = 0x00000400, wc_instr = 12, wc_data = 3
	addi $0 , $0 , 10    	# 0x0000000A
	addi $1 , $0 , 1     	# 0x00000001
	lw   $2 , 0($0)	
	add  $2 , $1 , $2 
	lw   $3 , 4($0)	
	lw   $4 , 8($0)	
	addi $0 , $3 , 10    	# 0x0000000A
	halt
	halt
	halt
	halt
	halt
