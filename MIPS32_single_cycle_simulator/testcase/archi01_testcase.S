	ORG. PC = 0 $sp = 0x03
	sll $0, $0, 0 		# write to $0     0000 0000
	lw  $0, 1($sp)		# write to $0 
	# shift part
	lw $1, 0($0)		# $1 = 0x8000000    data at 0
	srl $0, $1, 16 		# write to $0
	bne $0, $31, 3 		# see whether ($0 == 0)? 0003 1f14
	srl $1, $1, 16 		# $1 = 0x0008000
	sll $0, $1, 16 		# write to $0
	sll $1, $1, 16 		# $1 = 0x8000000
	sra $0, $1, 31 		# write to $0
	sra $1, $1, 31 		# $1 = 0xFFFFFFF
	addi $1, $1, 1 		# $1 = 0 
	# number overflow
	lw   $1,  1($sp)	# $1=0x7FFFFFFF     data at 4
	addi $0,  $1, 1 	# number overflow
	# correct
	addi $sp, $sp, -3   # $sp = 0
	addi $1, $0, 2^15-1 # $1 = 0x00007FFF
	addi $1, $1, 1 		# $1 = 0x00008000
	addi $1, $0, -2^16 	# $1 = 0xffff8000
	addi $1, $1, 1 		# $1 = 0xffff8001
	addi $1, $1, -2 	# $1 = 0xffff7fff
	# memory access
 	addi $1, $0, 0x400 
	addi $2, $0, 0xABCD 
	sb $2, -1($1)		# store to 1023
	lw $4, 1020($0) 	# $4 = 0xCD000000
	sh $2, -4($1)		# store to 1020
	lw $5, 1020($0)		# $5 = 0xCD00ABCD
	sb $2, -2($1)		# store to 1022
	lw $3, 1020($0)		# $03 = 0xCDCDABCD
	sh $2, -8($1)		# store to 1016
	lw $7, 1016($0)		# $07 = 0x0000ABCD			
	sw $2, -8($1)		# store to 1016
	lw $6, 1016($0)		# $06 = 0xFFFFABCD
	sb $2, -10($1)		# store to 1014
	lw $9, -12($1) 		# $9 = 0x00CD0000
	sh $2, 1014($0)		# store to 1014
	lw $8, -12($1) 		# $8 = 0xABCD0000
	# lb & lbu
	lbu $14, 1023($0)	# $14 = 0x000000CD ff03 0e90
	lb $15, 1023($0)	# $15 = 0xFFFFFFCD ff03 0f80
	# addi & bne & sll
	addi $10, $10, -2^15# $10 = 0xFFFF8000 0080 4a21
	addi $11, $0, 0xfffe# feff 0b20
Back:	
	and $10, $10, $11 	# e457 6a01
	sll $11, $11, 1 	# 4058 eb03
	bne $10, $0, Back 	# fdff 0a14		Back = -3
	# nor & bne & sra
	addi $10, $10, 7	# 0700 4a21
	lui $11, 0x8000 	# 0080 eb3f 
Back2:
	nor $12, $10, $11 	# e767 4b01
	sra $11, $11, 1 	# 4358 eb03
	bne $12, $0, Back2 	# fdff 8015		Back2 = -3
	# ori
	ori $13, $8 , 0xEf12# $13 = 0x0ABCEf12 12ef 0d35
	# lb, odd offset 
	lb $0, 13($0)   	# write to $0 (0d00 0080)
	lb $1, 29($0)   	# Ok
	# sub number overflow
	lw $1, 0($sp) 		# $1 = 0x80000000      data at 0
	sub $1, $0, $1 		# -$1 doesn't exist, number overflow
	sub $25, $8, $1 	# e2cf 0101, -$1 doesn't exist, number overflow, $8 = 0x2BCD0000
	addi $12, $1, -1 	# $12 = 0x7FFFFFFF, number overflow, ffff 2c20
	sub $12, $12, $1 	# -$1 doesn't exist, number overflow, e267 8101, $12 = 0xffffffff
	addi $1, $1, -1 	# $1 = 0x7FFFFFFF, number overflow, ffff 2120
	addi $1, $1, 1 		# $1 = 0x80000000, number overflow
	# lui
	lui $0, -2^15 		# write to $0
	bne $0, $31, 2 		# see whether ($0 == 0)? 0002 1f14 
	lui $1, -2^15 		# $1 = 0x80000000
	sub $1, $1, $1 		# -$1 doesn't exist, number overflow
	# multi error
	lw $1, 0($sp)
	sub $0, $1, $1 		# write to $0 & number overflow 
	bne $0, $31, 1 		# see whether ($0 == 0)? 0001 1f14 
	addi $sp, $sp, 3 	# $sp => 3
	# three errors
	lh	$0,	1022($sp)	# access address 1025, Write $0 error & Address overflow & Misalignment error
	halt
