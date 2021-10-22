#ifndef CORE_H_
#define CORE_H_

/* instruction type enumerate
 * attribute of struct ins
 */
enum type {
  NA = 16, // other instruction types
  AR,      // arithemetic instruction
  LD,      // load instruction
  ST,      // store instruction
  BR       // branch instruction
};

/* instruction struct
 * addr: instruction address
 * op: instruction op
 * argv: instruction arguments
 * argc: argument count
 * lbl_name: the name of the label for a branch/jump instruction
 * is_leader: flag indicates if the ins is a leader
 */
typedef struct ins {
  int addr;
  char *op;
  char **argv;
  int argc;
  char *lbl_name;
  int is_leader;
  int type;
} INS;

/* split the dumped asm file into small routines and store to files
 * params filename: filname of the large file
 * params type: specify type of asm, 0 for RISC-V, 1 for Arm
 * output ret_sz: number of outputs 
 * output : names of output files
 */
char** split_routine(char* filename, int type, int* ret_sz);

/* parse Arm asm file
 * params filename: filname of the routine file 
 * output ret_sz: number of outputs 
 * output: instructions array of the routine
 */
INS** aarch_parse(char* filename, int* ret_sz);

/* parse RISC-V asm file
 * params filename: filname of the routine file 
 * output ret_sz: number of outputs 
 * output: instructions array of the routine
 */
INS** riscv_parse(char* filename, int* ret_sz);

/* find the basic block from instructions
 * params ins: instruction array of a routine
 * params ins_sz: size of the instruction array
 * output ret_sz: number of basic blocks
 * output ret_bb_sz: len of each basic block
 * output: basic blocks
 */
INS*** findbb(INS** ins_arr, int ins_arr_sz, int* ret_sz, int** ret_bb_sz);

/* count the number of ins according to type
 * params ins_arr: array of instructions
 * params type: type of instruction to count
 * params ins_arr_sz: len of instrunction array
 * output: number of corresponding instructions
 */
int count_type(INS** ins_arr, int ins_arr_sz, int type);

#endif
