#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** split_subroutine(char* filename, int type, int* ret_sz) {
  char** subnames = malloc(sizeof(char*) * 200);
  *ret_sz = 0;

  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    puts("file cannot open. exit program...");
    exit(-1);
  }

  char* line = malloc(sizeof(char) * 128);
  char* name = malloc(sizeof(char) * 50);
  int address;
  FILE* fo; // output file
  int subroutine_start = 0;


  while ((fgets(line, 128, f) != NULL)) {
    // read line by line
    if (!subroutine_start) {
      if (sscanf(line, "%x <%[^>^:]>:\n", &address, name) == 2) { // find first subroutine
        if (name[0] == '_') continue;
        subroutine_start = 1;
        strncat(name, ".o", 3);
        printf("%s\n", name);
        subnames[(*ret_sz)] = malloc(sizeof(char) * 128);
        strncpy(subnames[(*ret_sz)], name, strlen(name) + 1);
        fo = fopen(name, "w+");
        fprintf(fo, "%s", line);
        (*ret_sz)++;
      }
    } else {
      if (line[0] == '\n') {
        if (fgets(line, 128, f) != NULL 
            && sscanf(line, "%x <%[^>^:]>:\n", &address, name) == 2) { // next subroutine
          fclose(fo);
          if (name[0] == '_') {
            subroutine_start = 0;
            continue;
          }
          subroutine_start = 1;
          strncat(name, ".o", 3);
          subnames[(*ret_sz)] = malloc(sizeof(char) * 128);
          strncpy(subnames[(*ret_sz)], name, strlen(name) + 1);
          fo = fopen(name, "w+");
          fprintf(fo, "%s", line);
          (*ret_sz)++;
        } else { // source code, continue
           fprintf(fo, "\n%s", line);
        }
      } else {
        fprintf(fo, "%s", line);
      }
    }
  }
  fclose(fo);
  fclose(f);
  return subnames;
}
