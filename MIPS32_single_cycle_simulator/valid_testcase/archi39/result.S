	.ORG 0x00000000 # $sp = 0x00000400, wc_instr = 9, wc_data = 2
	lw   $1 , 0($0)	
	sh   $1 , 4($0)
	lw   $2 , 4($0)	
	lhu  $0 , 2($2)
	halt
	halt
	halt
	halt
	halt
