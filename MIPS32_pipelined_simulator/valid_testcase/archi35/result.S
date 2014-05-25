	.ORG 0x00000270 # $sp = 0x00000300, wc_instr = 19, wc_data = 2
	addi $18, $0 , 14    	# 0x0000000E
	addi $19, $18, -16   	# 0xFFFFFFF0
	lw   $20, 0($0)	
	slt  $16, $19, $20
	add  $21, $19, $20
	slt  $17, $19, $21
	bne  $20, $21, 0     
	srl  $0 , $20, 2 
	srl  $0 , $20, 2 
	addi $22, $22, 1     	# 0x00000001
	bne  $20, $22, -5    
	sw   $19, 4($0)
	sw   $20, 3($19)
	lui  $0 , 18    		# 0x00000012
	halt
	halt
	halt
	halt
	halt
