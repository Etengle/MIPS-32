/* Simple MIPS-32 assembler
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
 
#ifndef __SIMPLE_MIPS_ASSEMBLER__
#define __SIMPLE_MIPS_ASSEMBLER__

void CleanArrays();
void R_type(int ZipCode);
void J_type(int ZipCode);
void I_type(int ZipCode);
void ErrorMsg_str(int ZipCode, char* str);
void ErrorMsg_num(int ZipCode, int num);
void ErrorMsg_halt(int ZipCode);
void EnterInstrOrData(int data);
void ClearAllArg();
int FirstPass(FILE *fh, char* endian);
void WriteObjectFile(char* endian);
void Enter_R_instr(int opcode, int rs, int rt, int rd, int shamt, int funct);
void Enter_J_instr(int opcode, int address);
void Enter_I_instr(int opcode, int rs, int rt, int imm);
void Scanning_num(int *num);
void Scanning_reg(int *num);
void SkipSpaceAndComma(int offset);
void Scanning_imm(int *num);
void LowerCase(char buf[]);

#endif
