#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char** split_routine(char* filename, int type, int* ret_sz) {
  char** subnames = malloc(sizeof(char*) * 250);
  *ret_sz = 0;

  FILE* f = fopen(filename, "r");
  if (f == NULL) {
    printf("file %s cannot open. exit program...\n", filename);
    exit(-1);
  }

  char* line = calloc(128, sizeof(char));
  char* name = calloc(50, sizeof(char));
  int addr, addrlen;
  FILE* fo; // output file
  int subroutine_start = 0;

  while ((fgets(line, 128, f) != NULL)) {
    // read line by line
    if (!subroutine_start) {
      if (sscanf(line, "%x%n <%[^>^:]>:\n", &addr, &addrlen, name) == 2
          && addrlen == 16) { // find first subroutine
        if (name[0] == '_' || name[0] == '.') continue;
        subroutine_start = 1;

        // set up file name
        char* tmp = strdup(name);
        sprintf(name, "subroutines/%s.%s.o", tmp, (type)? "aarch" : "riscv");
        printf("[Spliting]\t%s\n",name);
        subnames[(*ret_sz)] = calloc(strlen(name) + 1, sizeof(char));
        strncpy(subnames[(*ret_sz)++], name, strlen(name) + 1);
        fo = fopen(name, "w+");
        fprintf(fo, "%s", line);
        free(tmp);
      }
    } else {
      if (line[0] == '\n') {
        if (fgets(line, 128, f) != NULL 
            && sscanf(line, "%x%n <%[^>^:]>:\n", &addr, &addrlen, name) == 2
            && addrlen == 16) { // next subroutine
          fclose(fo);
          fo = NULL;
          if (name[0] == '_' || name[0] == '.') {
            subroutine_start = 0;
            continue;
          }

          // set up file name
          char* tmp = strdup(name);
          sprintf(name, "subroutines/%s.%s.o", tmp, (type)? "aarch" : "riscv");
          printf("[Spliting]\t%s\n",name);
          subnames[(*ret_sz)] = calloc(strlen(name) + 1, sizeof(char));
          strncpy(subnames[(*ret_sz)++], name, strlen(name) + 1);
          fo = fopen(name, "w+");
          fprintf(fo, "%s", line);
          free(tmp);
        } else { // source code, continue
           fprintf(fo, "\n%s", line);
        }
      } else { // inside subroutine, continue
        fprintf(fo, "%s", line);
      }
    }
  }
  free(line);
  free(name);
  char** tmp = realloc(subnames, sizeof(char*) * *ret_sz);
  if (tmp) subnames = tmp;
  if (fo) fclose(fo);
  if (f) fclose(f);
  return subnames;
}
