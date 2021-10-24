#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "core.h"

/* free the instruction
 * params ins_arr
 * params ins_sz
 */
static void freeins(INS** ins_arr, int ins_sz) {
  for (int i = 0; i < ins_sz; ++i) {
    if (ins_arr[i]) {
      // free strings
      free(ins_arr[i]->op);
      free(ins_arr[i]->lbl_name);
      for (int j = 0; j < ins_arr[i]->argc; ++j) {
        free(ins_arr[i]->argv[j]);
      }
    }
    free(ins_arr[i]);
  }
  free(ins_arr);
}

/* write bb information included file
 * input bb: list of bb in a subroutine
 * input bb_count: size of all bb
 * input bb_len: len of each bb
 * input name: name of input subroutine file
 * input ins_arr_sz: total len of instructions in this subroutine
 * output: success or not
 */ 
static int writebb(INS*** bb, int bb_count, int* bb_len,
                   int type, char* name, int ins_arr_sz) {
  // setup write file
  char* tmp = calloc(sizeof(char), strlen(name));
  char* fname = calloc(sizeof(char), strlen(name) + 5);
  strncpy(tmp, &name[12], strlen(name) - 20);
  if (type)
    sprintf(fname, "bbroutines/a/%s.aarch.bb", tmp);
  else
    sprintf(fname, "bbroutines/r/%s.riscv.bb", tmp);
  FILE* f = fopen(fname, "w+");
  if (f == NULL) {
    free(fname);
    free(tmp);
    return 0;
  }
  printf("[Writing]\t%s\n", fname);

  // write instruction and bb data into file
  int type_count[5] = {0}; // count for each type
  for (int i = 0; i < bb_count; ++i) {
    fprintf(f, "BB <%d>, %d ins\n", i, bb_len[i]);
    for (int j = 0; j < bb_len[i]; ++j) {
      INS *ins = bb[i][j];
      type_count[ins->type - NA]++;
      fprintf(f, "%x: \t%s\t", ins->addr, ins->op);
      for (int k = 0; k < ins->argc; ++k) {
        fprintf(f, "%s\t", ins->argv[k]);
      }
      if (ins->lbl_name) {
        fprintf(f, "<%s>\t", ins->lbl_name);
      }
      fprintf(f, "(%.2s, %d args)\t\n",
          &"NAARLDSTBR"[2 * (ins->type - NA)], ins->argc);
    }
    fprintf(f, "\n");
  } 
  
  fprintf(f, "================== CONCLUSION ==================\n");
  fprintf(f, "total instructions: %d\n", ins_arr_sz);
  fprintf(f, "average # of ins in a bb: %.3f\n", (float)ins_arr_sz / bb_count);
  for (int i = 0; i < 5; ++i) {
    fprintf(f, "type %.2s count: %d\t(%.3f%%)\n",
        &"NAARLDSTBR"[2 * i], type_count[i],
        (float)type_count[i] * 100 / ins_arr_sz);
  }

  free(fname);
  free(tmp);
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
  int fcount[2] = {0};
  int* ins_len_arr[2];

  for (int i = 0; i < 2; ++i) {
    filenames[i] = argv[i + 1];
    subnames[i] = split_routine(filenames[i], i, &fcount[i]);
    int* tmp = malloc(sizeof(int) * fcount[i]); 
    if (tmp) ins_len_arr[i] = tmp;
    for (int j = 0; j < fcount[i]; ++j) { 
      int ins_arr_sz, bb_count, *bb_sz;
      printf("[Parsing]\t%s\n", subnames[i][j]);
      INS** ins_arr = (i == 0)? riscv_parse(subnames[i][j], &ins_arr_sz):
                                aarch_parse(subnames[i][j], &ins_arr_sz);
      INS*** bb = findbb(ins_arr, ins_arr_sz, &bb_count, &bb_sz);
      ins_len_arr[i][j] = ins_arr_sz;
      if (!writebb(bb, bb_count, bb_sz, i, subnames[i][j], ins_arr_sz)) {
        printf("\nwrite bb file of %s failed. skiped...\n", subnames[i][j]);
      }

      freeins(ins_arr, ins_arr_sz); // free all instructions
    }
  }

  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < fcount[i]; ++j) {
      subnames[i][j][strlen(subnames[i][j]) - 8] = '\0';
    }
  }
  puts("\n===================== CONCLUSION =====================");
  printf("Comparing files:\n");
  printf("RISCV file: [%s]\n", filenames[0]);
  printf("AARCH file: [%s]\n", filenames[1]);
  printf("RISCV subroutine count: %d\n", fcount[0]);
  printf("AARCH subroutine count: %d\n", fcount[1]);
  printf("Subroutines that has more ins than AARCH:\n");
  int count = 0;
  for (int i = 0; i < fcount[0]; ++i) {
    for (int j = 0; j < fcount[1]; ++j) {
      if (strcmp(subnames[0][i], subnames[1][j]) == 0) {
        int diff = ins_len_arr[0][i] - ins_len_arr[1][j];
        float perc = (float)diff / ins_len_arr[1][j] * 100;
        printf("[%-30s]: RISCV: %4d,\tAARCH: %4d,\tdiff: %3d (%.2f%%)\n", 
            &subnames[0][i][12], ins_len_arr[0][i], ins_len_arr[1][j],
            diff, perc);
        count++;
        break;
      }
    }
  }
  printf("Total counts: %d\n", count);
  return 0;
}
