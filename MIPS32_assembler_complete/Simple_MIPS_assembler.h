/* Simple MIPS-32 assembler
 * About Author: 
 	College:		NTHU 
	Major: 			Department of Mathematics
	Double major: 	Department of Computer Science
	Student ID: 	101021120 
	Name: 			Yu Xiang, Zheng
 *
 * Filename: Simple_MIPS_assembler.c
 * Status: 
 *   Compile, simulate, execute successfully and impeccably.
 */
 
#ifndef __SIMPLE_MIPS_ASSEMBLER__
#define __SIMPLE_MIPS_ASSEMBLER__

void CleanArrays();
void R_type(int ZipCode, FILE *fh);
void J_type(int ZipCode, FILE *fh);
void I_type(int ZipCode, FILE *fh);
void ErrorMsg_str(int ZipCode, char* str);
void ErrorMsg_num(int ZipCode, int num);
void ErrorMsg_halt(int ZipCode);
void ErrorMsg_2(int ZipCode, char* str, int line);
void ErrorMsg_label(int ZipCode, char* str, int ln);
void SkipSpaceAndComma(int offset, FILE *fh);
void Scanning_imm_label(int *num, int ZipCode);
void EnterInstrOrData(int data, char *label);
void ClearAllArg();
void WriteObjectFile(char* endian);
void WriteSymbolFile(char *name);
int FirstPass(FILE *fh, char *name);
int SecondPass(FILE *fh, char* endian);
int FindMaxLength();
void Enter_R_instr(int opcode, int rs, int rt, int rd, int shamt, int funct);
void Enter_J_instr(int opcode, int address, char *label);
void Enter_I_instr(int opcode, int rs, int rt, int imm, char *label);
void Scanning_num(int *num, FILE *fh);
void Scanning_reg(int *num, FILE *fh);
void Scanning_imm_label(int *num, FILE *fh);
void LowerCase(char *buf);

#endif
