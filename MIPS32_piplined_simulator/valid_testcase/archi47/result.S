	.ORG 0x00000270 # $sp = 0x000003FB, wc_instr = 26, wc_data = 6
	lw   $1 , 0($0)	
	addi $17, $1 , 100   	# 0x00000064
	addi $3 , $4 , 3     	# 0x00000003
	addi $5 , $6 , 5     	# 0x00000005
	addi $7 , $8 , 7     	# 0x00000007
	addi $13, $14, 13    	# 0x0000000D
	addi $15, $16, 15    	# 0x0000000F
	beq  $15, $18, 5     
	halt
	halt
	halt
	halt
	halt
	addi $29, $29, -8    	# 0xFFFFFFF8
	sw   $31, 4($29)
	sw   $4 , 0($29)
	addi $4 , $4 , -1    	# 0xFFFFFFFF
	lw   $4 , 0($29)	
	lw   $31, 4($29)	
	addi $29, $29, 8     	# 0x00000008
	add  $2 , $2 , $4 
	halt
	halt
	halt
	halt
	halt
