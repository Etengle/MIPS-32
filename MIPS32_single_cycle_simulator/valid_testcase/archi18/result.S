	.ORG 0x00000000 # $sp = 0x00000400, wc_instr = 25, wc_data = 3
	lh   $8 , 4($16)
	lh   $9 , 2($16)
	j    0x020D0000			# PC => 0x08340000
	j    0x00000486			# PC => 0x00001218
	j    0x030004AE			# PC => 0x0C0012B8
	jal  0x0000088E			# PC => 0x00002238
	jal  0x01000982			# PC => 0x04002608
	srl  $1 , $10, 22
	Unknown
	Unknown
	Unknown
	Unknown
	Unknown
	Unknown
	Unknown
	Unknown
	halt
	Unknown
	Unknown
	Unknown
	Unknown
	sll  $1 , $24, 0 
	Unknown
	halt
	halt
