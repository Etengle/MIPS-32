	.ORG 0x00000000 # $sp = 0x00000000, wc_instr = 19, wc_data = 3
	lw   $1 , 0($0)	
	add  $3 , $2 , $1 
	add  $3 , $1 , $1 
	lh   $4 , 0($0)
	jr   $4 
	add  $3 , $2 , $1 
	add  $10, $11, $12
	sub  $9 , $10, $10
	xor  $12, $9 , $10
	beq  $0 , $0 , 2     
	lw   $5 , 0($7)	
	nand $3 , $3 , $3 
	lw   $3 , 0($0)	
	beq  $0 , $3 , 1     
	halt
	halt
	halt
	halt
	halt
