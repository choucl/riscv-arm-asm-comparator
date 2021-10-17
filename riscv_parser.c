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

int title();
INS *ins();
int arg_list();
int lbl();
int lbl_name(char **name);


/*
 * Function that matches terminals. Return 1 when match successfully, 0
 * otherwise. The related value is passed out via parameter of reference
 */
int hex(int *val);
int astring(char **str);
int match(char c);
void next();

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

int title() {
  int val;
  char *name;

  if (!hex(&val))
    return 0;
  if (!lbl_name(&name))
    return 0;
  if (!match(':'))
    return 0;
  if (!match('\n'))
    return 0;
  printf("%d with name %s\n", val, name);
  return 1;
}

INS *ins() {
  int addr, argc;
  char *op, *name, **argv;
  if (!hex(&addr))
    return NULL;
  if (!match(':'))
    return NULL;
  if (!hex(NULL))
    return NULL;
  if (!astring(&op))
    return NULL;
  if (!arg_list())
    return NULL;
  if (!lbl())
    return NULL;
  return NULL;
}

int hex(int *val) {
  *val = 0;

  while (*p) {
    if (*p >= '0' && *p <= '9')
      *val = *val * 16 + (*p - '0');
    else if (*p >= 'a' && *p <= 'f')
      *val = *val * 16 + (*p - 'a' + 10);  // 0xa is 10 in dec
    else
      break;

    ++p;
  }

  if (p == tk) return 0;

  next();
  return 1;
}

int lbl_name(char **name) {
  if (!match('<'))
    return 0;
  if (!astring(name))
    return 0;
  if (!match('>'))
    return 0;
  return 1;
}

int astring(char **str) {
  // Find the string w/o delimiter
  char *s;

  while (*p > ' ' && *p != '<' && *p != '>' && *p != ',' && *p != ':')
    ++p;

  int len = p - tk;
  if (!len) {
    *str = NULL;
    return 0;
  }
  *str = malloc(len + 1);
  strncpy(*str, tk, len);
  next();
  return 1;
}

int match(char c) {
  if (*tk != c)
    return 0;
  ++p;
  next();
  return 1;
}

INS* riscv_parse(char* filename, int *ret_sz) {
  fp = fopen(filename, "r");

  if (!fp)
    return NULL;

  fgets(buf, BUF_SZ, fp);
  p = buf;
  next();

  if(!title()) {
    printf("File: %s contains no title\n", filename);
  }

  while (fgets(buf, BUF_SZ, fp)) {
    INS *i = ins();
    if (!i) continue;

    // assemble i
  }

  return NULL;
}
