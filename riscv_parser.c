#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SZ 128

char *p;           // current position in the file
char *tk;          // point to current token
char buf[BUF_SZ];  // buffer to store the content of a line

FILE *fp;

void next();
int title();
int ins(INS **i);
int arg_list();
int arg();
int more_args();
int lbl();
int lbl_name(char **name);


/*
 * Function that matches terminals. Return 1 when match successfully, 0
 * otherwise. The related value is passed out via parameter of reference. When
 * matching successfully, it call next() to get to the next token for further 
 * parsing.
 */
int hex(int *val);
int astring(char **str);
int match(char c);

void next() {
  while (*p == ' ' || *p == '\t')
    ++p;

  /*
   * If it comes accross '#', which means the rest of the line are just comment,
   * read until it reach the '\n' symbol
   */
  if (*p == '#')
    while (*p != '\n')
      ++p;  
  tk = p;
}

int title() {
  int val;
  char *name;

  return hex(&val) && lbl_name(&name) && match(':') && match('\n') &&
         printf("Parsing routine: %s @0x%x\n", name, val);
}

int ins(INS **i) {
  int addr, argc, mc;
  char *op, *name, **argv;

  // TODO: Use the smart format if possible
  if (hex(&addr) && match(':') && hex(&mc) && astring(&op) && arg_list() && 
      match('\n')) {
    printf("ins: %x, %x, %s\n", addr, mc, op);
    return 1;
  }

  return 0;
}

int arg_list() {
  if (*tk == '\n') {
    return 1;
  } else {
    return arg() && more_args();
  }
  return 0;
}

int arg() {
  char *arg_str;
  return astring(&arg_str) && puts(arg_str) && lbl();
}

int more_args() {
  if (*tk == ',') {
    return match(',') && arg() && more_args();
  } else if (*tk == '\n') {
    return 1;
  } else {
    puts("Parse error");
    return 0;
  }
}

int lbl() {
  char *name;
  if (*tk == '<') {
    return lbl_name(&name);
  } else if (*tk == '\n' || *tk == ',') {
    return 1;
  } else {
    puts("Parse error");
    return 0;
  }
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

  if (p == tk)
    return 0;
  next();
  return 1;
}

int lbl_name(char **name) {
  return match('<') && astring(name) && match('>');
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
    printf("|%s", buf);
    p = buf;
    next();
    INS *i;
    if (ins(&i)) {
      puts("==========");
    }
    // assemble i
  }

  return NULL;
}
