#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** arg_parse(char* arg_str, int* argc) {
  char** arg_list = malloc(sizeof(char*) * 6);
  int count = 0;
  for (int i = 0; i < strlen(arg_str); ++i) {
    int arg_len = 0;
    char* arg_cur = malloc(sizeof(char) * 16);
    while (arg_str[i] == ' ' || arg_str[i] == '\t') ++i; // consume white spaces
    if (arg_str[i] == '<' || arg_str[i] == '/') break;
    if (arg_str[i] == '[') {
      while(arg_str[i] && arg_str[i] != ']') arg_cur[arg_len++] = arg_str[i++];
      arg_cur[arg_len++] = ']';
      i++;
      if (arg_str[i] && arg_str[i] == '!') {
        arg_cur[arg_len++] = '!';
        i++;
      }
    } else {
      while(arg_str[i] && (arg_str[i] != ',' && arg_str[i] != '\n' 
            && arg_str[i] != ' ' && arg_str[i] != '<' && arg_str[i] != '/'))
        arg_cur[arg_len++] = arg_str[i++];
    } 
    arg_cur[arg_len] = '\0';
    arg_list[count++] = arg_cur;
  }
  *argc = count;
  return arg_list;
}

INS* aarch_parse(char* filename, int* ret_sz) {
  FILE* f;
  f = fopen(filename, "r");
  if (f == NULL) {
    printf("%s file cannot be opened, skipped...", filename);
    return NULL;
  }

  char* line = malloc(sizeof(char) * 128);
  int ins_count = -1;
  int retlen = 100;
  INS* retins = malloc(sizeof(INS) * retlen);

  while (fgets(line, 128, f) != NULL) {
    if (ins_count == -1) {
      ins_count++;
      continue; // label name 
    }
    INS curins;
    int arg_start = 0;
    curins.op = malloc(sizeof(char) * 9);

    if (!sscanf(line, "%x: %*s %s%n", &curins.addr, curins.op, &arg_start))
      continue;
    
    char* arg_str = malloc(sizeof(char) * (strlen(line) - arg_start + 1));
    strncpy(arg_str, &line[arg_start], strlen(line) - arg_start + 1);
    curins.argv = arg_parse(arg_str, &(curins.argc));
      
    printf("%d\n", ins_count);
    printf("%d %s\n", curins.addr, curins.op);
    retins[ins_count++] = curins;
    if (ins_count == retlen - 1) {
      retlen *= 2;
      retins = realloc(retins, sizeof(INS) * retlen);
      if (retins == NULL) 
        puts("realloc failed");
    }
  }

  // return unused space
  retins = realloc(retins, sizeof(INS) * ins_count);

  fclose(f);
  *ret_sz = ins_count;  
  return retins;
}
