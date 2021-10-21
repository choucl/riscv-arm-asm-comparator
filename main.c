#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"

/* write bb information included file
 * input bb: list of bb in a subroutine
 * input bb_count: size of all bb
 * input bb_len: len of each bb
 * input name: name of input subroutine file
 * output: success or not
 */ 
static int writebb(INS*** bb, int bb_count, int* bb_len, int type, char* name) {
  // setup write file
  char* tmp = calloc(sizeof(char), strlen(name));
  char* fname = calloc(sizeof(char), strlen(name) + 5);
  strncpy(tmp, &name[12], strlen(name) - 20);
  if (type)
    sprintf(fname, "bbroutines/a/%s.bb.aarch", tmp);
  else
    sprintf(fname, "bbroutines/r/%s.bb.riscv", tmp);
  FILE* f = fopen(fname, "w+");
  if (f == NULL) return 0;
  printf("[writing] %s\n", fname);

  // write instruction and bb data into file
  for (int i = 0; i < bb_count; ++i) {
    fprintf(f, "BB <%d>, %d\n", i, bb_len[i]);
    for (int j = 0; j < bb_len[i]; ++j) {
      INS *ins = bb[i][j];
      fprintf(f, "%x: \t%s\t%.2s\t[%d args]\t%d\t", ins->addr, ins->op,
          &"NAARLDSTBR"[2 * (ins->type - NA)], ins->argc,
          ins->is_leader);
      for (int k = 0; k < ins->argc; ++k) {
        fprintf(f, "%s\t", ins->argv[k]);
      }
      if (ins->lbl_name) {
        fprintf(f, "<%s>\t", ins->lbl_name);
      }
      fprintf(f, "\n");
    }
    fprintf(f, "\n");
  } 
  free(tmp);
  free(name);
  fflush(f);
  fclose(f);
  return 1;
}

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
      int ins_arr_sz, bb_count, *bb_sz;
      printf("[parsing] %s\n", subnames[i][j]);
      INS** ins_arr = (i == 0)? riscv_parse(subnames[i][j], &ins_arr_sz) :
          aarch_parse(subnames[i][j], &ins_arr_sz);
      INS*** bb = findbb(ins_arr, ins_arr_sz, &bb_count, &bb_sz);
      if (!writebb(bb, bb_count, bb_sz, i, subnames[i][j])) {
        printf("\nwrite bb file of %s failed. skiped...\n", subnames[i][j]);
      }
    }
  }
  return 0;
}
