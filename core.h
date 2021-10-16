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
 * params type: specify type of asm, 0 for RISC-V, 1 for Arm
 * output ret_sz: number of outputs 
 * output : names of output files
 */
char** split_routine(char* filename, int type, int* ret_sz);

/*
 * parse Arm asm file
 * params filename: filname of the routine file 
 * output ret_sz: number of outputs 
 * output: instructions array of the routine
 */
INS* aarch_parse(char* filename, int* ret_sz);

/*
 * parse RISC-V asm file
 * params filename: filname of the routine file 
 * output ret_sz: number of outputs 
 * output: instructions array of the routine
 */
INS* riscv_parse(char* filename, int* ret_sz);

/*
 * find the basic block from instructions
 * params instructions: instruction array of a routine
 * output ret_sz: number of outputs 
 * output: basic blocks
 */
INS** findbb(INS* instructions, int* ret_sz);

#endif
