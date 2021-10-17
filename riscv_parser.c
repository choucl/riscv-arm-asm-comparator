#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SZ 128

int tk_len;

char *p;           // current position in the file
char *tk;          // point to current token
char buf[BUF_SZ];  // buffer to store the content of a line

FILE *fp;

void next();
void title();
int hex();
char *lbl_name();
INS *ins();
void op();
void arg_list();
void arg();
char *astring();
int match(char c);

void next() {
  while (*p == ' ' || *p == '\t')
    ++p;

  /*
   * Check if it reach the end of the line, if true, read a new line and
   * continue.
   */
  if (!*p) {
    fgets(buf, BUF_SZ, fp);
    p = buf;
    next();
  } else {
    tk = p;
    printf("tk: '%c'\n", *tk);
  }
}

void title() {
  int val = hex();
  if (!tk_len) return;  // Failed reading a hex
  next();

  char *name = lbl_name();
  if (!name) return;
  next();

  if (!match(':')) return;
  next();

  if (!match('\n')) return;
  next();

  printf("%d with name %s\n", val, name);
}

int hex() {
  int val = 0; 

  while (*p) {
    if (*p >= '0' && *p <= '9')
      val = val * 16 + (*p - '0');
    else if (*p >= 'a' && *p <= 'f')
      val = val * 16 + (*p - 'a' + 10);  // 0xa is 10 in dec
    else
      break;

    ++p;
  }

  tk_len = p - tk;
  return val;
}

char *lbl_name() {
  char *name;

  if (!match('<')) return NULL;
  next();

  name = astring();
  if (!name) return NULL;
  next();

  if (!match('>')) return NULL;

  return name;
}

char *astring() {
  // Find the string w/o delimiter
  char *s;

  while (*p > ' ' && *p != '<' && *p != '>' && *p != ',' && *p != ':')
    ++p;

  int len = p - tk;
  s = malloc(len + 1);
  strncpy(s, tk, len);

  return s;
}

int match(char c) {
  if (*tk != c) return 0;
  ++p;
  return 1;
}

INS* riscv_parse(char* filename, int *ret_sz) {
  fp = fopen(filename, "r");

  if (!fp)
    return NULL;

  fgets(buf, BUF_SZ, fp);
  p = buf;
  next();

  title();

  /*
  while (fgets(buf, BUF_SZ, fp)) {
    INS *i = ins();
    if (!i) continue;

    assemble i
  }
  */

  return NULL;
}
