/* Architecture Project 1
 * Team number: Archi01
 * Leader: 101021120, Y.Xiang Zheng (Slighten), student01
 * Member: x1022108, S.Jie Cao, student06
 * First submission Due: 2013-04-05 23:30
 * Second submission Due: 2014-04-12 23:30
 * Filename: Simple_MIPS_simulator.c
 * Status: 
 *   Compile, simulate, execute successfully and impeccably. 
 */

#include <stdio.h>
#include <stdlib.h>
#include "Simple_MIPS_simulator.h"
#define MAXSIZE 1023

static size_t fileSize_d, fileSize_i;
static int *mem_code_d, *mem_code_i;
//static int progSize_d, progSize_i;
static int mem_data_d[256], mem_data_i[256], R[32], PC = 0, word_num_i = 0, word_num_d = 0;
static int opcode = 0, rs = 0, rt = 0, rd = 0, shamt = 0, funct = 0, imm = 0, address = 0;
static int MSBrs = 0, MSBrt = 0, HaltFlag = 0, saved_Rrt = 0, ALUzero = 0, ALUout = 0;
static unsigned int CycleCount = 0;

int main(void){
	int i;
	FILE *fh_s = fopen("snapshot.rpt","w");
	FILE *fh_e = fopen("error_dump.rpt","w");
	ReadFile();
	CleanArrays(); /* clean array to all zero */
	distribute(); /* load data to memory */
	while(1){
		initialize(); /* reset variables */
		// Cycle amount should be less than 500K, and avoid infinite loop.
		if (CycleCount > 500000) {
			fprintf(stderr, "Illegal testcase: Cycle amount over 500K\n");
			exit(EXIT_FAILURE); 
		}
		// start outputting snapshot.rpt (1 cycle)
		fprintf(fh_s, "cycle %d\n", CycleCount);
		for (i = 0; i < 32; i++){
			fprintf(fh_s, "$%02d: 0x%08X\n", i, R[i]);
		}
		fprintf(fh_s, "PC: 0x%08X\n\n\n", PC);
		// end
		CycleCount++;
		/* Start IF */
		if (PC/4 > 255 || PC/4 < 0) ErrorMsg(3); /* Address overflow */
		if (PC % 4 != 0) ErrorMsg(4); /* Misalignment Error */
		if (HaltFlag == 1) exit(EXIT_FAILURE); /* If one of above two occurs, then halt. */
		decode(mem_data_i[PC/4]);
		if (opcode == 0x3f) break; /* halt */
	}
	fclose(fh_e);
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
	for (i = 0; i < 32; i++){
		R[i] = 0;
	}
}

/* reset variables */
void initialize(void){
	ALUout = ALUzero = saved_Rrt = R[0] = opcode = rs = rt = rd = shamt = 
	funct = imm = address = MSBrs = MSBrt = 0;
}

/* load data to memory */
void distribute(){
	int i, j;
	PC = mem_code_i[0]; /* PC */
	R[29] = mem_code_d[0]; /* $sp */
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
	opcode = (IR >> 26) & 0x3f;
	if (opcode == 0x3f) return; /* halt */
	PC += 4;
	if (opcode == 0x00) R_type(IR);
	else if (opcode == 0x02 || opcode == 0x03) J_type(IR);
	else I_type(IR);
	if (HaltFlag == 1) exit(EXIT_FAILURE);
	return;
}

/* R-type Instruction */
void R_type(int IR){
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
		exit(11);
	}
	if (rd == 0 && funct != 0x08){ // since jr doesn't use $rd
		ErrorMsg(1); /* Try to write to $0 */
	}
	switch (funct){
		case 0x20: /* add */ 
					/* save MSB */
					MSBrs = R[rs] >> 31 & 1; 
					MSBrt = R[rt] >> 31 & 1;
					R[rd] = R[rs] + R[rt];
					/* Number overflow (P+P=N or N+N=P) */
					if ( MSBrs == MSBrt && (R[rd] >> 31 & 1) != MSBrs )
						ErrorMsg(2); 
					break;
		case 0x22: /* sub */
					/* save MSB */
					MSBrs = R[rs] >> 31 & 1;
					MSBrt = R[rt] >> 31 & 1;
					saved_Rrt = R[rt];
					/* -0x80000000 doesn't exist */
					if (R[rt] == 0x80000000)
						R[rd] = R[rs] + R[rt];
					else
						R[rd] = R[rs] - R[rt];
					/* Number overflow (P-N=N or N-P=P) or -0x80000000 doesn't exist*/
					if ( (MSBrs != MSBrt && (R[rd] >> 31 & 1) == MSBrt) || saved_Rrt == 0x80000000) ErrorMsg(2); 
					break;
		case 0x24: /* and */
					R[rd] = R[rs] & R[rt];
					break;
		case 0x25: /* or */
					R[rd] = R[rs] | R[rt];
					break;
		case 0x26: /* xor */
					R[rd] = R[rs] ^ R[rt];
					break;
		case 0x27: /* nor */
					R[rd] = ~(R[rs] | R[rt]);
					break;
		case 0x28: /* nand */
					R[rd] = ~(R[rs] & R[rt]);
					break;
		case 0x2A: /* slt (set if less than) */
					R[rd] = R[rs] < R[rt];
					break;
		case 0x00: /* sll (shift left logic) */
					R[rd] = R[rt] << shamt;
					break;
		case 0x02: /* srl (shift right logic) */
				   /* ex: 0xffff1234 >> 8 = 0x00ffff12
				   		  0x00001234 >> 8 = 0x00000012 */
				   	/* for needing 32 - shamt  < 32 */ 
					if (shamt == 0)		R[rd] = R[rt];
					else // 0 < shamt <= 32	  	
						R[rd] = (R[rt] >> shamt) & ~(0xffffffff << (32 - shamt));
					break;
		case 0x03: /* sra (shift right arithmetic) */
				   /* ex: 0xffff1234 >> 8 = 0xffffff12
				   		  0x00001234 >> 8 = 0x00000012 */
				   	R[rd] = R[rt] >> shamt;
				   	break;
		case 0x08: /* jr (jump register) */
					PC = R[rs];
					break;			
		default: 
					break;
	}
}

/* J-type Instruction */
void J_type(int IR){
	address = IR & 0x03ffffff;
	switch (opcode){
		case 0x03: /* jal (jump and link) */
					R[31] = PC;
		case 0x02: /* j (jump)*/
					PC = (PC & 0xf0000000) | 4*address;
					break;
		default: /* Unknown R-Type Instruction */
					fprintf(stderr, "Illegal testcase: Unknown J-Type Instruction in cycle: %d\n", CycleCount);
					exit(11);
	}
}

/* I-type Instruction */
void I_type(int IR){
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
		exit(11);
	}
	if (rt == 0 && opcode != 0x2B && opcode != 0x29 && 
		opcode != 0x28 && opcode != 0x04 && opcode != 0x05){
		ErrorMsg(1); /* Try to write to $0 */
	}
	switch (opcode){
		case 0x08: /* addi (add immediate) */
					/* save MSB */
					MSBrs = R[rs] >> 31 & 1; 
					R[rt] = R[rs] + imm;
					/* Number overflow (P+P=N or N+N=P) */
					if ( MSBrs == (imm >> 31 & 1) && (R[rt] >> 31 & 1) != MSBrs )
						ErrorMsg(2);
					break;
		case 0x23: /* lw (load word) */
					AccessMemN_A_ErrorTest();
					/* Misalignment error */
					if ((R[rs] + imm) % 4 != 0) ErrorMsg(4);
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					R[rt] = mem_data_d[(R[rs] + imm)/4];
					break;
        case 0x21: /* lh (load halfword) */
					AccessMemN_A_ErrorTest();
					/* Misalignment error */
					if ((R[rs] + imm) % 2 != 0) ErrorMsg(4);
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					R[rt] = (mem_data_d[(R[rs] + imm)/4] << ((2 - finite_groupZ4(R[rs] + imm)) * 8)) >> 16; /* sign extension */
					break;
        case 0x25: /* lhu (load halfword unsigned) */
					AccessMemN_A_ErrorTest();
					/* Misalignment error */
					if ((R[rs] + imm) % 2 != 0) ErrorMsg(4);
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					R[rt] = (mem_data_d[(R[rs] + imm)/4] >> (finite_groupZ4(R[rs] + imm) * 8)) & 0xffff;
					break;
        case 0x20: /* lb (load one byte) */
					AccessMemN_A_ErrorTest();
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					R[rt] = (mem_data_d[(R[rs] + imm)/4] << ((3 - finite_groupZ4(R[rs] + imm)) * 8)) >> 24; /* sign extension */
					break;
		case 0x24: /* lbu (load one byte unsigned) */
					AccessMemN_A_ErrorTest();
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					R[rt] = (mem_data_d[(R[rs] + imm)/4] >> (finite_groupZ4(R[rs] + imm) * 8)) & 0xff;
					break;
		case 0x2B: /* sw (store word) */
					AccessMemN_A_ErrorTest();
					/* Misalignment error */
					if ((R[rs] + imm) % 4 != 0) ErrorMsg(4);
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					mem_data_d[(R[rs] + imm)/4] = R[rt];
					break;
		case 0x29: /* sh (store halfword) */
					AccessMemN_A_ErrorTest();
					/* Misalignment error */
					if ((R[rs] + imm) % 2 != 0) ErrorMsg(4);
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					mem_data_d[(R[rs] + imm)/4] &= ~(0xffff << (finite_groupZ4(R[rs] + imm) * 8)); /* clear */
					mem_data_d[(R[rs] + imm)/4] |= (R[rt] & 0xffff) << (finite_groupZ4(R[rs] + imm) * 8);
					break;
		case 0x28: /* sb (store one byte) */
					AccessMemN_A_ErrorTest();
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					mem_data_d[(R[rs] + imm)/4] &= ~(0xff << (finite_groupZ4(R[rs] + imm) * 8)); /* clear */
					mem_data_d[(R[rs] + imm)/4] |= (R[rt] & 0xff) << (finite_groupZ4(R[rs] + imm) * 8);
					break;
		case 0x0F: /* lui (load upper halfword immediate) */
					R[rt] = imm << 16;
					break;
		case 0x0C: /* andi (and immediate) */
					R[rt] = R[rs] & (imm & 0xffff);
					break;
		case 0x0D: /* ori (or immediate)  */
					R[rt] = R[rs] | (imm & 0xffff);
					break;
		case 0x0E: /* nori (nor immediate) */
					R[rt] = ~(R[rs] | (imm & 0xffff));
					break;
		case 0x0A: /* slti (set if less than immediate) */
					R[rt] = R[rs] < imm;
					break;
		case 0x04: /* beq (branch if equal) */

					if (R[rs] == R[rt])
						PC += 4*imm;
					break;
		case 0x05: /* bne (branch if not equal) */
					if (R[rs] != R[rt])
						PC += 4*imm;
					break;
		default: 
					break;
	}
}

/* error handler for possible error cases */
void ErrorMsg(int ZipCode){
	FILE *fh_e = fopen("error_dump.rpt","a");
	switch (ZipCode){
		case 1:/* Try to write to $0 */
				fprintf(fh_e, "Write $0 error in cycle: %d\n", CycleCount);
				break;
		case 2:/* Number overflow */
				fprintf(fh_e, "Number overflow in cycle: %d\n", CycleCount);
				break;
		case 3:/* Address overflow */
				fprintf(fh_e, "Address overflow in cycle: %d\n", CycleCount);
				HaltFlag = 1;
				break;
		case 4:/* Misalignment error */
				fprintf(fh_e, "Misalignment error in cycle: %d\n", CycleCount);
				HaltFlag = 1;
				break;
		default:
				break;
		}
		fclose(fh_e);
}

/* for memory accessing Number overflow & Address overflow error test */
void AccessMemN_A_ErrorTest(){
	/* Number overflow (P+P=N or N+N=P) */
	MSBrs = R[rs] >> 31 & 1;
	ALUout = R[rs] + imm;
	if ( MSBrs == (imm >> 31 & 1) && (ALUout >> 31 & 1) != MSBrs ) ErrorMsg(2);
	/* Address overflow */
	if (R[rs] + imm < 0 || R[rs] + imm > MAXSIZE) ErrorMsg(3);
}

/* for finite_groupZ4(-1) = 3, finite_groupZ4(5) = 1 */
int finite_groupZ4(int x){
	if (x < 0)
		return (x % 4) + 4;
	return x % 4;
}
