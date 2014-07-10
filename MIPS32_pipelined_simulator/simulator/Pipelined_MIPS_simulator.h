/* Architecture Project 2
 * Team number: Archi01
 * Leader: 101021120, Y.Xiang Zheng (Slighten), student01
 * Member: x1022108, S.Jie Cao, student06
 * First submission Due: 2014-05-10 23:30
 * Second submission Due: 2014-05-17 23:30
 * Filename: Pipelined_MIPS_simulator.h
 * This is the header file of Pipelined_MIPS_simulator.c
 */

#ifndef __PIPELINED_MIPS_SIMULATOR__
#define __PIPELINED_MIPS_SIMULATOR__

void InstrFetch(void);
void InstrDecode(void);
void Execute(void);
void DataMemoryAccess(void);
void WriteBack(void);
void PrintCycle(FILE *fh);
void Synchronize(void);
void CleanArrays(void); /* clean arrays and to all zero */
void initialize(void); /* reset variables */
void distribute(void); /* load data to memory */
void ErrorMsg(int ZipCode); /* error handler for possible error cases */
void ReadFile(void); /* This function opens and reads the bin file, allocates code memory, and in case of errors, it prints the error and exits.*/
int finite_groupZ4(int x); /* for finite_groupZ4(-1) = 3, finite_groupZ4(5) = 1 */

#endif
