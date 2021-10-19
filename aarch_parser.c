#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define  TB_SIZE 13
int op_trans_tb[TB_SIZE][26] = {
{ 3, 10,  7, NA, 11, NA, NA, NA, NA, NA, NA,  2,  5,  9,  6, NA, NA,  8,  1, AR, NA, NA, NA, NA, NA, NA},
{NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, ST, AR, NA, NA, NA, NA, NA},
{NA, NA, NA, LD, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, AR, NA, NA, NA, NA, NA, NA, NA},
{NA, NA, NA,  4, NA, NA, NA, NA, NA, NA, NA, NA, NA, AR, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA},
{NA, NA, NA, AR, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA},
{NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, AR, NA, NA, LD, ST, NA, AR, AR, NA, NA, NA, NA},
{NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, AR, NA, NA, NA, NA, NA, NA, NA, NA},
{NA, BR,  7, NA, NA, NA, NA, NA, NA, NA, NA, AR, AR, NA, NA, NA, NA, NA, AR, NA, NA, NA, NA, NA, NA, NA},
{NA, NA, NA, NA, 12, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA},
{NA, NA, NA, NA, AR, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA},
{BR, BR, BR, BR, BR, BR, BR, BR, AR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR, BR},
{NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, AR, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA},
{NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, BR, NA, AR, NA, NA, NA, NA},
};

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
