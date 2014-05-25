	.ORG 0x00000004 # $sp = 0x00000000, wc_instr = 10, wc_data = 0
	addi $17, $0 , 20    	# 0x00000014
	addi $18, $0 , 30    	# 0x0000001E
	add  $17, $17, $18
	add  $17, $19, $17
	add  $17, $17, $18
	halt
	halt
	halt
	halt
	halt
