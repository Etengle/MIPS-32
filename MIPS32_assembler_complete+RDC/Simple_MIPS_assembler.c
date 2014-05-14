/* Simple MIPS-32 assembler_complete+RDC
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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#define MAX_LEN 100
#define MAX_SIZE 256
#define R_num 12
#define J_num 2
#define I_num 16
#define MAX_REG 32

static int linenum = 0, PC = 0, Pass1_Error_amount = 0, Pass2_Error_amount = 0, ORGflag = 0;
static int mem[MAX_SIZE];
static char *opcode_R[R_num] = {
		"add", "sub", "and", "or", "xor", "nor", "nand", "slt",
		"sll", "srl", "sra", "jr"
};
static char *opcode_I[I_num] = {
		"addi", "lw", "lh", "lhu", "lb", "lbu", "sw", "sh",
		"sb", "lui", "andi", "ori", "nori", "slti", "beq", "bne"
};
static char *opcode_J[J_num] = { "j", "jal" };
static char *reg_alias[32] = {
	"$zero", "$at", 
	"$v0", "$v1", 
	"$a0", "$a1", "$a2", "$a3", 
	"$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7", "$t8", "$t9", 
	"$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
	"$k0", "$k1",
	"$gp",
	"$sp",
	"$fp",
	"$ra"
};
static struct {
		int op_r;
		int op_i;
		int op_j;
		int op_s;
}CmpStruct, initStruct = {-1, -1, -1, -1}; 
static char Line_buffer[MAX_LEN] , Word_buffer[MAX_LEN], ErrorStr[MAX_LEN], TempStr[MAX_LEN];
static char Word_buffer_ORIGIN[MAX_LEN];
static int argsave = 0, argoffset = 0, arg1 = 0, arg2 = 0;
static int Progstart = 0;
static int M_index = 0;
static char labelArray[MAX_SIZE][MAX_LEN];
static int JF[MAX_SIZE];
typedef struct LB{
	char name[MAX_LEN];
	int address;
	int line;
	int length;
}LABEL;

static LABEL LabelStruct[MAX_SIZE];
static LABEL INIT = {"", 0, 0, 0};
static int Label_index = 0;

void CleanArrays(){
	int i;
	for (i = 0; i < MAX_SIZE; i++){	
		mem[i] = 0;
		labelArray[i][0] = '\0';
		LabelStruct[i] = INIT;
	}
}

void ErrorMsg_halt(int ZipCode){
	Pass1_Error_amount++;
	switch (ZipCode){
		case 1: 
			fprintf(stderr, "Line %d: Instruction uses memory beyond memory location hex 400.\n", linenum);
			break;
		case 2:
			fprintf(stderr, "Warning! Line %d: Duplicated .ORG or program has started, so this line takes no effect.\n", linenum);
			Pass1_Error_amount--;
			return;
			break;
		case 3:
			fprintf(stderr, "Line %d: This line has too many words!\n", linenum);
			break;
		case 4:
			fprintf(stderr, "Line %d: Label amount has been over the maximum which is %d.\n", linenum, MAX_SIZE);
	}
	fprintf(stderr, "Pass 1 - %d error(s)\n", Pass1_Error_amount);
	exit(EXIT_FAILURE);
}
void ErrorMsg_label(int ZipCode, char* str, int ln){
	Pass1_Error_amount++;
	switch (ZipCode){
		case 1:
			fprintf(stderr, "Line %d: Duplicate label '%s' found with label on line %d\n", linenum, str, ln);
			break;
	}
}

void ErrorMsg_str(int ZipCode, char* str){
	Pass1_Error_amount++;
	switch (ZipCode){
		case 1:
			fprintf(stderr, "Line %d: Expected 32-bit value, but found '%s' instead\n", linenum, str);
			break;
		case 2:
			fprintf(stderr, "Line %d: Expected an instruction opcode, but found '%s' instead\n", linenum, str);
			break;
		case 3:
			fprintf(stderr, "Line %d: Expected a register name, but found '%s' instead\n", linenum, str);
			break;	
		case 4:
			fprintf(stderr, "Line %d: Expected an hexadecimal address, but found '%s' instead\n", linenum, str);
			break;	
		case 5:
			fprintf(stderr, "Line %d: Expected 16-bit value, but found '%s' instead\n", linenum, str);
			break;
		case 6:
			fprintf(stderr, "Warning! Line %d: Expected .ORG, but found '%s' instead. Hence, PC will default to 0.\n", linenum, str);
			Pass1_Error_amount--;
			break;
		case 7:
			fprintf(stderr, "Line %d: Expected a shift amount value, but found '%s' instead\n", linenum, str);
			break;
	}
}

void ErrorMsg_2(int ZipCode, char* str, int line){
	Pass2_Error_amount++;
	switch (ZipCode){
		case 1:
			fprintf(stderr, "Line %d: Instruction references undefined label '%s'\n", line, str);
			break;
	}
}

void ErrorMsg_num(int ZipCode, int num){
	Pass1_Error_amount++;
	switch (ZipCode){
		case 1:
			fprintf(stderr, "Line %d: Register %d does not exist\n", linenum, num);
			break;
		case 2:
			fprintf(stderr, "Warning! Line %d: '%d' is beyond 16-bit value\n", linenum, num);
			Pass1_Error_amount--;
			break;
		case 3:
			fprintf(stderr, "Warning! Line %d: '%d' is beyond 26-bit value\n", linenum, num);
			Pass1_Error_amount--;
			break;
		case 4:
			fprintf(stderr, "Line %d: Shift amount '%d' is beyond the range of 0 to 31\n", linenum, num);
			break;
	}
}

void ClearAllArg(){
	arg2 = arg1 = argsave = argoffset = 0;
	TempStr[0] = '\0';
}

void SkipSpaceAndComma(int* offset, FILE *fh){
	for ( ; Line_buffer[*offset] == ' ' || Line_buffer[*offset] == '	' || Line_buffer[*offset] == ',' || 
		Line_buffer[*offset] == '(' || Line_buffer[*offset] == ')'; (*offset)++){
	}
	//printf("line: %d '%c'\n", linenum, Line_buffer[*offset]);
	if (Line_buffer[*offset] < 32){
		if (fgets(Line_buffer, MAX_LEN - 1, fh) != NULL){
			CmpStruct = initStruct;
			linenum++;
			ClearAllArg();
			//check if fgets too many words
			if(Line_buffer[strlen(Line_buffer)] != '\0') ErrorMsg_halt(3);
			SkipSpaceAndComma(&argoffset, fh);
		}
	}
}

void Scanning_imm_label(int *num, int ZipCode, FILE *fh){
	SkipSpaceAndComma(&argoffset, fh);
	if (sscanf(Line_buffer + argoffset,  "-0x%X%n", num, &arg2) == 1){
		*num = -(*num);
		if(*num > 32767 || *num < -32768)
			ErrorMsg_num(2, *num);
	}
	else if (sscanf(Line_buffer + argoffset,  "0x%X%n", num, &arg2) == 1 || 
		sscanf(Line_buffer + argoffset,  "%d%n", num, &arg2) == 1){
		if(*num > 32767 || *num < -32768)
			ErrorMsg_num(2, *num);
	}
	else{
		if (ZipCode == 0){
			sscanf(Line_buffer + argoffset, "%s%n", ErrorStr, &arg2);
			ErrorMsg_str(5, ErrorStr);
		}
		else
			sscanf(Line_buffer + argoffset, "%s%n", TempStr, &arg2);
	}
	argoffset += arg2;
}

void EnterInstrOrData(int data, char *label){
	if (M_index == 1) M_index++;
	if (M_index < MAX_SIZE){
		mem[M_index] = data;
		if(label != NULL && label[0] != '\0')
			strcpy(labelArray[M_index], label);
		//printf("line: %d %s\n", linenum, label);
	}
	else ErrorMsg_halt(1); 
	M_index++;
	PC += 4;
}

void Enter_R_instr(int opcode, int rs, int rt, int rd, int shamt, int funct){
	EnterInstrOrData((opcode << 26) | (rs & 0x1f) << 21 | (rt & 0x1f) << 16 | (rd & 0x1f) << 11 | (shamt & 0x3f) << 6 | funct, NULL);
}

void Enter_J_instr(int opcode, int address, char *label){
	EnterInstrOrData((opcode << 26) | (address & 0x3ffffff), label);
}

void Enter_I_instr(int opcode, int rs, int rt, int imm, char *label){
	EnterInstrOrData((opcode << 26) | (rs & 0x1f) << 21 | (rt & 0x1f) << 16 | (imm & 0xffff), label);
}

void LowerCase(char *buf){
	int i = 0;
	while (*(buf + i)){
		buf[i] = ('A' <= buf[i] && buf[i] <= 'Z') ? 
			  (buf[i] - 'A' + 'a') : (buf[i]);
		i++;
	};
}

void WriteObjectFile(char* endian){
	mem[1] = M_index - 2;
	FILE *fhobj = fopen("iimage.bin", "wb");
	/* endian transformation */
	{
		int i = 0;
		union {
			int i;
			char buf[4];
		} u;
		u.i = 0x12345678;
		if ((u.buf[0] == 0x12 && !strcmp(endian, "l")) || 
		(u.buf[0] == 0x78 && !strcmp(endian, "b"))) { /* endian transformation */
			for (i = 0 ; i < M_index; i++) {
				mem[i] = mem[i] << 24 | ((mem[i] << 8) & 0xff0000) | ((mem[i] >> 8) & 0xff00) | ((mem[i] >> 24) & 0xff);
			}
		}
	}
	fwrite(mem, M_index, sizeof(int), fhobj);
	fclose(fhobj);
}

void Scanning_reg(int *num, FILE *fh){
	int i = 0;
	SkipSpaceAndComma(&argoffset, fh);
	if (sscanf(Line_buffer + argoffset, "$%d%n", num, &arg2) == 1){
		if(*num > 31 || *num < 0) ErrorMsg_num(1, *num);
	}
	else{
		sscanf(Line_buffer + argoffset, "%s%n", ErrorStr, &arg2);
		for (i = 0 ; i < 32 ; i++){
			if(!strncmp(ErrorStr, reg_alias[i], 3)){
				*num = i;
				i = -1;
				break;
			}
		}
		//printf("%d %s\n",i, ErrorStr);
		if (i != -1)
			ErrorMsg_str(3, ErrorStr);
	}
	argoffset += arg2;
	//SkipSpaceAndComma(&argoffset);
}

void Scanning_num(int *num, FILE *fh){
	SkipSpaceAndComma(&argoffset, fh);
	if (sscanf(Line_buffer + argoffset, "0x%X%n", num, &arg2) == 1 || 
		sscanf(Line_buffer + argoffset,"%d%n", num, &arg2) == 1){
		if(*num > 31 || *num < 0) ErrorMsg_num(4, *num);
	}
	else{
		sscanf(Line_buffer + argoffset, "%s%n", ErrorStr, &arg2);
		ErrorMsg_str(7, ErrorStr);
	}
	argoffset += arg2;
}

void R_type(int ZipCode, FILE *fh){
	/*static char *opcode_R[R_num] = {
		"add", "sub", "and", "or", "xor", "nor", "nand", "slt",
		"sll", "srl", "sra", "jr"
	};*/
	int rs = 0, rt = 0, rd = 0;
	int mappingTable[R_num] = {
		0x20, 0x22, 0x24, 0x25, 0x26, 0x27, 0x28, 0x2A,
		0x00, 0x02, 0x03, 0x08
	};
	int funct = mappingTable[ZipCode];
	int opcode = 0, shamt = 0;
	srand(time(NULL));
	switch (funct){
		case 0x20: /* add */ 
		case 0x22: /* sub */
		case 0x24: /* and */
		case 0x25: /* or */
		case 0x26: /* xor */
		case 0x27: /* nor */
		case 0x28: /* nand */
		case 0x2A: /* slt (set if less than) */
				shamt = rand() % MAX_REG;
				Scanning_reg(&rd, fh);
				Scanning_reg(&rs, fh);
				Scanning_reg(&rt, fh);
				break;
		case 0x00: /* sll (shift left logic) */
		case 0x02: /* srl (shift right logic) */
		case 0x03: /* sra (shift right arithmetic) */
				rs = rand() % MAX_REG;
				Scanning_reg(&rd, fh);
				Scanning_reg(&rt, fh);
				Scanning_num(&shamt, fh);
				break;
		case 0x08: /* jr (jump register) */
				rd = rand() % MAX_REG;
				rt = rand() % MAX_REG;
				shamt = rand() % MAX_REG;
				Scanning_reg(&rs, fh);
				break;			
	}
	Enter_R_instr(opcode, rs, rt, rd, shamt, funct);
}

/* J-type Instruction */
void J_type(int ZipCode, FILE *fh){
	/*static char *opcode_J[J_num] = { "j", "jal" };*/
	int mappingTable[J_num] = {
		0x02, 0x03
	};
	int opcode = mappingTable[ZipCode], address = 0;
	switch (opcode){
		case 0x03: /* jal (jump and link) */
		case 0x02: /* j (jump)*/
			SkipSpaceAndComma(&argoffset, fh);
			//printf("line: %d, %c\n", linenum, Line_buffer[argoffset]);
			if (sscanf(Line_buffer + argoffset, "0x%X%n" , &address, &arg2) != 1){
				sscanf(Line_buffer + argoffset, "%s%n", TempStr, &arg2);
				//printf("line: %d, %s\n", linenum, TempStr);
				JF[M_index] = 1;
				// ErrorMsg_str(4, ErrorStr);
			}
			else { if (address > 33554431 || address < -33554432) ErrorMsg_num(3, address); } 
			break;
	}
	argoffset += arg2;
	Enter_J_instr(opcode, address, TempStr);
}



/* I-type Instruction */
void I_type(int ZipCode, FILE* fh){
	/*static char *opcode_I[I_num] = {
		"addi", "lw", "lh", "lhu", "lb", "lbu", "sw", "sh",
		"sb", "lui", "andi", "ori", "nori", "slti", "beq", "bne"	
	};*/
	int mappingTable[I_num] = {
		0x08, 0x23, 0x21, 0x25, 0x20, 0x24, 0x2B, 0x29,
		0x28, 0x0F, 0x0C, 0x0D, 0x0E, 0x0A, 0x04, 0x05
	};
	int opcode = mappingTable[ZipCode];
	int rs = 0, rt = 0, imm = 0;
	srand(time(NULL));
	switch (opcode){
		case 0x08: /* addi (add immediate) */
		case 0x0C: /* andi (and immediate) */
		case 0x0D: /* ori (or immediate)  */
		case 0x0E: /* nori (nor immediate) */
		case 0x0A: /* slti (set if less than immediate) */
					Scanning_reg(&rt, fh);
					Scanning_reg(&rs, fh);
					Scanning_imm_label(&imm, 0, fh);
					break;
		case 0x23: /* lw (load word) */
        case 0x21: /* lh (load halfword) */
        case 0x25: /* lhu (load halfword unsigned) */
        case 0x20: /* lb (load one byte) */
		case 0x24: /* lbu (load one byte unsigned) */
		case 0x2B: /* sw (store word) */
		case 0x29: /* sh (store halfword) */
		case 0x28: /* sb (store one byte) */
					Scanning_reg(&rt, fh);
					Scanning_imm_label(&imm, 0, fh);
					Scanning_reg(&rs, fh);
					break;
		case 0x0F: /* lui (load upper halfword immediate) */
					rs = rand() % MAX_REG;
					Scanning_reg(&rt, fh);
					Scanning_imm_label(&imm, 0, fh);
					break;
		case 0x04: /* beq (branch if equal) */
		case 0x05: /* bne (branch if not equal) */
					Scanning_reg(&rs, fh);
					Scanning_reg(&rt, fh);
					Scanning_imm_label(&imm, 1, fh);
					break;
	}
	Enter_I_instr(opcode, rs, rt, imm, TempStr);
}

int FindMaxLength(){
	int i, maxindex = -1;
	for (i = 0 ; i < MAX_SIZE ; i++){
		if(LabelStruct[i].line != 0 && 
	(maxindex < 0 || LabelStruct[i].length > LabelStruct[maxindex].length))
			maxindex = i;
	}
	return LabelStruct[maxindex].length;
}

void WriteSymbolFile(char *name){
	int i = 0, max, cmp;
	strcat(name, ".sym");
	FILE *fhsym = fopen(name, "w");
	name[strlen(name)-4] = '\0';
	max = FindMaxLength();
	cmp = (max < 12) ? (12 + 5) : (max + 5);
	//do this in case label is too long
	fprintf(fhsym, 
			   "// Symbol table for \"%s.S\"\n"
		       "// Scope level 0:\n"
		       "//      %-*s  Page Address (decimal)\n"
		       "//      ", name, cmp, "Symbol table");
	for (i = 0 ; i < cmp ; i++){
		fprintf(fhsym,"-");
	}
	fprintf(fhsym, "  ----------------------\n");
	for (i = 0 ; i < MAX_SIZE ; i++){
		if(LabelStruct[i].line != 0)
			fprintf(fhsym,"//      %-*s  %04hi\n", cmp, LabelStruct[i].name, (LabelStruct[i].address-2)*4 + Progstart);
	}
	fclose(fhsym);
}

int FirstPass(FILE *fh, char *name){
	int i;
	CleanArrays();
	while (fgets(Line_buffer, MAX_LEN - 1, fh) != NULL){
		CmpStruct = initStruct;
		linenum++;
		ClearAllArg();
		//check if fgets too many words
		if(Line_buffer[strlen(Line_buffer)] != '\0') ErrorMsg_halt(3);
		while(sscanf(Line_buffer + argsave, "%s%n", Word_buffer, &argoffset) == 1){
			//printf("line: %d %s\n", linen, Word_buffer);
			argoffset += argsave;
			// comment, skip this line
			strcpy(Word_buffer_ORIGIN, Word_buffer);
			LowerCase(Word_buffer);
			SkipSpaceAndComma(&argoffset, fh);
			if (Word_buffer[0] == '#' || Word_buffer[0] == '\n' || Word_buffer[0] == '\0') break;
			/* scan */
			for (i = 0; i < R_num; i++){
				if (!strcmp(opcode_R[i], Word_buffer)){CmpStruct.op_r = i;}
			}
			for (i = 0; i < I_num; i++){ 
				if (!strcmp(opcode_I[i], Word_buffer)){CmpStruct.op_i = i;}
			}
			for (i = 0; i < J_num; i++){ 
				if (!strcmp(opcode_J[i], Word_buffer)){CmpStruct.op_j = i;}
			}
			/* tell */
			if (ORGflag == 0){ // not set origin PC value yet
				if (!strcmp(Word_buffer, ".org")){
					 if(sscanf(Line_buffer + argoffset, "%X%n", &PC, &arg1) == 1){
					 	Progstart = PC;
					 	EnterInstrOrData(PC, NULL);
					 	ORGflag = 1;
					 	argoffset += arg1;
						argsave = argoffset;
					 	continue;
					 }
					else {
					 	sscanf(Line_buffer + argoffset, "%s%n", ErrorStr, &arg1);
						ErrorMsg_str(1, ErrorStr);
					}
				}
				else {
					Progstart = PC = 0;
					EnterInstrOrData(PC, NULL);
					ErrorMsg_str(6, Word_buffer_ORIGIN);
					ORGflag = 1;
				}
				//continue;
			}
			//else{
				//SkipSpaceAndComma(&argoffset);
				if (M_index > 0 && !strcmp(Word_buffer, ".org")){
					ErrorMsg_halt(2);
					break;
				}
				else if (!strcmp(Word_buffer, "halt")){
					EnterInstrOrData(0xffffffff, NULL);
				}
				else if (CmpStruct.op_i != -1) I_type(CmpStruct.op_i, fh);
				else if (CmpStruct.op_r != -1) R_type(CmpStruct.op_r, fh);
				else if (CmpStruct.op_j != -1) J_type(CmpStruct.op_j, fh);
				else if (strcmp(Word_buffer, ".org")){ // label
					if (Label_index < MAX_SIZE){
						if (Word_buffer_ORIGIN[strlen(Word_buffer_ORIGIN) - 1] == ':')
						Word_buffer_ORIGIN[strlen(Word_buffer_ORIGIN) - 1] = '\0';
						strcpy(LabelStruct[Label_index].name, Word_buffer_ORIGIN);
						LabelStruct[Label_index].address = M_index;
						LabelStruct[Label_index].line = linenum;
						LabelStruct[Label_index].length =  strlen(Word_buffer_ORIGIN);
						for (i = 0; i < Label_index; i++){
							if (!strcmp(LabelStruct[Label_index].name,LabelStruct[i].name))
								ErrorMsg_label(1, LabelStruct[Label_index].name, LabelStruct[i].line);
						}
						Label_index++;
					}
					else{ // over MAX
						ErrorMsg_halt(4);
					}
					// ErrorMsg_str(2, Word_buffer_ORIGIN);
				}
			//}
			SkipSpaceAndComma(&argoffset, fh);
			//printf("%d: %s %c\n",linenum,Word_buffer_ORIGIN,Line_buffer[argoffset]);
			if (Line_buffer[argoffset] == '\n')
				break;
			argsave = argoffset;
		}
	}
	if (!Pass1_Error_amount)
		WriteSymbolFile(name);
	return Pass1_Error_amount;
}


/* Second Pass -- not yet 
 * need: 
 *	1. Symbol table
 *  2. Check undefined label
 *  3. Check redefined label
 *  4. Check label's range
 * and so forth.
 */
int SecondPass(FILE *fh, char* endian){
	int i = 0, j = 0;
	for(i = 0; i < MAX_SIZE ; i++){
	  if(labelArray[i][0] != '\0'){
		for(j = 0; LabelStruct[j].line != 0; j++){
			if(!strcmp(labelArray[i], LabelStruct[j].name)){
				if(JF[i] == 1)
						mem[i] = (mem[i] & 0xfc000000) | (((4*LabelStruct[j].address + Progstart) >> 2) - 2); 
				else
						mem[i] = (mem[i] & 0xffff0000) | (LabelStruct[j].address - (i + 1));
				break;			
			}
		}
		if (LabelStruct[j].line == 0)
			ErrorMsg_2(1, labelArray[i], i);
		}
	}
	if (!Pass2_Error_amount)
		WriteObjectFile(endian);
	return Pass2_Error_amount;
}

int main(int argc, char **argv){
	int pass1_rti, pass2_rti;
	char *endian;
	FILE *fh_S = fopen(argv[1],"r");
	if (argc < 2){
		fprintf(stderr,"usage: %s file.S [optional: endian]\n", argv[0]);
		exit(1);
	}
	if (strcmp(argv[1] + strlen(argv[1]) - 2,".S")){
		fprintf(stderr,"file name must end with .S\n");
		exit(2);
	}
	if (fh_S == NULL){
		fprintf(stderr,"file doesn't exist\n");
		exit(3);
	}
	if (argc != 3) endian = "l";
	else if (strcmp(argv[2], "l") && strcmp(argv[2], "b")){
		fprintf(stderr,"usage: %s file.S [endian:l/b]\n", argv[0]);
		exit(1);
	}
	else endian = argv[2];
	printf("Assembling %s\n", argv[1]);
	//pass1
	puts("Starting Pass 1...");
	argv[1][strlen(argv[1])-2] = '\0';
	pass1_rti = FirstPass(fh_S, argv[1]);
	if (pass1_rti){
		fprintf(stderr,"Pass 1 - %d error(s)\n", pass1_rti);
		return 1;
	}
	else
		puts("Pass 1 - 0 error(s)");
	//pass2
	puts("Starting Pass 2...");
	pass2_rti = SecondPass(fh_S, endian);
	if (pass2_rti){
		fprintf(stderr,"Pass 2 - %d error(s)\n", pass2_rti);
		return 1;
	}
	else
		puts("Pass 2 - 0 error(s)");
	fclose(fh_S);
	puts("Successfully Assembled!");
	return 0;
}	
