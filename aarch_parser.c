#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int op_trans_tb[13][26] = {{-1}};

void trans_tb_init() {
  for (int i = 0; i < 13; ++i)
    for (int j = 0; j < 26; ++j)
      op_trans_tb[i][j] = -1;
  op_trans_tb[0][0] = 7;    // A
  op_trans_tb[0][1] = 1;    // B
  op_trans_tb[0][2] = 11;   // C
  op_trans_tb[0][11] = 4;   // L
  op_trans_tb[0][12] = 9;   // M
  op_trans_tb[0][14] = 10;  // O
  op_trans_tb[0][18] = 2;   // S
  op_trans_tb[2][19] = 3;   // ST
  op_trans_tb[2][20] = 6;   // SU
  op_trans_tb[4][3] = 5;    // LD
  op_trans_tb[4][18] = 6;   // LS
  op_trans_tb[7][3] = 8;    // AD
  op_trans_tb[7][13] = 6;   // AN
  op_trans_tb[8][3] = 6;    // ADD
  op_trans_tb[9][14] = op_trans_tb[9][20] = 6; // MO, MU
  op_trans_tb[10][17] = 6;  // OR
  op_trans_tb[11][2] = 11;  // CC
  op_trans_tb[11][12] = 12; // CM
  op_trans_tb[12][15] = 6;  // CMP

  for (int i = 0; i < 26; ++i) {
    op_trans_tb[1][i] = 1; // BR
    op_trans_tb[3][i] = 3; // ST
    op_trans_tb[5][i] = 5; // LD
    op_trans_tb[6][i] = 6; // AR
  }
}

int findtype(char* op) {
  int i = 0;
  int state = 0;
  while (op[i] != '\0' && state != -1) {
    state = op_trans_tb[state][op[i++] - 'a'];
    if (state == 1) return BR;
    if (state == 3) return ST;
    if (state == 5) return LD;
    if (state == 6) return AR;
  }
  return OW;
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
