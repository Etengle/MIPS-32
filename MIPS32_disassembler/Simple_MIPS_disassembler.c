/* Disassembler for MIPS
 * About the author: 
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

#include <stdio.h>
#include <stdlib.h>
#include "Simple_MIPS_disassembler.h"
#define MAXSIZE 1023

static size_t fileSize_d, fileSize_i;
static int *mem_code_d, *mem_code_i;
//static int progSize_d, progSize_i;
static int mem_data_d[256], mem_data_i[256], sp, PC = 0, word_num_i = 0, word_num_d = 0;
static int opcode = 0, rs = 0, rt = 0, rd = 0, shamt = 0, funct = 0, imm = 0, address = 0;
static int CycleCount = 0;

int main(void){
	FILE *fh_s = fopen("result.S","w");
	int i;
	ReadFile();
	CleanArrays(); /* clean array to all zero */
	distribute(); /* load data to memory */
	fprintf(fh_s,"\t.ORG 0x%08X # $sp = 0x%08X, wc_instr = %d, wc_data = %d\n", PC, sp, word_num_i, word_num_d);
	for (i = 0; i < word_num_i; i++){
		CycleCount++;
		decode(mem_data_i[PC/4]);
	}	
	fclose(fh_s);
	return 0;
}

/* This function opens and reads the bin file, allocates code memory,
 * and in case of errors, it prints the error and exits.*/
void ReadFile(void){
	FILE *fh_d = fopen("dimage.bin","rb"); /* open as binary */
	FILE *fh_i = fopen("iimage.bin","rb"); /* open as binary */
	if (fh_d == NULL){
		fprintf(stderr, "Illegal testcase: Cannot open \"dimage.bin\"\n");
		exit(2);
	}
	if (fh_i == NULL){
		fprintf(stderr, "Illegal testcase: Cannot open \"iimage.bin\"\n");
		exit(2);
	}
	/* find the file size */
	fseek(fh_d, 0L, SEEK_END); /* go to the end of file */
	fileSize_d = ftell(fh_d);    /* find where it is */
	fseek(fh_d, 0L, SEEK_SET); /* go to the beginning */
	
	fseek(fh_i, 0L, SEEK_END); /* go to the end of file */
	fileSize_i = ftell(fh_i);    /* find where it is */
	fseek(fh_i, 0L, SEEK_SET); /* go to the beginning */
	/* now allocate a buffer of the size */
	mem_code_d = malloc(fileSize_d);
	mem_code_i = malloc(fileSize_i);
	if (mem_code_d == NULL || mem_code_i == NULL){
		fprintf(stderr, "Cannot allocate dynamic memory\n");
		exit(3);
	}
	fread(mem_code_d, fileSize_d, 1, fh_d);
	fread(mem_code_i, fileSize_i, 1, fh_i);
	fclose(fh_d);
	fclose(fh_i);
	/* check if file is empty */
	if (fileSize_d == 0 && fileSize_i == 0){
		return;
	}
	/* convert from little endian to big endian format */
	{
		int i;
		union {
			int i;
			char buf[4];
		} u;
		u.i = 0x12345678;
		if (u.buf[0] == 0x12){ /* big endian */
			mem_code_d[1] = mem_code_d[1] << 24 | ((mem_code_d[1] << 8) & 0xff0000) | ((mem_code_d[1] >> 8) & 0xff00) | ((mem_code_d[1] >> 24) & 0xff);
			for ( i = 0; i < mem_code_d[1] + 2 ; i++){
				mem_code_d[i] = mem_code_d[i] << 24 | ((mem_code_d[i] << 8) & 0xff0000) | ((mem_code_d[i] >> 8) & 0xff00) | ((mem_code_d[i] >> 24) & 0xff);
			}
			mem_code_i[1] = mem_code_i[1] << 24 | ((mem_code_i[1] << 8) & 0xff0000) | ((mem_code_i[1] >> 8) & 0xff00) | ((mem_code_i[1] >> 24) & 0xff);
			for ( i = 0; i < mem_code_i[1] + 2; i++){
				mem_code_i[i] = mem_code_i[i] << 24 | ((mem_code_i[i] << 8) & 0xff0000) | ((mem_code_i[i] >> 8) & 0xff00) | ((mem_code_i[i] >> 24) & 0xff);
			}
		}
	}
}

/* clean arrays and to all zero */
void CleanArrays(void){
	int i;
	for (i = 0; i < 256; i++){
		mem_data_d[i] = 0;
		mem_data_i[i] = 0;
	}
}

/* load data to memory */
void distribute(){
	int i, j;
	PC = mem_code_i[0]; /* PC */
	sp = mem_code_d[0]; /* $sp */
	word_num_i = mem_code_i[1]; /* word amount */
	word_num_d = mem_code_d[1]; /* word amount */
	/*allocate*/
	if (PC + word_num_i > 1024) {
		/* memory overflow */
		fprintf(stderr, "Illegal testcase: instruction fetch over 1K space\n");
		exit(7);
	}
	if (word_num_d > 1024) {
		/* memory overflow */
		fprintf(stderr, "Illegal testcase: data fetch over 1K space\n");
		exit(7);
	}
	for(i = PC/4, j = 2; i < PC/4 + word_num_i; i++, j++) {
		mem_data_i[i] = mem_code_i[j];
	}
	for(i = 0, j = 2; i < word_num_d; i++, j++){
		mem_data_d[i] = mem_code_d[j];
	}
	free(mem_code_d);
	free(mem_code_i);
}

/* decode the data */
void decode(int IR){
	FILE *fh_s = fopen("result.S","a");
	opcode = (IR >> 26) & 0x3f;
	PC += 4;
	if (opcode == 0x3f) fprintf(fh_s,"\thalt\n"); 
	else if (opcode == 0x00) R_type(IR);
	else if (opcode == 0x02 || opcode == 0x03) J_type(IR);
	else I_type(IR);
	return;
}

/* R-type Instruction */
void R_type(int IR){
	FILE *fh_s = fopen("result.S","a");
	funct = IR & 0x3f;
	rs = (IR >> 21) & 0x1f;
	rt = (IR >> 16) & 0x1f;
	rd = (IR >> 11) & 0x1f;
	shamt = (IR >> 6) & 0x1f;
	/* test if is unknown R-Type instruction */
	/* Why I test this first is that if is unknown instruction, 
	   I don't know whether or not if it try to write to $0.*/
	if (funct != 0x20 && funct != 0x22 && 
		funct != 0x24 && funct != 0x25 && 
		funct != 0x26 && funct != 0x27 &&
		funct != 0x28 && funct != 0x2A &&
		funct != 0x00 && funct != 0x02 &&
		funct != 0x03 && funct != 0x08 ){
		fprintf(stderr, "Illegal testcase: Unknown R-Type Instruction in cycle: %d\n", CycleCount);
		fprintf(fh_s, "\tUnknown\n");
		return;
	}
	switch (funct){
		case 0x20: /* add */ 
					fprintf(fh_s, "\tadd  $%-2d, $%-2d, $%-2d\n", rd, rs, rt);
					break;
		case 0x22: /* sub */
					fprintf(fh_s, "\tsub  $%-2d, $%-2d, $%-2d\n", rd, rs, rt);
					break;
		case 0x24: /* and */
					fprintf(fh_s, "\tand  $%-2d, $%-2d, $%-2d\n", rd, rs, rt);
					break;
		case 0x25: /* or */
					fprintf(fh_s, "\tor   $%-2d, $%-2d, $%-2d\n", rd, rs, rt);
					break;
		case 0x26: /* xor */
					fprintf(fh_s, "\txor  $%-2d, $%-2d, $%-2d\n", rd, rs, rt);
					break;
		case 0x27: /* nor */
					fprintf(fh_s, "\tnor  $%-2d, $%-2d, $%-2d\n", rd, rs, rt);
					break;
		case 0x28: /* nand */
					fprintf(fh_s, "\tnand $%-2d, $%-2d, $%-2d\n", rd, rs, rt);
					break;
		case 0x2A: /* slt (set if less than) */
					fprintf(fh_s, "\tslt  $%-2d, $%-2d, $%-2d\n", rd, rs, rt);
					break;
		case 0x00: /* sll (shift left logic) */
					fprintf(fh_s, "\tsll  $%-2d, $%-2d, %-2d\n", rd, rt, shamt);
					break;
		case 0x02: /* srl (shift right logic) */
				    fprintf(fh_s, "\tsrl  $%-2d, $%-2d, %-2d\n", rd, rt, shamt);
					break;
		case 0x03: /* sra (shift right arithmetic) */
				    fprintf(fh_s, "\tsra  $%-2d, $%-2d, %-2d\n", rd, rt, shamt);
				   	break;
		case 0x08: /* jr (jump register) */
				   	fprintf(fh_s, "\tjr   $%-2d\n", rs);
					break;			
		default: 
					break;
	}
}

/* J-type Instruction */
void J_type(int IR){
	FILE *fh_s = fopen("result.S","a");
	address = IR & 0x03ffffff;
	switch (opcode){
		case 0x03: /* jal (jump and link) */
					fprintf(fh_s, "\tjal  0x%08X\t\t\t# PC => 0x%08X\n", address, (PC & 0xf0000000) | 4*address);
					break;
		case 0x02: /* j (jump)*/
					fprintf(fh_s, "\tj    0x%08X\t\t\t# PC => 0x%08X\n", address, (PC & 0xf0000000) | 4*address);
					break;
		default: /* Unknown R-Type Instruction */
					fprintf(stderr, "Illegal testcase: Unknown J-Type Instruction in cycle: %d\n", CycleCount);
					fprintf(fh_s, "\tUnknown\n");
					break;
	}
}

/* I-type Instruction */
void I_type(int IR){
	FILE *fh_s = fopen("result.S","a");
	rs = (IR >> 21) & 0x1f;
	rt = (IR >> 16) & 0x1f;
	imm = ((IR & 0xffff) << 16) >> 16; /* sign extension */
	if (opcode != 0x08 && opcode != 0x23 && 
		opcode != 0x21 && opcode != 0x25 && 
		opcode != 0x20 && opcode != 0x24 &&
		opcode != 0x2B && opcode != 0x29 &&
		opcode != 0x28 && opcode != 0x0F &&
		opcode != 0x0C && opcode != 0x0D &&
		opcode != 0x0E && opcode != 0x0A &&
		opcode != 0x04 && opcode != 0x05 ){
		fprintf(stderr, "Illegal testcase: Unknown I-Type Instruction in cycle: %d\n", CycleCount);
		fprintf(fh_s, "\tUnknown\n");
		return;
	}
	switch (opcode){
		case 0x08: /* addi (add immediate) */
					fprintf(fh_s, "\taddi $%-2d, $%-2d, %-6d\t# 0x%08X\n", rt, rs, imm, imm);
					break;
		case 0x23: /* lw (load word) */
					fprintf(fh_s, "\tlw   $%-2d, %d($%d)\t\n", rt, imm, rs);
					break;
        case 0x21: /* lh (load halfword) */
					fprintf(fh_s, "\tlh   $%-2d, %d($%d)\n", rt, imm, rs);
					break;
        case 0x25: /* lhu (load halfword unsigned) */
					fprintf(fh_s, "\tlhu  $%-2d, %d($%d)\n", rt, imm, rs);
					break;
        case 0x20: /* lb (load one byte) */
					fprintf(fh_s, "\tlb   $%-2d, %d($%d)\n", rt, imm, rs);
					break;
		case 0x24: /* lbu (load one byte unsigned) */
					fprintf(fh_s, "\tlbu  $%-2d, %d($%d)\n", rt, imm, rs);
					break;
		case 0x2B: /* sw (store word) */
					fprintf(fh_s, "\tsw   $%-2d, %d($%d)\n", rt, imm, rs);
					break;
		case 0x29: /* sh (store halfword) */
					fprintf(fh_s, "\tsh   $%-2d, %d($%d)\n", rt, imm, rs);
					break;
		case 0x28: /* sb (store one byte) */
					fprintf(fh_s, "\tsb   $%-2d, %d($%d)\n", rt, imm, rs);
					break;
		case 0x0F: /* lui (load upper halfword immediate) */
					fprintf(fh_s, "\tlui  $%-2d, %-6d\t\t# 0x%08X\n", rt, imm, imm);
					break;
		case 0x0C: /* andi (and immediate) */
					fprintf(fh_s, "\tandi $%-2d, $%-2d, %-6d\t# 0x%08X\n", rt, rs, imm, imm);
					break;
		case 0x0D: /* ori (or immediate)  */
					fprintf(fh_s, "\tori  $%-2d, $%-2d, %-6d\t# 0x%08X\n", rt, rs, imm, imm);
					break;
		case 0x0E: /* nori (nor immediate) */
					fprintf(fh_s, "\tnori $%-2d, $%-2d, %-6d\t# 0x%08X\n", rt, rs, imm, imm);
					break;
		case 0x0A: /* slti (set if less than immediate) */
					fprintf(fh_s, "\tslti $%-2d, $%-2d, %-6d\t# 0x%08X\n", rt, rs, imm, imm);
					break;
		case 0x04: /* beq (branch if equal) */
					fprintf(fh_s, "\tbeq  $%-2d, $%-2d, %-6d\n", rs, rt, imm);
					break;
		case 0x05: /* bne (branch if not equal) */
					fprintf(fh_s, "\tbne  $%-2d, $%-2d, %-6d\n", rs, rt, imm);
					break;
		default: 
					break;
	}
}
