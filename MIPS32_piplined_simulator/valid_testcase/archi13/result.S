	.ORG 0x00000200 # $sp = 0x00000400, wc_instr = 13, wc_data = 1
	addi $0 , $0 , 1     	# 0x00000001
	lw   $1 , 0($0)	
	add  $2 , $1 , $1 
	sub  $3 , $0 , $1 
	add  $0 , $3 , $3 
	sub  $5 , $1 , $3 
	sub  $5 , $3 , $1 
	lw   $0 , 3($1)	
	halt
	halt
	halt
	halt
	halt
