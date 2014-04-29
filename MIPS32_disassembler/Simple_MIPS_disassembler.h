/* Disassembler for MIPS
 * About Author: 
 	College:		NTHU 
	Major: 			Department of Mathematics
	Double major: 	Department of Computer Science
	Student ID: 	101021120 
	Name: 			Yu Xiang, Zheng
	Nickname: 		Slighten
	E-mail: 		zz687@yahoo.com.tw
	FB: 			https://www.facebook.com/Slighten.Zheng
	Cellphone: 		0923576510
 *
 * Filename: Simple_MIPS_assembler.c
 * Status: 
 *   Compile, simulate, execute successfully and impeccably.
 */

#ifndef __SIMPLE_MIPS_DISASSEMBLER__
#define __SIMPLE_MIPS_DISASSEMBLER__

void CleanArrays(void); /* clean arrays and to all zero */
void distribute(void); /* load data to memory */
void R_type(int IR); /* R-type Instruction */
void I_type(int IR); /* I-type Instruction */
void J_type(int IR); /* J-type Instruction */
void decode(int IR); /* decode the data */
void ReadFile(void); /* This function opens and reads the bin file, allocates code memory, and in case of errors, it prints the error and exits.*/
 
#endif
