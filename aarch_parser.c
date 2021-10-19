#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define  TB_SIZE 13
int op_trans_tb[TB_SIZE][26];

void trans_tb_init() {
  for (int i = 0; i < TB_SIZE; ++i)
    for (int j = 0; j < 26; ++j)
      op_trans_tb[i][j] = NA;
  for (int i = 0; i < 26; ++i) op_trans_tb[10][i] = BR;
  op_trans_tb[0][0] = 3;   // A
  op_trans_tb[0][1] = 10;  // B
  op_trans_tb[0][2] = 7;   // C
  op_trans_tb[0][4] = 11;  // E
  op_trans_tb[0][11] = 2;  // L
  op_trans_tb[0][12] = 5;  // M
  op_trans_tb[0][13] = 9;  // N
  op_trans_tb[0][14] = 6;  // O
  op_trans_tb[0][17] = 8;  // R
  op_trans_tb[0][18] = 1;  // S
  op_trans_tb[0][19] = AR; // T
  op_trans_tb[1][19] = ST; // ST
  op_trans_tb[1][20] = AR; // SU
  op_trans_tb[2][3] = LD;  // LD
  op_trans_tb[2][18] = AR; // LS
  op_trans_tb[3][3] = 4;   // AD
  op_trans_tb[3][13] = AR; // AN
  op_trans_tb[4][3] = AR;  // ADD
  op_trans_tb[5][14] = op_trans_tb[5][20] = op_trans_tb[5][21] = AR; // MO, MU, MV
  op_trans_tb[5][17] = LD; // MR
  op_trans_tb[5][18] = ST; // MS
  op_trans_tb[6][17] = AR; // OR
  op_trans_tb[7][1] = BR;  // CB
  op_trans_tb[7][2] = 7;   // CC
  op_trans_tb[7][12] = op_trans_tb[7][18] = op_trans_tb[7][11] = AR; // CM, CS, CL
  op_trans_tb[8][4] = 12;  // RE
  op_trans_tb[9][4] = AR;  // NE
  op_trans_tb[10][8] = AR; // BI
  op_trans_tb[11][14] = AR; // EO 
  op_trans_tb[12][19] = BR;  // RET
  op_trans_tb[12][21] = AR;  // REV
}

int findtype(char* op) {
  int state = 0;
  while (*op != '\0') {
    state = op_trans_tb[state][*(op)++ - 'a'];
    if (state >= NA) return state; // accept state
    if ((*(op) == '.' || *(op) == '\0') && state == 10) return BR;
  }
  return NA;
}

char** arg_parse(char* arg_str, int* argc) {
  char** arg_list = malloc(sizeof(char*) * 6);
  int count = 0;
  for (int i = 0; i < strlen(arg_str); ++i) {
    int arg_len = 0;
    char* arg_cur = calloc(20, sizeof(char));

    while (arg_str[i] == ' ' || arg_str[i] == '\t') ++i; // consume white spaces
    if (arg_str[i] == '<' || arg_str[i] == '/') break; // end condition

    if (arg_str[i] == '[') { // argument set condition
      while (arg_str[i] && arg_str[i] != ']') arg_cur[arg_len++] = arg_str[i++];
      arg_cur[arg_len++] = ']';
      i++;
      if (arg_str[i] && arg_str[i] == '!') {
        arg_cur[arg_len++] = '!';
        i++;
      }
      arg_cur[arg_len] = '\0';
      arg_list[count++] = arg_cur;
    } else if (arg_str[i] != '\n') { // normal argumet condition
      while (arg_str[i] && (arg_str[i] != ',' && arg_str[i] != '\n' 
            && arg_str[i] != ' ' && arg_str[i] != '<' && arg_str[i] != '/'))
        arg_cur[arg_len++] = arg_str[i++];

      arg_cur[arg_len] = '\0';
      arg_list[count++] = arg_cur;
    } 
  }
  *argc = count;
  return arg_list;
}

INS** aarch_parse(char* filename, int* ret_sz) {
  FILE* f;
  f = fopen(filename, "r");
  if (f == NULL) {
    printf("%s file cannot be opened, skipped...", filename);
    return NULL;
  }

  char* line = malloc(sizeof(char) * 128);
  int ins_count = -1;
  int ret_len = 16;
  INS** ret_ins = malloc(sizeof(INS*) * ret_len);

  trans_tb_init();

  while (fgets(line, 128, f) != NULL) {
    if (ins_count == -1) {
      ins_count++;
      continue; // label name 
    }
    INS* cur_ins = malloc(sizeof(INS));
    int arg_start = 0;
    cur_ins->op = malloc(sizeof(char) * 9);

    if (!sscanf(line, "%x: %*s %s%n", &cur_ins->addr, cur_ins->op, &arg_start))
      continue;
    cur_ins->type = findtype(cur_ins->op);
    
    char* arg_str = calloc((strlen(line) - arg_start + 1), sizeof(char));
    strncpy(arg_str, &line[arg_start], strlen(line) - arg_start + 1);
    cur_ins->argv = arg_parse(arg_str, &(cur_ins->argc));

    // find lbl_name
    cur_ins->lbl_name = NULL;
    for (int i = 0; i < strlen(line); ++i) {
      if (line[i] == '<') { // find
        cur_ins->lbl_name = calloc((strlen(line) - i + 1), sizeof(char));
        int lbl_start = i++;
        while (line[i] != '>') { // copy lbl_name
          (cur_ins->lbl_name)[i - lbl_start - 1] = line[i]; 
          i++;
        }
      }
    }
      
    ret_ins[ins_count++] = cur_ins;
    if (ins_count == ret_len - 1) { // dynamicall allocate
      ret_len *= 2;
      INS** tmp = realloc(ret_ins, sizeof(INS*) * ret_len);
      if (tmp == NULL) {
        puts("realloc failed, following instructions skipped");
        *ret_sz = ins_count;
        return ret_ins;
      }
      ret_ins = tmp;
    }
  }

  // return unused space
  ret_ins = realloc(ret_ins, sizeof(INS*) * ins_count);

  fclose(f);
  *ret_sz = ins_count;  
  return ret_ins;
}
