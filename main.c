#include <stdio.h>
#include <stdlib.h>
#include "core.h"

int main(int argc, char** argv) {
  if (argc != 3) {
    puts("Please execute w/ 2 arguments, RISC-V asm and aarch64 asm");
    puts("exit program...");
    exit(-1);
  }
  char* filenames[2];
  char** subnames[2];
  int fcount = 0;

  for (int i = 0; i < 2; ++i) {
    filenames[i] = argv[i + 1];
    subnames[i] = split_routine(filenames[i], i, &fcount);
    for (int j = 0; j < fcount; ++j) { 
      int ins_arr_sz, ins_sz, *bb_sz;
      INS** ins_arr = (i == 0)? riscv_parse(subnames[i][j], &ins_arr_sz) :
          aarch_parse(subnames[i][j], &ins_arr_sz);
      INS*** bb = findbb(ins_arr, ins_arr_sz, &ins_sz, &bb_sz);
    }
  }
  return 0;
}
