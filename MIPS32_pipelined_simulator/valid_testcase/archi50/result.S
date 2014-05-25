	.ORG 0x00000270 # $sp = 0x00000001, wc_instr = 18, wc_data = 2
	sll  $0 , $0 , 0 
	sll  $0 , $0 , 0 
	addi $0 , $31, -1    	# 0xFFFFFFFF
	lui  $9 , -32768		# 0xFFFF8000
	sub  $0 , $31, $9 
	addi $10, $10, 1     	# 0x00000001
	beq  $29, $10, 1     
	add  $9 , $31, $9 
	jal  0x000000A5			# PC => 0x00000294
	jal  0x000000A6			# PC => 0x00000298
	jal  0x000000A7			# PC => 0x0000029C
	sw   $10, 2($0)
	sw   $10, 0($0)
	halt
	halt
	halt
	halt
	halt
