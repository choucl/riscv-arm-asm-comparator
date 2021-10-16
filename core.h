#ifndef CORE_H_
#define CORE_H_

/* instruction struct
 * op: instruction op
 * argv: instruction arguments
 * argc: argument count
 * is_end: flag indicates if the ins is end point
 */
typedef struct ins {
  char *op;
  char **argv;
  int argc;
  int is_end;
} INS;

/*
 * split the dumped asm file into small routines and store to files
 * params filename: filname of the large file
 * output: none
 */
void split_routine(char* filename);

/*
 * parse arm asm file
 * params filename: filname of the routine file 
 * output: instructions array of the routine
 */
INS* aarch_parse(char* filename);

/*
 * parse RISC-V asm file
 * params filename: filname of the routine file 
 * output: instructions array of the routine
 */
INS* riscv_parse(char* filename);

/*
 * find the basic block from instructions
 * params instructions: instruction array of a routine
 * output: basic blocks
 */
INS** findbb(INS* instructions);

#endif
