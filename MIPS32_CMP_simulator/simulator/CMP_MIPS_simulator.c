/* Architecture Project 3
 * Team number: Archi01
 * Leader: 101021120, Y.Xiang Zheng (Slighten), student01
 * Member: x1022108, S.Jie Cao, student06
 * First submission Due: 2014-06-25 23:30
 * Second submission Due: 2014-07-02 23:30
 * Filename: CMP_MIPS_simulator.c
 * Status: 
 *   Compile, simulate, execute successfully and impeccably. 
 */

#include <stdio.h>
#include <stdlib.h>
#include "CMP_MIPS_simulator.h"
#define MAXSIZE 1023

static size_t fileSize_d, fileSize_i;
static int *mem_code_d, *mem_code_i;
//static int progSize_d, progSize_i;
static int D_hard_drive[256], I_hard_drive[256], R[32], PC = 0, word_num_i = 0, word_num_d = 0;
static int opcode = 0, rs = 0, rt = 0, rd = 0, shamt = 0, funct = 0, imm = 0, address = 0;
static int MSBrs = 0, MSBrt = 0, HaltFlag = 0, saved_Rrt = 0, ALUzero = 0, ALUout = 0;
static unsigned int CycleCount = 0;
static unsigned int ICache_misses = 0, ICache_hits = 0, DCache_misses = 0, DCache_hits = 0;
static unsigned int ITLB_misses = 0, ITLB_hits = 0, DTLB_misses = 0, DTLB_hits = 0;
static unsigned int IPTE_misses = 0, IPTE_hits = 0, DPTE_misses = 0, DPTE_hits = 0;
static int IPTE_entries = 0, DPTE_entries = 0, ITLB_entries = 0, DTLB_entries = 0;
static int ICACHE_blocks = 0, DCACHE_blocks = 0, I_associativity = 0, D_associativity = 0;
static int IMEM_blocks = 0, DMEM_blocks = 0;
static int I_pagesize = 0, D_pagesize = 0, I_blocksize = 0, D_blocksize = 0;
static struct MEMORY{
	int *data;
	int *valid;
	int *LRUorder;
}D_MEM, I_MEM;

static struct CACHE{
	int *data;
	int *valid;
	int *tag;
	int *LRUorder;
}I_CACHE, D_CACHE;

static struct Page_Table_Entries{
	int *content;
	int *valid;
}I_PTE, D_PTE;

static struct Translation_Lookaside_Buffer{
	int *content;
	int *valid;
	int *tag;
	int *LRUorder;
}I_TLB, D_TLB;

int main(int argc, char **argv){
	int i;
	FILE *fh_s = fopen("snapshot.rpt","w");
	ReadFile();
	CleanArrays(); /* clean array to all zero */
	distribute(); /* load data to memory */
	AllocateMemory(argc ,argv);
	while(1){
		initialize(); /* reset variables */
		// Cycle amount should be less than 500K, and avoid infinite loop.
		if (CycleCount > 500000) {
			fprintf(stderr, "Illegal testcase: Cycle amount over 500K\n");
			exit(EXIT_FAILURE); 
		}
		// start outputting snapshot.rpt (1 cycle)
		fprintf(fh_s, "cycle %u\n", CycleCount);
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
		decode(I_hard_drive[PC/4]);
		if (opcode == 0x3f) break; /* halt */
	}
	PrintReport();
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
		D_hard_drive[i] = 0;
		I_hard_drive[i] = 0;
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
		I_hard_drive[i] = mem_code_i[j];
	}
	for(i = 0, j = 2; i < word_num_d; i++, j++){
		D_hard_drive[i] = mem_code_d[j];
	}
	free(mem_code_d);
	free(mem_code_i);
}

/* decode the data */
void decode(int IR){
	ITLBlookup(PC);
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
		fprintf(stderr, "Illegal testcase: Unknown R-Type Instruction in cycle: %u\n", CycleCount);
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
					fprintf(stderr, "Illegal testcase: Unknown J-Type Instruction in cycle: %u\n", CycleCount);
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
		fprintf(stderr, "Illegal testcase: Unknown I-Type Instruction in cycle: %u\n", CycleCount);
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
					DTLBlookup(R[rs] + imm);
					R[rt] = D_hard_drive[(R[rs] + imm)/4];
					break;
        case 0x21: /* lh (load halfword) */
					AccessMemN_A_ErrorTest();
					/* Misalignment error */
					if ((R[rs] + imm) % 2 != 0) ErrorMsg(4);
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					DTLBlookup(R[rs] + imm);
					R[rt] = (D_hard_drive[(R[rs] + imm)/4] << ((2 - finite_groupZ4(R[rs] + imm)) * 8)) >> 16; /* sign extension */
					break;
        case 0x25: /* lhu (load halfword unsigned) */
					AccessMemN_A_ErrorTest();
					/* Misalignment error */
					if ((R[rs] + imm) % 2 != 0) ErrorMsg(4);
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					DTLBlookup(R[rs] + imm);
					R[rt] = (D_hard_drive[(R[rs] + imm)/4] >> (finite_groupZ4(R[rs] + imm) * 8)) & 0xffff;
					break;
        case 0x20: /* lb (load one byte) */
					AccessMemN_A_ErrorTest();
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					DTLBlookup(R[rs] + imm);
					R[rt] = (D_hard_drive[(R[rs] + imm)/4] << ((3 - finite_groupZ4(R[rs] + imm)) * 8)) >> 24; /* sign extension */
					break;
		case 0x24: /* lbu (load one byte unsigned) */
					AccessMemN_A_ErrorTest();
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					DTLBlookup(R[rs] + imm);
					R[rt] = (D_hard_drive[(R[rs] + imm)/4] >> (finite_groupZ4(R[rs] + imm) * 8)) & 0xff;
					break;
		case 0x2B: /* sw (store word) */
					AccessMemN_A_ErrorTest();
					/* Misalignment error */
					if ((R[rs] + imm) % 4 != 0) ErrorMsg(4);
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					DTLBlookup(R[rs] + imm);
					D_hard_drive[(R[rs] + imm)/4] = R[rt];
					break;
		case 0x29: /* sh (store halfword) */
					AccessMemN_A_ErrorTest();
					/* Misalignment error */
					if ((R[rs] + imm) % 2 != 0) ErrorMsg(4);
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					DTLBlookup(R[rs] + imm);
					D_hard_drive[(R[rs] + imm)/4] &= ~(0xffff << (finite_groupZ4(R[rs] + imm) * 8)); /* clear */
					D_hard_drive[(R[rs] + imm)/4] |= (R[rt] & 0xffff) << (finite_groupZ4(R[rs] + imm) * 8);
					break;
		case 0x28: /* sb (store one byte) */
					AccessMemN_A_ErrorTest();
					if (HaltFlag == 1) exit(EXIT_FAILURE);
					DTLBlookup(R[rs] + imm);
					D_hard_drive[(R[rs] + imm)/4] &= ~(0xff << (finite_groupZ4(R[rs] + imm) * 8)); /* clear */
					D_hard_drive[(R[rs] + imm)/4] |= (R[rt] & 0xff) << (finite_groupZ4(R[rs] + imm) * 8);
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
	//FILE *fh_e = fopen("error_dump.rpt","a");
	switch (ZipCode){
		case 1:/* Try to write to $0 */
				fprintf(stderr, "Write $0 error in cycle: %u\n", CycleCount);
				break;
		case 2:/* Number overflow */
				fprintf(stderr, "Number overflow in cycle: %u\n", CycleCount);
				break;
		case 3:/* Address overflow */
				fprintf(stderr, "Address overflow in cycle: %u\n", CycleCount);
				HaltFlag = 1;
				break;
		case 4:/* Misalignment error */
				fprintf(stderr, "Misalignment error in cycle: %u\n", CycleCount);
				HaltFlag = 1;
				break;
		default:
				break;
		}
		//fclose(fh_e);
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

void PrintReport(){
	//int i;
	FILE *fh_r = fopen("report.rpt","w");
	fprintf(fh_r, 
		"ICache :\n"
		"# hits: %u\n"
		"# misses: %u\n\n", ICache_hits, ICache_misses);
	fprintf(fh_r, 
		"DCache :\n"
		"# hits: %u\n"
		"# misses: %u\n\n", DCache_hits, DCache_misses);
	fprintf(fh_r, 
		"ITLB :\n"
		"# hits: %u\n"
		"# misses: %u\n\n", ITLB_hits, ITLB_misses);
	fprintf(fh_r, 
		"DTLB :\n"
		"# hits: %u\n"
		"# misses: %u\n\n", DTLB_hits, DTLB_misses);
	fprintf(fh_r, 
		"IPageTable :\n"
		"# hits: %u\n"
		"# misses: %u\n\n", IPTE_hits, IPTE_misses);
	fprintf(fh_r, 
		"DPageTable :\n"
		"# hits: %u\n"
		"# misses: %u\n\n", DPTE_hits, DPTE_misses);
	fclose(fh_r);
	/*for(i = 0; i < ICACHE_blocks; i++)
		printf("I_CACHE.tag[%d] = %d\n", i, I_CACHE.tag[i]);*/
}

void AllocateMemory(int argc, char **argv){
	int i;
	/* IMEM-size, DMEM-size, I-pagesize, D-pagesize, ICACHE-size
	   I-blocksize, I-associativity, DCACHE-size, D-blocksize, D-associativity*/
	int ArgArray[10] = {64, 32, 8, 16, 16, 4, 4, 16, 4, 1};
	if (argc == 11){
		for (i = 0 ; i < 10 ; i++){
			ArgArray[i] = atoi(argv[i+1]);
			if (i != 6 && i != 9 && ArgArray[i] % 4 != 0){
				printf("argv[%d] is not a multiple of 4.", i+1);
				exit(EXIT_FAILURE);
			}
		}
	}
	if (argc == 1 || argc == 11){
		I_associativity = ArgArray[6];
		D_associativity = ArgArray[9];

		I_pagesize = ArgArray[2];
		D_pagesize = ArgArray[3];

		I_blocksize = ArgArray[5];
		D_blocksize = ArgArray[8];

		IMEM_blocks = ArgArray[0]/I_pagesize;
		DMEM_blocks = ArgArray[1]/D_pagesize;
		IPTE_entries = 1024/I_pagesize;
		DPTE_entries = 1024/D_pagesize;
		ITLB_entries = (1024/I_pagesize)/4;
		DTLB_entries = (1024/D_pagesize)/4;
		ICACHE_blocks = ArgArray[4]/I_blocksize;
		DCACHE_blocks = ArgArray[7]/D_blocksize;

		I_MEM.data = (int*) calloc(ArgArray[0]/sizeof(int), sizeof(int));
		I_MEM.valid = (int*) calloc(IMEM_blocks, sizeof(int));
		I_MEM.LRUorder = (int*) calloc(IMEM_blocks, sizeof(int));

		D_MEM.data = (int*) calloc(ArgArray[1]/sizeof(int), sizeof(int));
		D_MEM.valid = (int*) calloc(DMEM_blocks, sizeof(int));
		D_MEM.LRUorder = (int*) calloc(DMEM_blocks, sizeof(int));

		I_PTE.content = (int*) calloc(IPTE_entries, sizeof(int));
		I_PTE.valid = (int*) calloc(IPTE_entries, sizeof(int));
		D_PTE.content = (int*) calloc(DPTE_entries, sizeof(int));
		D_PTE.valid = (int*) calloc(DPTE_entries, sizeof(int));

		I_TLB.content = (int*) calloc(ITLB_entries, sizeof(int));
		I_TLB.valid = (int*) calloc(ITLB_entries, sizeof(int));
		I_TLB.tag = (int*) calloc(ITLB_entries, sizeof(int));
		I_TLB.LRUorder = (int*) calloc(ITLB_entries, sizeof(int));
		D_TLB.content = (int*) calloc(DTLB_entries, sizeof(int));
		D_TLB.valid = (int*) calloc(DTLB_entries, sizeof(int));
		D_TLB.tag = (int*) calloc(DTLB_entries, sizeof(int));
		D_TLB.LRUorder = (int*) calloc(DTLB_entries, sizeof(int));

		I_CACHE.data = (int*) calloc(ArgArray[4]/sizeof(int), sizeof(int));
		I_CACHE.valid = (int*) calloc(ICACHE_blocks, sizeof(int));
		I_CACHE.tag = (int*) calloc(ICACHE_blocks, sizeof(int));
		I_CACHE.LRUorder = (int*) calloc(ICACHE_blocks, sizeof(int));
		D_CACHE.data = (int*) calloc(ArgArray[7]/sizeof(int), sizeof(int));
		D_CACHE.valid = (int*) calloc(DCACHE_blocks, sizeof(int));
		D_CACHE.tag = (int*) calloc(DCACHE_blocks, sizeof(int));
		D_CACHE.LRUorder = (int*) calloc(DCACHE_blocks, sizeof(int));

		if (I_MEM.data == NULL || I_MEM.valid == NULL || D_MEM.data == NULL || D_MEM.valid == NULL || 
			I_PTE.content == NULL || D_PTE.content == NULL || I_CACHE.data == NULL || I_CACHE.valid == NULL || 
			D_CACHE.data == NULL || D_CACHE.valid == NULL ){
			fprintf(stderr, "Cannot allocate dynamic memory\n");
			exit(3);
		}
		// initialize
		for (i = 0; i < IPTE_entries; i++)	{I_PTE.content[i] = -1;I_PTE.valid[i] = 0;}
		for (i = 0; i < DPTE_entries; i++)	{D_PTE.content[i] = -1;D_PTE.valid[i] = 0;}
		for (i = 0; i < ITLB_entries; i++)	{I_TLB.LRUorder[i] = -1;I_TLB.content[i] = -1; I_TLB.valid[i] = 0; I_TLB.tag[i] = -1;}
		for (i = 0; i < DTLB_entries; i++)	{D_TLB.LRUorder[i] = -1;D_TLB.content[i] = -1; D_TLB.valid[i] = 0; D_TLB.tag[i] = -1;}
		for (i = 0; i < IMEM_blocks; i++)	{I_MEM.LRUorder[i] = -1;I_MEM.valid[i] = 0;}
		for (i = 0; i < DMEM_blocks; i++)	{D_MEM.LRUorder[i] = -1;D_MEM.valid[i] = 0;}
		for (i = 0; i < ICACHE_blocks; i++)	{I_CACHE.LRUorder[i] = -1;I_CACHE.tag[i] = -1;I_CACHE.valid[i] = 0;}
		for (i = 0; i < DCACHE_blocks; i++)	{D_CACHE.LRUorder[i] = -1;D_CACHE.tag[i] = -1;D_CACHE.valid[i] = 0;}
	}
	else{ 
		fprintf(stderr, "The amount of the parameters after \"./CMP\" should be 0 \"OR\" 10.\n");
		exit(EXIT_FAILURE);
	}
}

void ITLBlookup(int addr){
	int i, j, k, Replaced_Index = -1, Changeto_invalid_Index = -1;
	// search
	for (i = 0 ; i < ITLB_entries ; i++){
		// search for nonempty
		if (I_TLB.valid[i] == 1){
			 // ITLB hit
			if (I_TLB.tag[i] == addr/I_pagesize){
				ITLB_hits ++;
				// LRU replacemenet policy (reordering LRU), must be found in LRU order
				for(j = 0; j < ITLB_entries; j++){
					if (I_TLB.LRUorder[j] == i){
						for (k = j; k < ITLB_entries - 1; k++)
							I_TLB.LRUorder[k] = I_TLB.LRUorder[k+1];
						I_TLB.LRUorder[ITLB_entries - 1] = -1;
						for (k = j; k < ITLB_entries; k++){
							if (I_TLB.LRUorder[k] == -1){
								I_TLB.LRUorder[k] = i;
								break;
							}
						}	
					}
				}	
				ICACHElookup(I_TLB.content[i] + (addr % I_pagesize));
				return;
			}
		}
	}
	// ITLB miss
	ITLB_misses++;
	Changeto_invalid_Index = IPTElookup(addr);
	// renew ITLB
	for (i = 0; i < ITLB_entries; i++){
		// search for invalid place(= empty or wrong info.)
		if (I_TLB.valid[i] == 0){
			I_TLB.valid[i] = 1;
			I_TLB.tag[i] = addr/I_pagesize;
			I_TLB.content[i] = I_PTE.content[addr/I_pagesize];
			// LRU replacemenet policy (reordering LRU)
			for(j = 0; j < ITLB_entries; j++){
				if (I_TLB.LRUorder[j] == i){
					for (k = j ; k < ITLB_entries - 1 ; k++)
						I_TLB.LRUorder[k] = I_TLB.LRUorder[k+1];
					I_TLB.LRUorder[ITLB_entries - 1] = -1;
					for (k = j ; k < ITLB_entries ; k++){
						if (I_TLB.LRUorder[k] == -1){
							I_TLB.LRUorder[k] = i;
							break;
						}
					}	
				}
			}	
			// not found in LRU order
			if (j == ITLB_entries){
				for(j = 0; j < ITLB_entries; j++){
					if (I_TLB.LRUorder[j] == -1){
							I_TLB.LRUorder[j] = i;
							break;
					}
				}
			}
			ICACHElookup(I_TLB.content[i]  + (addr % I_pagesize));
			// IPTE miss, may an entry change to invalid 
			if (Changeto_invalid_Index != -1){
				for (i = 0; i < ITLB_entries; i++){
					if (I_TLB.tag[i] != -1 && I_TLB.tag[i] == Changeto_invalid_Index){
						I_TLB.tag[i] = -1;
						I_TLB.valid[i] = 0;
					}
				}
			}
			return;
		}
	}
	// not found invalid place, check LRU for replacing
	if (i == ITLB_entries){
		Replaced_Index = I_TLB.LRUorder[0];
		I_TLB.tag[Replaced_Index] = addr/I_pagesize;
		I_TLB.content[Replaced_Index] = I_PTE.content[addr/I_pagesize];
		// LRU reorder
		for(i = 0; i < ITLB_entries - 1; i++)
			I_TLB.LRUorder[i] = I_TLB.LRUorder[i+1];
		I_TLB.LRUorder[ITLB_entries - 1] = Replaced_Index;
	}
	// IPTE miss, may an entry change to invalid 
	if (Changeto_invalid_Index != -1){
		for (i = 0; i < ITLB_entries; i++){
			if (I_TLB.tag[i] != -1 && I_TLB.tag[i] == Changeto_invalid_Index){
						I_TLB.tag[i] = -1;
						I_TLB.valid[i] = 0;
					}
		}
	}
	ICACHElookup(I_TLB.content[Replaced_Index]  + (addr % I_pagesize));
	return;
}

/* If has the valid bit of some index changed to 0 (invalid), return the index.
 * Otherwise, return -1.*/
int IPTElookup(int addr){
	int i, j, k, Replaced_Index = -1, Check_Same_page_of_MEM = -1; 
	int MoveHowManyWords = I_pagesize/sizeof(int);
	//IPTE hit, data in mem
	/*for(i = 0; i < IPTE_entries; i++)
		printf("I_PTE.valid[%d] = %d, I_PTE.content[%d] = %d\n", i, I_PTE.valid[i], i, I_PTE.content[i]);*/
	if (I_PTE.valid[addr/I_pagesize] == 1){
		IPTE_hits++;
		return -1;
	}
	else{	//IPTE miss (page fault)
		IPTE_misses++;
		I_PTE.valid[addr/I_pagesize] = 1;
		// move data from I_hard_drive to IMEM
		for (i = 0; i < IMEM_blocks; i++){
			// search for empty, move the page, reorder LRU, tell PTE the physical address.
			if (I_MEM.valid[i] == 0){
				I_MEM.valid[i] = 1;
				for(j = addr/sizeof(int), k = i * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					I_MEM.data[k] = I_hard_drive[j];
				I_PTE.content[addr/I_pagesize] = Check_Same_page_of_MEM = i*I_pagesize;
				// LRU replacemenet policy (reordering LRU)
				for(j = 0; j < IMEM_blocks; j++){
					if (I_MEM.LRUorder[j] == i){
						for (k = j ; k < IMEM_blocks - 1 ; k++)
							I_MEM.LRUorder[k] = I_MEM.LRUorder[k+1];
						I_MEM.LRUorder[IMEM_blocks - 1] = -1;
						for (k = j ; k < IMEM_blocks ; k++){
							if (I_MEM.LRUorder[k] == -1){
								I_MEM.LRUorder[k] = i;
								break;
							}
						}	
					}
				}	
				// not found in LRU order
				if (j == IMEM_blocks){
					for(j = 0; j < IMEM_blocks; j++){
						if (I_MEM.LRUorder[j] == -1){
							I_MEM.LRUorder[j] = i;
							break;
						}
					}
				}	
				break;
			}
		}

		// if no empty place, find LRU, move the page, tell PTE the physical address, reorder LRU.
		if (i == IMEM_blocks){
			Replaced_Index = I_MEM.LRUorder[0];
			for(j = addr/sizeof(int), k = Replaced_Index * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					I_MEM.data[k] = I_hard_drive[j];
			I_PTE.content[addr/I_pagesize] = Check_Same_page_of_MEM = Replaced_Index*I_pagesize;
			// LRU reorder
			for(i = 0; i < IMEM_blocks - 1; i++)
				I_MEM.LRUorder[i] = I_MEM.LRUorder[i+1];
			I_MEM.LRUorder[IMEM_blocks - 1] = Replaced_Index;
		}
		// Check if same page of MEM in PTE
		for (i = 0; i < IPTE_entries; i++){
			if (I_PTE.content[i] != -1 && (I_PTE.content[i])/I_pagesize == (Check_Same_page_of_MEM)/I_pagesize && i != addr/I_pagesize){
				I_PTE.content[i] = -1;
				I_PTE.valid[i] = 0;
				// Cache also need to be invaid if have that page.
				for(j = 0; j < ICACHE_blocks; j++){
					if (I_CACHE.valid[j] == 1 && I_CACHE.tag[j] != -1 && I_CACHE.tag[j]*I_blocksize/I_pagesize == (Check_Same_page_of_MEM)/I_pagesize){
						I_CACHE.tag[j] = -1;
						I_CACHE.valid[j] = 0;
					}
				}
				return i;
			}
		}
		// if no same page of MEM in PTE
		if (i == IPTE_entries)
			return -1;
	}
	return -2;
}

void ICACHElookup(int addr){
	int i, j, k, Replaced_Index = -1;
	int memindex; // actually mem_block_index
	int MoveHowManyWords = I_blocksize/sizeof(int);
	//int setAmount = ICACHE_blocks/I_associativity;
	int which_set;
	memindex = addr / I_blocksize;
	// full associativity
	for (i = 0; i < ICACHE_blocks; i++){
		if (I_CACHE.valid[i] == 1){
			// ICACHE hit
			if (I_CACHE.tag[i] == memindex){
				ICache_hits++;
				// LRU replacemenet policy (reordering LRU), must be found in LRU order
				// fully assoassociative
				 if(I_associativity == ICACHE_blocks){	
					for(j = 0; j < ICACHE_blocks; j++){
						if (I_CACHE.LRUorder[j] == i){
							for (k = j; k < ICACHE_blocks - 1; k++)
								I_CACHE.LRUorder[k] = I_CACHE.LRUorder[k+1];
							I_CACHE.LRUorder[ICACHE_blocks - 1] = -1;
							for (k = j; k < ICACHE_blocks; k++){
								if (I_CACHE.LRUorder[k] == -1){
									I_CACHE.LRUorder[k] = i;		
									break;
								}
							}	
						}
					}	
				}
				// direct map
				else if (I_associativity == 1){
					//no LRU
				}
				// n-ways
				else{
					for (j = (i/I_associativity)*I_associativity ; j < (i/I_associativity)*I_associativity + I_associativity; j++){
						if (I_CACHE.LRUorder[j] == i){
							for (k = j; k < (i/I_associativity)*I_associativity + I_associativity - 1; k++)
								I_CACHE.LRUorder[k] = I_CACHE.LRUorder[k+1];
							I_CACHE.LRUorder[(i/I_associativity)*I_associativity + I_associativity - 1] = -1;
							for (k = j; k < (i/I_associativity)*I_associativity + I_associativity; k++){
								if (I_CACHE.LRUorder[k] == -1){
									I_CACHE.LRUorder[k] = i;		
									break;
								}
							}	
						}	
					}
				}
				return;
			}
		}
	}
	// ICACHE miss
	ICache_misses++;
	// LRU replacemenet policy (reordering LRU), must be found in LRU order
	// fully assoassociative
	if (I_associativity == ICACHE_blocks){
		for (i = 0; i < ICACHE_blocks; i++){
		// search for invalid place(= empty or wrong info.)
			if (I_CACHE.valid[i] == 0){
				I_CACHE.valid[i] = 1;
				I_CACHE.tag[i] = memindex;
				for(j = addr/sizeof(int), k = i * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					I_CACHE.data[k] = I_MEM.data[j];
				// LRU replacemenet policy (reordering LRU)
				for(j = 0; j < ICACHE_blocks; j++){
					if (I_CACHE.LRUorder[j] == i){
						for (k = j ; k < ICACHE_blocks - 1 ; k++)
							I_CACHE.LRUorder[k] = I_CACHE.LRUorder[k+1];
						I_CACHE.LRUorder[ICACHE_blocks - 1] = -1;
						for (k = j ; k < ICACHE_blocks ; k++){
							if (I_CACHE.LRUorder[k] == -1){
								I_CACHE.LRUorder[k] = i;
								break;
							}
						}	
					}
				}	
				// not found in LRU order
				if (j == ICACHE_blocks){
					for(j = 0; j < ICACHE_blocks; j++){
						if (I_CACHE.LRUorder[j] == -1){
							I_CACHE.LRUorder[j] = i;
							break;
						}
					}
				}	
				break;
			}
		}
		// if no empty place
		if (i == ICACHE_blocks){
			Replaced_Index = I_CACHE.LRUorder[0];
			I_CACHE.tag[Replaced_Index] = memindex;
			for(j = addr/sizeof(int), k = Replaced_Index * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					I_CACHE.data[k] = I_MEM.data[j];
			// LRU reorder
			for(i = 0; i < ICACHE_blocks - 1; i++)
				I_CACHE.LRUorder[i] = I_CACHE.LRUorder[i+1];
			I_CACHE.LRUorder[ICACHE_blocks - 1] = Replaced_Index;
		}
	}
	// direct map
	else if (I_associativity == 1){
		i = memindex % ICACHE_blocks;
		I_CACHE.tag[i] = memindex;
		I_CACHE.valid[i] = 1;
		for(j = addr/sizeof(int), k = i * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					I_CACHE.data[k] = I_MEM.data[j];
		// no LRU
	}
	// n-ways
	else {
		which_set = memindex % (ICACHE_blocks/I_associativity);
		// check valid bit
		for (i = which_set*I_associativity; i < which_set*I_associativity + I_associativity; i++){
			if (I_CACHE.valid[i] == 0){
				I_CACHE.valid[i] = 1;
				I_CACHE.tag[i] = memindex;
				for(j = addr/sizeof(int), k = i * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					I_CACHE.data[k] = I_MEM.data[j];
				// LRU replacemenet policy (reordering LRU)
				for(j = which_set*I_associativity; j < which_set*I_associativity + I_associativity; j++){
					if (I_CACHE.LRUorder[j] == i){
						for (k = j ; k <  which_set*I_associativity + I_associativity - 1 ; k++)
							I_CACHE.LRUorder[k] = I_CACHE.LRUorder[k+1];
						I_CACHE.LRUorder[which_set*I_associativity + I_associativity - 1] = -1;
						for (k = j ; k <  which_set*I_associativity + I_associativity ; k++){
							if (I_CACHE.LRUorder[k] == -1){
								I_CACHE.LRUorder[k] = i;
								break;
							}
						}	
					}
				}	
				// not found in LRU order
				if (j == which_set*I_associativity + I_associativity){
					for(j = which_set*I_associativity; j <  which_set*I_associativity + I_associativity; j++){
						if (I_CACHE.LRUorder[j] == -1){
							I_CACHE.LRUorder[j] = i;
							break;
						}
					}
				}	
				break;
			}
		}
		// if no empty place
		if (i == which_set*I_associativity + I_associativity){
			Replaced_Index = I_CACHE.LRUorder[which_set*I_associativity];
			I_CACHE.tag[Replaced_Index] = memindex;
			for(j = addr/sizeof(int), k = Replaced_Index * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					I_CACHE.data[k] = I_MEM.data[j];
			// LRU reorder
			for(i =  which_set*I_associativity; i < which_set*I_associativity + I_associativity - 1; i++)
				I_CACHE.LRUorder[i] = I_CACHE.LRUorder[i+1];
			I_CACHE.LRUorder[which_set*I_associativity + I_associativity - 1] = Replaced_Index;
		}
	}
	/*printf("addr = %d, memindex = %d, which_set = %d, PC = %d, cycle = %d\n", addr, memindex, which_set, PC, CycleCount);
		for(i = 0; i < ICACHE_blocks; i++){
		printf("I_CACHE.valid[%d] = %d, I_CACHE.tag[%d] = %d, I_CACHE.LRUorder[%d] = %d\n", 
			i, I_CACHE.valid[i], i, I_CACHE.tag[i], i,I_CACHE.LRUorder[i]);
		}*/
}

void DTLBlookup(int addr){
	int i, j, k, Replaced_Index = -1, Changeto_invalid_Index = -1;
	// search
	/*printf("addr = %d\n", addr);
	for(i = 0; i < DTLB_entries ; i ++){
		printf("D_TLB.tag[%d] = %d, D_TLB.valid[%d] = %d, D_TLB.content[%d] = %d, PC = %d, CycleCount = %d\n", 
			i, D_TLB.tag[i], i, D_TLB.valid[i], i, D_TLB.content[i], PC, CycleCount);
	}
	puts(" ");*/
	for (i = 0 ; i < DTLB_entries ; i++){
		// search for nonempty
		if (D_TLB.valid[i] == 1){
			 // DTLB hit
			if (D_TLB.tag[i] == addr/D_pagesize){
				DTLB_hits ++;
				// LRU replacemenet policy (reordering LRU), must be found in LRU order
				for(j = 0; j < DTLB_entries; j++){
					if (D_TLB.LRUorder[j] == i){
						for (k = j; k < DTLB_entries - 1; k++)
							D_TLB.LRUorder[k] = D_TLB.LRUorder[k+1];
						D_TLB.LRUorder[DTLB_entries - 1] = -1;
						for (k = j; k < DTLB_entries; k++){
							if (D_TLB.LRUorder[k] == -1){
								D_TLB.LRUorder[k] = i;
								break;
							}
						}	
					}
				}	
				DCACHElookup(D_TLB.content[i] + (addr % D_pagesize));
				return;
			}
		}
	}

	// DTLB miss
	DTLB_misses++;
	Changeto_invalid_Index = DPTElookup(addr);
	// renew DTLB
	for (i = 0; i < DTLB_entries; i++){
		// search for invalid place(= empty or wrong info.)
		if (D_TLB.valid[i] == 0){
			D_TLB.valid[i] = 1;
			D_TLB.tag[i] = addr/D_pagesize;
			D_TLB.content[i] = D_PTE.content[addr/D_pagesize];
			// LRU replacemenet policy (reordering LRU)
			for(j = 0; j < DTLB_entries; j++){
				if (D_TLB.LRUorder[j] == i){
					for (k = j ; k < DTLB_entries - 1 ; k++)
						D_TLB.LRUorder[k] = D_TLB.LRUorder[k+1];
					D_TLB.LRUorder[DTLB_entries - 1] = -1;
					for (k = j ; k < DTLB_entries ; k++){
						if (D_TLB.LRUorder[k] == -1){
							D_TLB.LRUorder[k] = i;
							break;
						}
					}	
				}
			}	
			// not found in LRU order
			if (j == DTLB_entries){
				for(j = 0; j < DTLB_entries; j++){
					if (D_TLB.LRUorder[j] == -1){
							D_TLB.LRUorder[j] = i;
							break;
					}
				}
			}
			DCACHElookup(D_TLB.content[i] + (addr % D_pagesize));
			// DPTE miss, may an entry change to invalid 
			if (Changeto_invalid_Index != -1){
				for (i = 0; i < DTLB_entries; i++){
					if (D_TLB.tag[i] != -1 && D_TLB.tag[i] == Changeto_invalid_Index){
						D_TLB.tag[i] = -1;
						D_TLB.valid[i] = 0;
					}
				}
			}
			return;
		}
	}
	// not found invalid place, check LRU for replacing
	if (i == DTLB_entries){
		Replaced_Index = D_TLB.LRUorder[0];
		D_TLB.tag[Replaced_Index] = addr/D_pagesize;
		D_TLB.content[Replaced_Index] = D_PTE.content[addr/D_pagesize];
		// LRU reorder
		for(i = 0; i < DTLB_entries - 1; i++)
			D_TLB.LRUorder[i] = D_TLB.LRUorder[i+1];
		D_TLB.LRUorder[DTLB_entries - 1] = Replaced_Index;
	}
	// DPTE miss, may an entry change to invalid 
	if (Changeto_invalid_Index != -1){
		for (i = 0; i < DTLB_entries; i++){
			if (D_TLB.tag[i] != -1 && D_TLB.tag[i] == Changeto_invalid_Index){
				D_TLB.tag[i] = -1;
				D_TLB.valid[i] = 0;
			}
		}
	}
	DCACHElookup(D_TLB.content[Replaced_Index] + (addr % D_pagesize));
	return;
}

/* If has the valid bit of some index changed to 0 (invalid), return the index.
 * Otherwise, return -1.*/
int DPTElookup(int addr){
	int i, j, k, Replaced_Index = -1, Check_Same_page_of_MEM = -1; 
	int MoveHowManyWords = D_pagesize/sizeof(int);
	//DPTE hit, data in mem
	if (D_PTE.valid[addr/D_pagesize] == 1){
		DPTE_hits++;
		return -1;
	}
	else{	//DPTE miss (page fault)
		DPTE_misses++;
		D_PTE.valid[addr/D_pagesize] = 1;
		// move data from D_hard_drive to DMEM
		/*for (i = 0; i < DMEM_blocks; i++){
			printf("%d\n", D_MEM.valid[i]);
		}*/
		for (i = 0; i < DMEM_blocks; i++){
			// search for empty, move the page, reorder LRU, tell PTE the physical address.
			if (D_MEM.valid[i] == 0){
				D_MEM.valid[i] = 1;
				for(j = addr/sizeof(int), k = i * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					D_MEM.data[k] = D_hard_drive[j];
				D_PTE.content[addr/D_pagesize] = Check_Same_page_of_MEM = i*D_pagesize;
				//printf("D_PTE.content[%d] = %d\n", addr/D_pagesize, D_PTE.content[addr/D_pagesize]);
				// LRU replacemenet policy (reordering LRU)
				for(j = 0; j < DMEM_blocks; j++){
					if (D_MEM.LRUorder[j] == i){
						for (k = j ; k < DMEM_blocks - 1 ; k++)
							D_MEM.LRUorder[k] = D_MEM.LRUorder[k+1];
						D_MEM.LRUorder[DMEM_blocks - 1] = -1;
						for (k = j ; k < DMEM_blocks ; k++){
							if (D_MEM.LRUorder[k] == -1){
								D_MEM.LRUorder[k] = i;
								break;
							}
						}	
					}
				}	
				// not found in LRU order
				if (j == DMEM_blocks){
					for(j = 0; j < DMEM_blocks; j++){
						if (D_MEM.LRUorder[j] == -1){
							D_MEM.LRUorder[j] = i;
							break;
						}
					}
				}
				break;
			}
		}

		// if no empty place, find LRU, move the page, tell PTE the physical address, reorder LRU.
		if (i == DMEM_blocks){
			//for (i = 0; i < DMEM_blocks; i++)
			//printf("DLRU %d\n", D_MEM.LRUorder[i]);
			Replaced_Index = D_MEM.LRUorder[0];
			for(j = addr/sizeof(int), k = Replaced_Index * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					D_MEM.data[k] = D_hard_drive[j];
			D_PTE.content[addr/D_pagesize] = Check_Same_page_of_MEM = Replaced_Index*D_pagesize;
			//printf("%d\n", Check_Same_page_of_MEM);
			// LRU reorder
			for(i = 0; i < DMEM_blocks - 1; i++)
				D_MEM.LRUorder[i] = D_MEM.LRUorder[i+1];
			D_MEM.LRUorder[DMEM_blocks - 1] = Replaced_Index;
		}
		// Check if same page of MEM in PTE
		for (i = 0; i < DPTE_entries; i++){
			if (D_PTE.content[i] != -1 && (D_PTE.content[i])/D_pagesize == (Check_Same_page_of_MEM)/D_pagesize && i != addr/D_pagesize){
				D_PTE.content[i] = -1;
				D_PTE.valid[i] = 0;
				// Cache also need to be invaid if have that page.
				for(j = 0; j < DCACHE_blocks; j++){
					if (D_CACHE.valid[j] == 1 && D_CACHE.tag[j] != -1 && D_CACHE.tag[j]*D_blocksize/D_pagesize == (Check_Same_page_of_MEM)/D_pagesize){
						D_CACHE.tag[j] = -1;
						D_CACHE.valid[j] = 0;
					}
				}
				return i;
			}
		}
		// if no same page of MEM in PTE
		if (i == DPTE_entries)
			return -1;
	}
	exit(EXIT_FAILURE);
}

void DCACHElookup(int addr){
	int i, j, k, Replaced_Index = -1;
	int memindex; // actually mem_block_index
	int MoveHowManyWords = D_blocksize/sizeof(int);
	//int setAmount = DCACHE_blocks/D_associativity;
	int which_set;
	memindex = addr / D_blocksize;
	/*printf("addr = %d, memindex = %d, PC = %d, cycle = %d\n", addr, memindex, PC, CycleCount);
		for(i = 0; i < DCACHE_blocks; i++){
		printf("D_CACHE.valid[%d] = %d, D_CACHE.tag[%d] = %d, D_CACHE.LRUorder[%d] = %d\n", 
			i, D_CACHE.valid[i], i, D_CACHE.tag[i], i, D_CACHE.LRUorder[i]);
		}*/
	// full associativity
	for (i = 0; i < DCACHE_blocks; i++){
		if (D_CACHE.valid[i] == 1){
			// DCACHE hit
			if (D_CACHE.tag[i] == memindex){
				DCache_hits++;
				// LRU replacemenet policy (reordering LRU), must be found in LRU order
				// fully assoassociative
				 if(D_associativity == DCACHE_blocks){	
					for(j = 0; j < DCACHE_blocks; j++){
						if (D_CACHE.LRUorder[j] == i){
							for (k = j; k < DCACHE_blocks - 1; k++)
								D_CACHE.LRUorder[k] = D_CACHE.LRUorder[k+1];
							D_CACHE.LRUorder[DCACHE_blocks - 1] = -1;
							for (k = j; k < DCACHE_blocks; k++){
								if (D_CACHE.LRUorder[k] == -1){
									D_CACHE.LRUorder[k] = i;		
									break;
								}
							}	
						}
					}	
				}
				// direct map
				else if (D_associativity == 1){
					//no LRU
				}
				// n-ways
				else{
					for (j = (i/D_associativity)*D_associativity ; j < (i/D_associativity)*D_associativity + D_associativity; j++){
						if (D_CACHE.LRUorder[j] == i){
							for (k = j; k < (i/D_associativity)*D_associativity + D_associativity - 1; k++)
								D_CACHE.LRUorder[k] = D_CACHE.LRUorder[k+1];
							D_CACHE.LRUorder[(i/D_associativity)*D_associativity + D_associativity - 1] = -1;
							for (k = j; k < (i/D_associativity)*D_associativity + D_associativity; k++){
								if (D_CACHE.LRUorder[k] == -1){
									D_CACHE.LRUorder[k] = i;		
									break;
								}
							}	
						}	
					}
				}
				return;
			}
		}
	}
	// DCACHE miss
	DCache_misses++;
	// LRU replacemenet policy (reordering LRU), must be found in LRU order
	// fully assoassociative
	if (D_associativity == DCACHE_blocks){
		for (i = 0; i < DCACHE_blocks; i++){
		// search for invalid place(= empty or wrong info.)
			if (D_CACHE.valid[i] == 0){
				D_CACHE.valid[i] = 1;
				D_CACHE.tag[i] = memindex;
				for(j = addr/sizeof(int), k = i * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					D_CACHE.data[k] = D_MEM.data[j];
				// LRU replacemenet policy (reordering LRU)
				for(j = 0; j < DCACHE_blocks; j++){
					if (D_CACHE.LRUorder[j] == i){
						for (k = j ; k < DCACHE_blocks - 1 ; k++)
							D_CACHE.LRUorder[k] = D_CACHE.LRUorder[k+1];
						D_CACHE.LRUorder[DCACHE_blocks - 1] = -1;
						for (k = j ; k < DCACHE_blocks ; k++){
							if (D_CACHE.LRUorder[k] == -1){
								D_CACHE.LRUorder[k] = i;
								break;
							}
						}	
					}
				}	
				// not found in LRU order
				if (j == DCACHE_blocks){
					for(j = 0; j < DCACHE_blocks; j++){
						if (D_CACHE.LRUorder[j] == -1){
							D_CACHE.LRUorder[j] = i;
							break;
						}
					}
				}	
				break;
			}
		}
		// if no empty place
		if (i == DCACHE_blocks){
			Replaced_Index = D_CACHE.LRUorder[0];
			D_CACHE.tag[Replaced_Index] = memindex;
			for(j = addr/sizeof(int), k = Replaced_Index * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					D_CACHE.data[k] = D_MEM.data[j];
			// LRU reorder
			for(i = 0; i < DCACHE_blocks - 1; i++)
				D_CACHE.LRUorder[i] = D_CACHE.LRUorder[i+1];
			D_CACHE.LRUorder[DCACHE_blocks - 1] = Replaced_Index;
		}
	}
	// direct map
	else if (D_associativity == 1){
		i = memindex % DCACHE_blocks;
		//printf("i = %d, memindex = %d, PC = %d\n\n", i, memindex, PC);
		D_CACHE.tag[i] = memindex;
		D_CACHE.valid[i] = 1;
		for(j = addr/sizeof(int), k = i * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					D_CACHE.data[k] = D_MEM.data[j];
		// no LRU
	}
	// n-ways
	else {
		which_set = memindex % (DCACHE_blocks/D_associativity);
		// check valid bit
		for (i = which_set*D_associativity; i < which_set*D_associativity + D_associativity; i++){
			if (D_CACHE.valid[i] == 0){
				D_CACHE.tag[i] = memindex;
				for(j = addr/sizeof(int), k = i * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					D_CACHE.data[k] = D_MEM.data[j];
				// LRU replacemenet policy (reordering LRU)
				for(j = which_set*D_associativity; j < which_set*D_associativity + D_associativity; j++){
					if (D_CACHE.LRUorder[j] == i){
						for (k = j ; k <  which_set*D_associativity + D_associativity - 1 ; k++)
							D_CACHE.LRUorder[k] = D_CACHE.LRUorder[k+1];
						D_CACHE.LRUorder[which_set*D_associativity + D_associativity - 1] = -1;
						for (k = j ; k <  which_set*D_associativity + D_associativity ; k++){
							if (D_CACHE.LRUorder[k] == -1){
								D_CACHE.LRUorder[k] = i;
								break;
							}
						}	
					}
				}	
				// not found in LRU order
				if (j == which_set*D_associativity + D_associativity){
					for(j = which_set*D_associativity; j <  which_set*D_associativity + D_associativity; j++){
						if (D_CACHE.LRUorder[j] == -1){
							D_CACHE.LRUorder[j] = i;
							break;
						}
					}
				}	
				break;
			}
		}
		// if no empty place
		if (i == which_set*D_associativity + D_associativity){
			Replaced_Index = D_CACHE.LRUorder[which_set*D_associativity];
			D_CACHE.tag[Replaced_Index] = memindex;
			for(j = addr/sizeof(int), k = Replaced_Index * MoveHowManyWords; j < addr/sizeof(int) + MoveHowManyWords; j++, k++)
					D_CACHE.data[k] = D_MEM.data[j];
			// LRU reorder
			for(i =  which_set*D_associativity; i < which_set*D_associativity + D_associativity - 1; i++)
				D_CACHE.LRUorder[i] = D_CACHE.LRUorder[i+1];
			D_CACHE.LRUorder[which_set*D_associativity + D_associativity - 1] = Replaced_Index;
		}
	}
}
