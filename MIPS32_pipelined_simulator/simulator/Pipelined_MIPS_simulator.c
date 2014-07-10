/* Architecture Project 2
 * Team number: Archi01
 * Leader: 101021120, Y.Xiang Zheng (Slighten), student01
 * Member: x1022108, S.Jie Cao, student06
 * First submission Due: 2014-05-10 23:30
 * Second submission Due: 2014-05-17 23:30
 * Filename: Pipelined_MIPS_simulator.c
 * Status: 
 *   Compile, simulate, execute successfully and impeccably. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Pipelined_MIPS_simulator.h"
#define MAXSIZE 1023
#define R_num 12
#define J_num 2
#define I_num 16

static size_t fileSize_d, fileSize_i;
static int *mem_code_d, *mem_code_i;
//static int progSize_d, progSize_i;
static int mem_data_d[256], mem_data_i[256], R[32], R_buf[32], PC = 0, word_num_i = 0, word_num_d = 0;
static int MSBrs = 0, MSBrt = 0, HaltFlag = 0, saved_Rrt = 0;
static int PChalt = 0,flushflag = 0, stallflag = 0, PCsaved = 0, fwdflag_rs = 0, fwdflag_rt = 0, fwdrs = 0, fwdrt = 0; 
static int EXfwdrs = 0, EXfwdrt = 0;
static struct{
	char *where;
	int flag;
}fwd_EX_rs = {NULL, 0}, fwd_EX_rt = {NULL, 0}, init_fwd = {NULL, 0};
static unsigned int CycleCount = 0;
static char *opcode_R[R_num] = {
		"ADD", "SUB", "AND", "OR", "XOR", "NOR", "NAND", "SLT",
		"SLL", "SRL", "SRA", "JR"
};
static int mappingTable_R[R_num] = {
		0x20, 0x22, 0x24, 0x25, 0x26, 0x27, 0x28, 0x2A,
		0x00, 0x02, 0x03, 0x08
};
static char *opcode_I[I_num] = {
		"ADDI", "LW", "LH", "LHU", "LB", "LBU", "SW", "SH",
		"SB", "LUI", "ANDI", "ORI", "NORI", "SLTI", "BEQ", "BNE"
};
static int mappingTable_I[I_num] = {
		0x08, 0x23, 0x21, 0x25, 0x20, 0x24, 0x2B, 0x29,
		0x28, 0x0F, 0x0C, 0x0D, 0x0E, 0x0A, 0x04, 0x05
};
static char *opcode_J[J_num] = { "J", "JAL" };
static int mappingTable_J[J_num] = {
		0x02, 0x03
};

typedef struct INSTRUCTION{
	int IR;
	int type; // 1: R, 2: I, 3: J
	char *name;
	int opcode;
	int rd;
	int rs;
	int rt;
	int shamt;
	int funct;
	int imm;
	int address;
}INSTR;

static struct BUFFER_BETWEEN_PIPE{
	INSTR instr;
	int ALUout;
}init = {{0,0,"NOP",0,0,0,0,0,0,0,0},0}, IFIDin, IFIDout = {{0,0,"NOP"},0}, IDEXin, IDEXout = {{0,0,"NOP"},0}, 
EXDMin, EXDMout = {{0,0,"NOP"},0}, DMWBin, DMWBout = {{0,0,"NOP"},0}, WB = {{0,0,"NOP"},0};

int main(void){
	FILE *fh_s = fopen("snapshot.rpt","w");
	FILE *fh_e = fopen("error_dump.rpt","w");
	ReadFile();
	CleanArrays(); /* clean array to all zero */
	distribute(); /* load data to memory */
	while(1){
		initialize();
		if (PC/4 > 255 || PC/4 < 0) { ErrorMsg(3); PChalt = 1; }  /* Address overflow */
		if (PC % 4 != 0) { ErrorMsg(4); PChalt = 1; } /* Misalignment Error */
		if (PChalt == 1 || HaltFlag == 1) exit(EXIT_FAILURE); 
		WriteBack();
		DataMemoryAccess();
		Execute();
		InstrDecode();
		InstrFetch();
		PrintCycle(fh_s);
		if (DMWBout.instr.opcode == 0x3f) break;
		Synchronize();
		if (flushflag == 1) PC = PCsaved;
		else if (stallflag == 1) PC = PC; 
		else PC += 4;
		/* Start IF */
		CycleCount++;
	}
	fclose(fh_e);
	fclose(fh_s);
	return 0;
}

void Synchronize(){
	int i;
	if (flushflag == 1) IFIDin = init;
	if (stallflag == 0){
		IFIDout = IFIDin;
	}
	else IDEXin = init;
	IDEXout = IDEXin;
	EXDMout = EXDMin;
	DMWBout = DMWBin;
	IFIDin = IDEXin = EXDMin = DMWBin = init;
	for (i = 0 ; i < 32 ; i++) R_buf[i] =  R[i];
}

void InstrFetch(){
	IFIDin.instr.IR = mem_data_i[PC/4];
}

void DataMemoryAccess(){
	DMWBin = EXDMout;
	if (DMWBin.instr.opcode != 0x23 && DMWBin.instr.opcode != 0x21 && DMWBin.instr.opcode != 0x25 &&
		DMWBin.instr.opcode != 0x20 && DMWBin.instr.opcode != 0x24 && DMWBin.instr.opcode != 0x2B && 
		DMWBin.instr.opcode != 0x29 && DMWBin.instr.opcode != 0x28) return;
	switch (DMWBin.instr.opcode){
		case 0x23: /* lw (load word) */
					/* Address overflow */
					if (DMWBin.ALUout < 0 || DMWBin.ALUout > MAXSIZE) ErrorMsg(3);
					/* Misalignment error */
					if (DMWBin.ALUout % 4 != 0) ErrorMsg(4);
					if (HaltFlag == 1) break;
					//printf("%X %d\n", PC,mem_data_d[DMWBin.ALUout/4]);
					DMWBin.ALUout = mem_data_d[DMWBin.ALUout/4];
					break;
        case 0x21: /* lh (load halfword) */
					if (DMWBin.ALUout < 0 || DMWBin.ALUout > MAXSIZE) ErrorMsg(3);
					/* Misalignment error */
					if (DMWBin.ALUout % 2 != 0) ErrorMsg(4);
					if (HaltFlag == 1) break;
					DMWBin.ALUout = (mem_data_d[DMWBin.ALUout/4] << ((2 - finite_groupZ4(DMWBin.ALUout)) * 8)) >> 16; /* sign extension */
					break;
        case 0x25: /* lhu (load halfword unsigned) */
					if (DMWBin.ALUout < 0 || DMWBin.ALUout > MAXSIZE) ErrorMsg(3);
					/* Misalignment error */
					if (DMWBin.ALUout % 2 != 0) ErrorMsg(4);
					if (HaltFlag == 1) break;
					DMWBin.ALUout = (mem_data_d[DMWBin.ALUout/4] >> (finite_groupZ4(DMWBin.ALUout) * 8)) & 0xffff;
					break;
        case 0x20: /* lb (load one byte) */
					if (DMWBin.ALUout < 0 || DMWBin.ALUout > MAXSIZE) ErrorMsg(3);
					if (HaltFlag == 1) break;
					DMWBin.ALUout = (mem_data_d[DMWBin.ALUout/4] << ((3 - finite_groupZ4(DMWBin.ALUout)) * 8)) >> 24; /* sign extension */
					break;
		case 0x24: /* lbu (load one byte unsigned) */
					if (DMWBin.ALUout < 0 || DMWBin.ALUout > MAXSIZE) ErrorMsg(3);
					if (HaltFlag == 1) break;
					DMWBin.ALUout = (mem_data_d[DMWBin.ALUout/4] >> (finite_groupZ4(DMWBin.ALUout) * 8)) & 0xff;
					break;
		case 0x2B: /* sw (store word) */
					if (DMWBin.ALUout < 0 || DMWBin.ALUout > MAXSIZE) ErrorMsg(3);
					/* Misalignment error */
					if (DMWBin.ALUout % 4 != 0) ErrorMsg(4);
					if (HaltFlag == 1) break;
					mem_data_d[DMWBin.ALUout/4] = R[DMWBin.instr.rt];
					break;
		case 0x29: /* sh (store halfword) */
					if (DMWBin.ALUout < 0 || DMWBin.ALUout > MAXSIZE) ErrorMsg(3);
					/* Misalignment error */
					if (DMWBin.ALUout % 2 != 0) ErrorMsg(4);
					if (HaltFlag == 1)  break;
					mem_data_d[DMWBin.ALUout/4] &= ~(0xffff << (finite_groupZ4(DMWBin.ALUout) * 8)); /* clear */
					mem_data_d[DMWBin.ALUout/4] |= (R[DMWBin.instr.rt] & 0xffff) << (finite_groupZ4(DMWBin.ALUout) * 8);
					break;
		case 0x28: /* sb (store one byte) */
					if (DMWBin.ALUout < 0 || DMWBin.ALUout > MAXSIZE) ErrorMsg(3);
					if (HaltFlag == 1) break;
					mem_data_d[DMWBin.ALUout/4] &= ~(0xff << (finite_groupZ4(DMWBin.ALUout) * 8)); /* clear */
					mem_data_d[DMWBin.ALUout/4] |= (R[DMWBin.instr.rt] & 0xff) << (finite_groupZ4(DMWBin.ALUout) * 8);
					break;
	}
}

void Execute(){
	EXDMin = IDEXout;
	if (!strcmp(EXDMin.instr.name, "JR")  || EXDMin.instr.opcode == 0x02 || EXDMin.instr.opcode == 0x03 ||
		EXDMin.instr.opcode == 0x04 || EXDMin.instr.opcode == 0x05 || EXDMin.instr.IR == 0) return;
	switch (EXDMin.instr.type){
		case 1:
				/* fwdtest */
				{

					switch (DMWBout.instr.type){
						case 1:
							if (DMWBout.instr.rd != 0 && EXDMin.instr.rs == DMWBout.instr.rd){
									fwd_EX_rs.where = "DM-WB";
									fwd_EX_rs.flag = 1;
									EXfwdrs = DMWBout.ALUout;
								}
							if (DMWBout.instr.rd != 0 && EXDMin.instr.rt == DMWBout.instr.rd){
									fwd_EX_rt.where = "DM-WB";
									fwd_EX_rt.flag = 1;
									EXfwdrt = DMWBout.ALUout;
							}
							break;
						case 2:
							if(DMWBout.instr.opcode != 0x2B && DMWBout.instr.opcode != 0x29 && 
								DMWBout.instr.opcode != 0x28 && DMWBout.instr.opcode != 0x04 &&
								DMWBout.instr.opcode != 0x05){
											if (DMWBout.instr.rt != 0 && EXDMin.instr.rs == DMWBout.instr.rt){
												fwd_EX_rs.where = "DM-WB";
												fwd_EX_rs.flag = 1;
												EXfwdrs = DMWBout.ALUout;
											}
											if (DMWBout.instr.rt != 0 && EXDMin.instr.rt == DMWBout.instr.rt){
												fwd_EX_rt.where = "DM-WB";
												fwd_EX_rt.flag = 1;
												EXfwdrt = DMWBout.ALUout;
											}
							}
							break;
						case 3:
								if(!strcmp(DMWBout.instr.name, "JAL")){
										if (EXDMin.instr.rs == 31){
											fwd_EX_rs.where = "DM-WB";
												fwd_EX_rs.flag = 1;
												EXfwdrs = DMWBout.ALUout;
										}
										if (EXDMin.instr.rt == 31){
												fwd_EX_rt.where = "DM-WB";
													fwd_EX_rt.flag = 1;
													EXfwdrt = DMWBout.ALUout;
											}
								}
							break;
					}
					
					switch (EXDMout.instr.type){
						case 1:
							if (EXDMout.instr.rd != 0 && EXDMin.instr.rs == EXDMout.instr.rd){
									fwd_EX_rs.where = "EX-DM";
									fwd_EX_rs.flag = 1;
									EXfwdrs = EXDMout.ALUout;
								}
							if (EXDMout.instr.rd != 0 && EXDMin.instr.rt == EXDMout.instr.rd){
									fwd_EX_rt.where = "EX-DM";
									fwd_EX_rt.flag = 1;
									EXfwdrt = EXDMout.ALUout;
							}
							break;
						case 2:
							if(EXDMout.instr.opcode == 0x08 || EXDMout.instr.opcode == 0x0f ||
								EXDMout.instr.opcode == 0x0c || EXDMout.instr.opcode == 0x0d ||
								EXDMout.instr.opcode == 0x0e || EXDMout.instr.opcode == 0x0a){
											if (EXDMout.instr.rt != 0 && EXDMin.instr.rs == EXDMout.instr.rt ){
												fwd_EX_rs.where = "EX-DM";
												fwd_EX_rs.flag = 1;
												EXfwdrs = EXDMout.ALUout;
											}
											if (EXDMout.instr.rt != 0 && EXDMin.instr.rt == EXDMout.instr.rt){
												fwd_EX_rt.where = "EX-DM";
												fwd_EX_rt.flag = 1;
												EXfwdrt = EXDMout.ALUout;
											}
							}
							break;
						/*case 3:
								if(!strcmp(EXDMout.instr.name, "JAL")){
										if (EXDMin.instr.rs == 31){
												fwdflag_rs = 1;
												fwdrs = EXDMout.ALUout;
										}
										else if (EXDMin.instr.rt == 31){
													fwdflag_rt = 1;
													fwdrt = EXDMout.ALUout;
											}
								}
								break;*/
				}

					
				}
				{
					if (fwd_EX_rs.flag != 1) EXfwdrs = R[EXDMin.instr.rs];
					if (fwd_EX_rt.flag != 1) EXfwdrt = R[EXDMin.instr.rt];
					switch (EXDMin.instr.funct){
						case 0x20: /* add */ 
									/* save MSB */
									MSBrs = EXfwdrs >> 31 & 1; 
									MSBrt = EXfwdrt >> 31 & 1;
									EXDMin.ALUout = EXfwdrs + EXfwdrt;
									/* Number overflow (P+P=N or N+N=P) */
									if ( MSBrs == MSBrt && (EXDMin.ALUout >> 31 & 1) != MSBrs )
										ErrorMsg(2); 
									break;
						case 0x22: /* sub */
									/* save MSB */
									MSBrs = EXfwdrs >> 31 & 1;
									MSBrt = EXfwdrt >> 31 & 1;
									saved_Rrt = EXfwdrt;
									/* -0x80000000 doesn't exist */
									if (EXfwdrt == 0x80000000)
										EXDMin.ALUout = EXfwdrs + EXfwdrt;
									else
										EXDMin.ALUout = EXfwdrs - EXfwdrt;
									/* Number overflow (P-N=N or N-P=P) or -0x80000000 doesn't exist*/
									if ( (MSBrs != MSBrt && (EXDMin.ALUout >> 31 & 1) == MSBrt) || saved_Rrt == 0x80000000) ErrorMsg(2); 
									break;
						case 0x24: /* and */
									EXDMin.ALUout = EXfwdrs & EXfwdrt;
									break;
						case 0x25: /* or */
									EXDMin.ALUout = EXfwdrs | EXfwdrt;
									break;
						case 0x26: /* xor */
									EXDMin.ALUout = EXfwdrs ^ EXfwdrt;
									break;
						case 0x27: /* nor */
									EXDMin.ALUout = ~(EXfwdrs | EXfwdrt);
									break;
						case 0x28: /* nand */
									EXDMin.ALUout = ~(EXfwdrs & EXfwdrt);
									break;
						case 0x2A: /* slt (set if less than) */
									EXDMin.ALUout = EXfwdrs < EXfwdrt;
									break;
						case 0x00: /* sll (shift left logic) */
									EXDMin.ALUout = EXfwdrt << EXDMin.instr.shamt;
									break;
						case 0x02: /* srl (shift right logic) */
				   					/* ex: 0xffff1234 >> 8 = 0x00ffff12
				   						   0x00001234 >> 8 = 0x00000012 */
				   					/* for needing 32 - shamt  < 32 */ 
									if (EXDMin.instr.shamt == 0)	EXDMin.ALUout = EXfwdrt;
									else // 0 <= shamt < 32	  	
									EXDMin.ALUout = (EXfwdrt >> EXDMin.instr.shamt) & ~(0xffffffff << (32 - EXDMin.instr.shamt));
									break;
						case 0x03: /* sra (shift right arithmetic) */
				 					/* ex: 0xffff1234 >> 8 = 0xffffff12
				   		 			       0x00001234 >> 8 = 0x00000012 */
				   					EXDMin.ALUout = EXfwdrt >> EXDMin.instr.shamt;
				   					break;	
					}
				}
				break;
		case 2:
				/* fwdtest */
					switch (DMWBout.instr.type){
						case 1:
							if (DMWBout.instr.rd != 0 && EXDMin.instr.rs == DMWBout.instr.rd){
									fwd_EX_rs.where = "DM-WB";
									fwd_EX_rs.flag = 1;
									EXfwdrs = DMWBout.ALUout;
								}
							if (EXDMin.instr.opcode == 0x2B || EXDMin.instr.opcode == 0x29 ||
								EXDMin.instr.opcode == 0x28){
								if (DMWBout.instr.rd != 0 && EXDMin.instr.rt == DMWBout.instr.rd){
									fwd_EX_rt.where = "DM-WB";
									fwd_EX_rt.flag = 1;
									EXfwdrt = DMWBout.ALUout;
								}
							}
							break;
						case 2:
							if(DMWBout.instr.opcode != 0x2B && DMWBout.instr.opcode != 0x29 && 
								DMWBout.instr.opcode != 0x28 && DMWBout.instr.opcode != 0x04 &&
								DMWBout.instr.opcode != 0x05){
											if ( DMWBout.instr.rt != 0 && (EXDMin.instr.rs == DMWBout.instr.rt)){
												fwd_EX_rs.where = "DM-WB";
												fwd_EX_rs.flag = 1;
												EXfwdrs = DMWBout.ALUout;
											}
							if (EXDMin.instr.opcode == 0x2B || EXDMin.instr.opcode == 0x29 ||
								EXDMin.instr.opcode == 0x28){
								if (DMWBout.instr.rt != 0 && EXDMin.instr.rt == DMWBout.instr.rt){
									fwd_EX_rt.where = "DM-WB";
									fwd_EX_rt.flag = 1;
									EXfwdrt = DMWBout.ALUout;
								}
							}
							}	
							break;
						case 3:
								if(!strcmp(DMWBout.instr.name, "JAL")){
										if (EXDMin.instr.rs == 31){
												fwd_EX_rs.flag = 1;
												fwd_EX_rs.where = "DM-WB";
												EXfwdrs = DMWBout.ALUout;
										}
							if (EXDMin.instr.opcode == 0x2B || EXDMin.instr.opcode == 0x29 ||
								EXDMin.instr.opcode == 0x28){
										if (EXDMin.instr.rt == 31){
													fwd_EX_rt.where = "DM-WB";
													fwd_EX_rt.flag = 1;
													EXfwdrt = DMWBout.ALUout;
											}
										}
								}
							break;
						}
				
					switch (EXDMout.instr.type){
						case 1:
							if (EXDMout.instr.rd != 0 && EXDMin.instr.rs == EXDMout.instr.rd){
									fwd_EX_rs.where = "EX-DM";
									fwd_EX_rs.flag = 1;
									EXfwdrs = EXDMout.ALUout;
								}
							if (EXDMin.instr.opcode == 0x2B || EXDMin.instr.opcode == 0x29 ||
								EXDMin.instr.opcode == 0x28){
								if (EXDMout.instr.rd != 0 && EXDMin.instr.rt == EXDMout.instr.rd){
									fwd_EX_rt.where = "EX-DM";
									fwd_EX_rt.flag = 1;
									EXfwdrt = EXDMout.ALUout;
								}
							}
							break;
						case 2:
							if(EXDMout.instr.opcode == 0x2B || EXDMout.instr.opcode == 0x29 ||
								EXDMout.instr.opcode == 0x28 || EXDMout.instr.opcode == 0x04 ||
								EXDMout.instr.opcode == 0x05) break; // actually branch, load won't happen
							if (EXDMout.instr.rt != 0 && EXDMin.instr.rs == EXDMout.instr.rt){
									fwd_EX_rs.where = "EX-DM";
									fwd_EX_rs.flag = 1;
									EXfwdrs = EXDMout.ALUout;
								}
							if (EXDMin.instr.opcode == 0x2B || EXDMin.instr.opcode == 0x29 ||
								EXDMin.instr.opcode == 0x28){
								if (EXDMout.instr.rt != 0 && EXDMin.instr.rt == EXDMout.instr.rt){
									fwd_EX_rt.where = "EX-DM";
									fwd_EX_rt.flag = 1;
									EXfwdrt = EXDMout.ALUout;
								}
							}
							break;
					}
		
				{
					if (fwd_EX_rs.flag != 1) EXfwdrs = R[EXDMin.instr.rs];
					switch (EXDMin.instr.opcode){
							case 0x08: /* addi (add immediate) */
										/* save MSB */
										MSBrs = EXfwdrs >> 31 & 1; 
										EXDMin.ALUout = EXfwdrs + EXDMin.instr.imm;
										/* Number overflow (P+P=N or N+N=P) */
										if ( MSBrs == (EXDMin.instr.imm >> 31 & 1) && (EXDMin.ALUout >> 31 & 1) != MSBrs )
											ErrorMsg(2);
										break;
							case 0x0F: /* lui (load upper halfword immediate) */
										EXDMin.ALUout = EXDMin.instr.imm << 16;
										break;
							case 0x0C: /* andi (and immediate) */
										EXDMin.ALUout = EXfwdrs & (EXDMin.instr.imm & 0xffff);
										break;
							case 0x0D: /* ori (or immediate)  */
										EXDMin.ALUout = EXfwdrs | (EXDMin.instr.imm & 0xffff);
										break;
							case 0x0E: /* nori (nor immediate) */
										EXDMin.ALUout = ~(EXfwdrs | (EXDMin.instr.imm & 0xffff));
										break;
							case 0x0A: /* slti (set if less than immediate) */
										EXDMin.ALUout = EXfwdrs < EXDMin.instr.imm;
										break;
							case 0x23: /* lw (load word) */
							case 0x21: /* lh (load halfword) */
					        case 0x25: /* lhu (load halfword unsigned) */
					        case 0x20: /* lb (load one byte) */
							case 0x24: /* lbu (load one byte unsigned) */
							case 0x2B: /* sw (store word) */
							case 0x29: /* sh (store halfword) */
							case 0x28: /* sb (store one byte) */
										MSBrs = EXfwdrs >> 31 & 1;		
										EXDMin.ALUout = EXfwdrs + EXDMin.instr.imm;
										/* Number overflow (P+P=N or N+N=P) */
										if ( MSBrs == (EXDMin.instr.imm >> 31 & 1) && (EXDMin.ALUout >> 31 & 1) != MSBrs ) ErrorMsg(2);
										break;
					}
				}
				break;
	}
}

void InstrDecode(){
	int i = 0;
	IDEXin = IFIDout;
	if (IDEXin.instr.IR == 0) {IDEXin.instr.name = "NOP"; return;}
	IDEXin.instr.opcode = (IDEXin.instr.IR >> 26) & 0x3f;
	if (IDEXin.instr.opcode == 0x3f) {IDEXin.instr.name = "HALT"; return;}
	/* distinguish type */
	if (IDEXin.instr.opcode == 0x00) IDEXin.instr.type = 1;
	else if (IDEXin.instr.opcode == 0x02 || IDEXin.instr.opcode == 0x03) IDEXin.instr.type = 3;
	else IDEXin.instr.type = 2;
	/* type decode */
			switch (IDEXin.instr.type){
				case 1: // R-type				
							IDEXin.instr.funct = IDEXin.instr.IR & 0x3f;
							if(IDEXin.instr.funct != 0x00 && IDEXin.instr.funct != 0x03 && 
								IDEXin.instr.funct != 0x02 )
								IDEXin.instr.rs = (IDEXin.instr.IR >> 21) & 0x1f;
							if (IDEXin.instr.funct != 0x08){/*jr*/
								IDEXin.instr.rt = (IDEXin.instr.IR >> 16) & 0x1f;
								IDEXin.instr.rd = (IDEXin.instr.IR >> 11) & 0x1f;
							}
							if(IDEXin.instr.funct == 0x00 || IDEXin.instr.funct == 0x03 ||
								IDEXin.instr.funct == 0x02 )
								IDEXin.instr.shamt = (IDEXin.instr.IR >> 6) & 0x1f;

							for (i = 0; i < R_num; i++){
								if (IDEXin.instr.funct == mappingTable_R[i]){IDEXin.instr.name = opcode_R[i];}
							}
							if (IDEXin.instr.name == NULL){
								fprintf(stderr, "Illegal testcase: Unknown R-type Instruction in cycle: %d\n", CycleCount);
								exit(EXIT_FAILURE);
							}
							
							/* stall test */
							switch (IDEXout.instr.type){
									/*case 1:
											if (IDEXout.instr.rd != 0 && (IDEXin.instr.rs == IDEXout.instr.rd || 
												IDEXin.instr.rt == IDEXout.instr.rd )){
												stallflag = 1;
												return;
											}
											break;*/
									case 2:
											/* load */
											if (IDEXout.instr.rt != 0 && (IDEXin.instr.rs == IDEXout.instr.rt || 
												IDEXin.instr.rt == IDEXout.instr.rt) && (IDEXout.instr.opcode == 0x23 || IDEXout.instr.opcode == 0x21 || 
												IDEXout.instr.opcode == 0x25 || IDEXout.instr.opcode == 0x20 ||
												IDEXout.instr.opcode == 0x24)){
												stallflag = 1;
												return;
											}
											break;
							}
							if (!strcmp(IDEXin.instr.name, "JR")){
								/* stall test */
								switch (IDEXout.instr.type){
									case 1:
											if (IDEXout.instr.rd != 0 && (IDEXin.instr.rs == IDEXout.instr.rd || 
												IDEXin.instr.rt == IDEXout.instr.rd )){
												stallflag = 1;
												return;
											}
											break;
									case 2:
											if (IDEXout.instr.rt != 0 && (IDEXin.instr.rs == IDEXout.instr.rt) && 
												(IDEXout.instr.opcode != 0x2B && IDEXout.instr.opcode != 0x29 && 
												IDEXout.instr.opcode != 0x28 && IDEXout.instr.opcode != 0x04 &&
												IDEXout.instr.opcode != 0x05)){
												stallflag = 1;
												return;
											}
											break;
								}
								/* fwd test */
								switch (EXDMout.instr.type){
										case 1:
												//printf("%d\n", EXDMout.instr.rd);
											if (EXDMout.instr.rd != 0 && IDEXin.instr.rs == EXDMout.instr.rd){
												fwdflag_rs = 1;
												fwdrs = EXDMout.ALUout;
											}
											break;
										case 2:
											if(EXDMout.instr.opcode == 0x23 || EXDMout.instr.opcode == 0x21 || 
												EXDMout.instr.opcode == 0x25 || EXDMout.instr.opcode == 0x20 ||
												EXDMout.instr.opcode == 0x24){
												if (EXDMout.instr.rt != 0 && (IDEXin.instr.rs == EXDMout.instr.rt)){
													stallflag = 1;
													return;
												}
											}
											else if(EXDMout.instr.opcode != 0x2B && EXDMout.instr.opcode != 0x29 && 
												EXDMout.instr.opcode != 0x28 && EXDMout.instr.opcode != 0x04 &&
												EXDMout.instr.opcode != 0x05){
												if (EXDMout.instr.rt != 0 && IDEXin.instr.rs == EXDMout.instr.rt){
													fwdflag_rs = 1;
													fwdrs = EXDMout.ALUout;
												}
											}
											break;
										case 3:
											if(!strcmp(EXDMout.instr.name, "JAL")){
												if (IDEXin.instr.rs == 31){
													fwdflag_rs = 1;
													fwdrs = EXDMout.ALUout;
												}
											}
											break;
									}
								if (fwdflag_rs == 0) fwdrs = R[IDEXin.instr.rs];
								PCsaved = fwdrs;
								flushflag = 1;
								return;
							}
						break;
				case 2: // I-type
							if(IDEXin.instr.opcode != 0x0f) /* lui */
								IDEXin.instr.rs = (IDEXin.instr.IR >> 21) & 0x1f;
							IDEXin.instr.rt = (IDEXin.instr.IR >> 16) & 0x1f;
							IDEXin.instr.imm = ((IDEXin.instr.IR & 0xffff) << 16) >> 16; /* sign extension */
							for (i = 0; i < I_num; i++){ 
								if (IDEXin.instr.opcode == mappingTable_I[i]){IDEXin.instr.name = opcode_I[i];}
							}
							if (IDEXin.instr.name == NULL){
								fprintf(stderr, "Illegal testcase: Unknown I-type Instruction in cycle: %d\n", CycleCount);
								exit(EXIT_FAILURE);
							}
							/* stall & fwd test */
									switch (IDEXout.instr.type){
									/*case 1:
											if (IDEXout.instr.rd != 0 && IDEXin.instr.rs == IDEXout.instr.rd){
												stallflag = 1;
												return;
											}
											break;*/
									case 2:
											if(IDEXout.instr.opcode == 0x23 || IDEXout.instr.opcode == 0x21 || 
												IDEXout.instr.opcode == 0x25 || IDEXout.instr.opcode == 0x20 ||
												IDEXout.instr.opcode == 0x24){
												if (IDEXout.instr.rt != 0 && IDEXin.instr.rs == IDEXout.instr.rt){
													stallflag = 1;
													return;
												}
												/*else if ((IDEXin.instr.opcode == 0x2B || IDEXin.instr.opcode == 0x29 || 
												IDEXin.instr.opcode == 0x28) && IDEXout.instr.rt != 0 && IDEXin.instr.rt == IDEXout.instr.rt){
													stallflag = 1;
													return;
												}*/
												if (IDEXin.instr.opcode == 0x2B || IDEXin.instr.opcode == 0x29 ||
													IDEXin.instr.opcode == 0x28){
													if (IDEXout.instr.rt != 0 && IDEXin.instr.rt == IDEXout.instr.rt){
														stallflag = 1;
														return;
													}
												}
											}
											break;
									}
							if (!strcmp(IDEXin.instr.name, "BEQ") || !strcmp(IDEXin.instr.name, "BNE")){
									switch (IDEXout.instr.type){
									case 1:
											if (IDEXout.instr.rd != 0 && (IDEXin.instr.rt == IDEXout.instr.rd || IDEXin.instr.rs == IDEXout.instr.rd)){
												stallflag = 1;
												return;
											}
											break;
									case 2:
											if(IDEXout.instr.opcode != 0x2B && IDEXout.instr.opcode != 0x29 && 
												IDEXout.instr.opcode != 0x28 && IDEXout.instr.opcode != 0x04 &&
												IDEXout.instr.opcode != 0x05){
												if (IDEXout.instr.rt != 0 && IDEXin.instr.rt == IDEXout.instr.rt){
													stallflag = 1;
													return;
												}
												if (IDEXout.instr.rt != 0 && IDEXin.instr.rs == IDEXout.instr.rt){
													stallflag = 1;
													return;
												}
											}
											break;
									}
									switch (EXDMout.instr.type){
										case 1:
												//printf("%d\n", EXDMout.instr.rd);
											if (EXDMout.instr.rd != 0 && IDEXin.instr.rs == EXDMout.instr.rd){
												
												fwdflag_rs = 1;
												fwdrs = EXDMout.ALUout;
											}
											if (EXDMout.instr.rd != 0 && IDEXin.instr.rt == EXDMout.instr.rd){
												
												fwdflag_rt = 1;
												fwdrt = EXDMout.ALUout;
											}
											break;
										case 2:
											if(EXDMout.instr.opcode == 0x23 || EXDMout.instr.opcode == 0x21 || 
												EXDMout.instr.opcode == 0x25 || EXDMout.instr.opcode == 0x20 ||
												EXDMout.instr.opcode == 0x24){
												if (EXDMout.instr.rt != 0 && (IDEXin.instr.rs == EXDMout.instr.rt || 
													IDEXin.instr.rt == EXDMout.instr.rt)){
													stallflag = 1;
													return;
												}
											}
											else if(EXDMout.instr.opcode != 0x2B && EXDMout.instr.opcode != 0x29 && 
												EXDMout.instr.opcode != 0x28 && EXDMout.instr.opcode != 0x04 &&
												EXDMout.instr.opcode != 0x05){
												if (EXDMout.instr.rt != 0 && IDEXin.instr.rs == EXDMout.instr.rt){
													fwdflag_rs = 1;
													fwdrs = EXDMout.ALUout;
												}
												if (EXDMout.instr.rt != 0 && IDEXin.instr.rt == EXDMout.instr.rt){
													fwdflag_rt = 1;
													fwdrt = EXDMout.ALUout;
												}
											}
											break;
										case 3:
											if(!strcmp(EXDMout.instr.name, "JAL")){
												if (IDEXin.instr.rs == 31){
													fwdflag_rs = 1;
													fwdrs = EXDMout.ALUout;
												}
												if (IDEXin.instr.rt == 31){
													fwdflag_rt = 1;
													fwdrt = EXDMout.ALUout;
												}
											}
											break;
									}
								}
								switch(IDEXin.instr.opcode){
									case 0x04: /* BEQ (branch if equal) */
										if (fwdflag_rs != 1) fwdrs = R[IDEXin.instr.rs];
										if (fwdflag_rt != 1) fwdrt = R[IDEXin.instr.rt];
										if (fwdrs == fwdrt){
											PCsaved = PC + 4*IDEXin.instr.imm;
											flushflag = 1;
										}
										break;
									case 0x05: /* BNE (branch if not equal) */
										if (fwdflag_rs != 1) fwdrs = R[IDEXin.instr.rs];
										if (fwdflag_rt != 1) fwdrt = R[IDEXin.instr.rt];
										if (fwdrs != fwdrt){
											PCsaved = PC + 4*IDEXin.instr.imm;
											flushflag = 1;
										}
										break;
								}
						break;
				case 3: // J-type
						{
							for (i = 0; i < J_num; i++){ 
								if (IDEXin.instr.opcode == mappingTable_J[i]){IDEXin.instr.name = opcode_J[i];}
							}
							IDEXin.instr.address = IDEXin.instr.IR & 0x03ffffff;
							PCsaved = ((PC + 4) & 0xf0000000) | 4*IDEXin.instr.address;
							if (!strcmp(IDEXin.instr.name, "JAL")){
								IDEXin.instr.rs = 31;
								IDEXin.ALUout = PC;
							}
							flushflag = 1;
						}
						break;
	}
}


void WriteBack(){
	WB = DMWBout;
	switch(WB.instr.type){
		case 1:
			if (WB.instr.funct != 0x08){
				if (WB.instr.rd == 0)	
					ErrorMsg(1);
				else
					R[WB.instr.rd] = WB.ALUout;
			}
			break;
		case 2:
			if (WB.instr.opcode == 0x2B || WB.instr.opcode == 0x29 || 
				WB.instr.opcode == 0x28 || WB.instr.opcode == 0x04 ||
				WB.instr.opcode == 0x05 ) return;
			else{
				if (WB.instr.rt == 0)  ErrorMsg(1); // write to $0
				else R[WB.instr.rt] = WB.ALUout;
			}
			break;
		case 3:
			if (!strcmp(WB.instr.name, "JAL"))
					R[31] = WB.ALUout;
			break;
	}
}
	
void PrintCycle(FILE *fh){
	int i;
	fprintf(fh, "cycle %d\n", CycleCount);
	for (i = 0; i < 32; i++){
		fprintf(fh, "$%02d: 0x%08X\n", i, R_buf[i]);
	}
	fprintf(fh, "PC: 0x%08X\n", PC);

	fprintf(fh, "IF: 0x%08X", IFIDin.instr.IR);
	if (stallflag == 1) fprintf(fh, " to_be_stalled\n");
	else if (flushflag == 1) fprintf(fh, " to_be_flushed\n"); 
	else fprintf(fh, "\n");

	fprintf(fh, "ID: %s", IDEXin.instr.name);
	if (stallflag == 1) fprintf(fh, " to_be_stalled\n");
	else if (fwdflag_rs == 1 || fwdflag_rt == 1) 
		fwdflag_rs == 0 ? fprintf(fh, " fwd_EX-DM_rt_$%d\n", IDEXin.instr.rt) : 
		fwdflag_rt == 0 ? fprintf(fh, " fwd_EX-DM_rs_$%d\n", IDEXin.instr.rs) :
		fprintf(fh, " fwd_EX-DM_rs_$%d fwd_EX-DM_rt_$%d\n", IDEXin.instr.rs, IDEXin.instr.rt);
	else fprintf(fh, "\n");

	fprintf(fh, "EX: %s", EXDMin.instr.name);
	if (fwd_EX_rs.flag == 1 || fwd_EX_rt.flag == 1)
		fwd_EX_rs.flag == 0 ? fprintf(fh, " fwd_%s_rt_$%d\n", fwd_EX_rt.where, EXDMin.instr.rt) : 
		fwd_EX_rt.flag == 0 ? fprintf(fh, " fwd_%s_rs_$%d\n", fwd_EX_rs.where, EXDMin.instr.rs) :
		fprintf(fh, " fwd_%s_rs_$%d fwd_%s_rt_$%d\n", fwd_EX_rs.where, EXDMin.instr.rs, fwd_EX_rt.where, EXDMin.instr.rt);
	else fprintf(fh, "\n");

	fprintf(fh, "DM: %s\n", DMWBin.instr.name);
	fprintf(fh, "WB: %s\n\n\n", WB.instr.name);
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
		R_buf[i] = 0;
	}
}

/* reset variables */
void initialize(void){
	EXfwdrt = EXfwdrs = fwdrs = fwdrt = fwdflag_rs = fwdflag_rt = PCsaved = 
	stallflag = flushflag = saved_Rrt = R[0] = MSBrs = MSBrt = 0;
	fwd_EX_rs = fwd_EX_rt = init_fwd;
}

/* load data to memory */
void distribute(){
	int i, j;
	PC = mem_code_i[0]; /* PC */
	R_buf[29] = R[29] = mem_code_d[0]; /* $sp */
	word_num_i = mem_code_i[1]; /* word amount */
	word_num_d = mem_code_d[1]; /* word amount */
	/*allocate*/
	if (PC + word_num_i > 1024 && word_num_i != 0) {
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

/* for finite_groupZ4(-1) = 3, finite_groupZ4(5) = 1 */
int finite_groupZ4(int x){
	if (x < 0)
		return (x % 4) + 4;
	return x % 4;
}

/* error handler for possible error cases */
void ErrorMsg(int ZipCode){
	FILE *fh_e = fopen("error_dump.rpt","a");
	switch (ZipCode){
		case 1:/* Try to write to $0 */
				fprintf(fh_e, "Write $0 error in cycle: %d\n", CycleCount+1);
				break;
		case 2:/* Number overflow */
				fprintf(fh_e, "Number overflow in cycle: %d\n", CycleCount+1);
				break;
		case 3:/* Address overflow */
				fprintf(fh_e, "Address overflow in cycle: %d\n", CycleCount+1);
				HaltFlag = 1;
				break;
		case 4:/* Misalignment error */
				fprintf(fh_e, "Misalignment error in cycle: %d\n", CycleCount+1);
				HaltFlag = 1;
				break;
		default:
				break;
		}
		fclose(fh_e);
}
