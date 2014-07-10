/* Architecture Project 1
 * Team number: #1
 * Leader: 101021120, Y.Xiang Zheng (Slighten), student01
 * Member: x1022108, S.Jie Cao, student06
 * Due date: 2013-04-05 23:30
 * Filename: Simple_MIPS_simulator.h
 * This is the header file of Simple_MIPS_simulator.c
 */

#ifndef __SIMPLE_MIPS_SIMULATOR__
#define __SIMPLE_MIPS_SIMULATOR__

void CleanArrays(void); /* clean arrays and to all zero */
void initialize(void); /* reset variables */
void distribute(void); /* load data to memory */
void ErrorMsg(int ZipCode); /* error handler for possible error cases */
void R_type(int IR); /* R-type Instruction */
void I_type(int IR); /* I-type Instruction */
void J_type(int IR); /* J-type Instruction */
void decode(int IR); /* decode the data */
void ReadFile(void); /* This function opens and reads the bin file, allocates code memory, and in case of errors, it prints the error and exits.*/
void AccessMemN_A_ErrorTest(); /* for memory accessing Number overflow & Address overflow error test */
int finite_groupZ4(int x); /* for finite_groupZ4(-1) = 3, finite_groupZ4(5) = 1 */

#endif
