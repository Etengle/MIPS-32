	.ORG 0x00000128 # $sp = 0x00000400, wc_instr = 29, wc_data = 4
	lui  $8 , -1348 		# 0xFFFFFABC
	ori  $8 , $8 , -30020	# 0xFFFF8ABC
	lw   $0 , 8($0)	
	lw   $9 , 8($0)	
	lh   $0 , 2($0)
	lh   $10, 2($0)
	lhu  $0 , 4($0)
	lhu  $11, 4($0)
	lb   $0 , 3($0)
	lb   $10, 2($0)
	lbu  $0 , 4($0)
	lbu  $12, 5($0)
	add  $13, $8 , $9 
	add  $14, $8 , $9 
	beq  $13, $14, 5     
	lui  $8 , -32768		# 0xFFFF8000
	addi $9 , $8 , -1    	# 0xFFFFFFFF
	beq  $8 , $9 , 1024  
	halt
	halt
	sw   $10, -4($29)
	sh   $8 , -2($29)
	sb   $9 , -4($29)
	lw   $14, -4($29)	
	sra  $14, $14, 4 
	srl  $14, $14, 8 
	bne  $14, $13, -13   
	halt
	halt
