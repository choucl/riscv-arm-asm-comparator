#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#define BUF_SZ 128

int tk_len;

char *p;           // current position in the file
char *tk;          // point to current token
char buf[BUF_SZ];  // buffer to store the content of a line

FILE *fp;

void next();
void title();
void ins_list();
int hex();
int lbl_name();
void ins();
void op();
void arg_list();
void arg();
void astring();
void flush();

void next() {
  while (*p == ' ' || *p == '\t')
    ++p;

  /*
   * Check if it reach the end of the line, if true, read a new line and
   * continue.
   */
  if (*p == '\n' || *p == '\0') {
    fgets(buf, BUF_SZ, fp);
    p = buf;
    next();
  } else {
    tk = p;
  }
}

void title() {

  int val = hex();
  next();


}

int hex() {
  int val = 0; 

  /*
   * Check if the upcoming character is a valid digit of a hex number, if not 
   * return -1 to indicate invalidation.
   */
  if (!(*p > '0' && *p < '9' || *p > 'a' && *p < 'f'))
    return -1;
  
  do {
    if (*p > '0' && *p < '9')
      val = val * 16 + (*p - '0');
    else if (*p > 'a' && *p < 'f')
      val = val * 16 + (*p - 'a' + 10);  // 0xa is 10 in dec
    else
      break;
    ++p;
  } while (*p);

  tk_len = p - tk;
  return val;
}

int lbl_name() {
  if (*tk != '<') return -1;
  next();
  astring();
  if (!tk_len) return -1;
  next();
  if (*tk != '>') return -1;
  return 0;
}

void ins_list() {

}

void astring() {
  // Find the string w/o delimiter
  while (*p > ' ' && *p != '<' && *p != '>' && *p != ',' && *p != ':') {
    ++p;
  }
  tk_len = p - tk;
}

INS* riscv_parse(char* filename, int *ret_sz) {
  fp = fopen(filename, "r");

  if (!fp)
    return NULL;

  next();
  title();

  return NULL;
}
