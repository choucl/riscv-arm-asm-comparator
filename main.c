#include <stdio.h>
#include <stdlib.h>
#include "core.h"

int main(int argc, char** argv) {
  if (argc != 3) {
    puts("Please execute w/ 2 arguments, RISC-V asm and aarch64 asm");
    puts("exit program...");
    exit(-1);
  }
  char* riscv_fname = argv[1];
  char* aarch_fname = argv[2];
  
  char **r_subnames, **a_subnames; 
  int r_fcount = 0;
  int a_fcount = 0;

  r_subnames = split_routine(riscv_fname, 0, &r_fcount);  
  a_subnames = split_routine(aarch_fname, 1, &a_fcount);
  
  int ret_sz = 0;
  for (int i = 0; i < r_fcount; ++i) {
    INS* r_ins = riscv_parse(r_subnames[i], &ret_sz);
    INS** r_bb = findbb(r_ins, &ret_sz); 
  }
  for (int i = 0; i < a_fcount; ++i) {
    INS* a_ins = riscv_parse(a_subnames[i], &ret_sz);
    INS** a_bb = findbb(a_ins, &ret_sz); 
  }

  return 0;
}
