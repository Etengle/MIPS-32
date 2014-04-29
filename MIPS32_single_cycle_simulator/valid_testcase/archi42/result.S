	.ORG 0x00000200 # $sp = 0x000004FF, wc_instr = 16, wc_data = 2
	lw   $0 , 0($0)	
	lw   $16, 0($0)	
	or   $17, $0 , $16
	add  $18, $16, $17
	addi $8 , $0 , 8     	# 0x00000008
	addi $8 , $8 , -4    	# 0xFFFFFFFC
	lw   $9 , 0($8)	
	addi $16, $8 , 16    	# 0x00000010
	and  $17, $17, $0 
	lw   $16, 8($8)	
	sw   $17, -4($0)
	halt
	halt
	halt
	halt
	halt
