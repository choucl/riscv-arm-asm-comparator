#include "core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define BUF_SZ 128
#define MAX_ARGC 8

static char *p;           // current position in the file
static char *tk;          // point to current token
static char buf[BUF_SZ];  // buffer to store the content of a line

static FILE *fp;

static int trans[4][26] = {
  /* a,  b,  c,  d,  e,  f,  g,  h,  i,  j,  k,  l,  m,  n,  o,  p,  q,  r,  s,
     t,  u,  v,  w,  x,  y,  z */
  { AR, BR, BR, AR, BR, AR, NA, NA, NA, BR, NA,  2, AR, AR, AR, NA, NA,  1,  3,
    BR, NA, NA, NA, AR, NA, NA },
  { NA, NA, NA, NA,  1, NA, NA, NA, NA, NA, NA, NA, AR, NA, NA, NA, NA, NA, NA,
    BR, NA, NA, NA, NA, NA, NA },
  { NA, LD, NA, LD, NA, NA, NA, LD, AR, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
    NA, AR, NA, LD, NA, NA, NA },
  { NA, ST, NA, ST, AR, NA, AR, ST, NA, NA, NA, AR, NA, AR, NA, NA, NA, AR, NA,
    NA, AR, NA, ST, NA, NA, NA }
};

static int findtype(char *op);

static void next();
static int title();
static int ins(INS *i);
static int arg_list(INS *i);
static int arg(INS *i);
static int more_args(INS *i);
static int lbl(char **name);
static int lbl_name(char **name);

/* Function that matches terminals. Return 1 when match successfully, 0
 * otherwise. The related value is passed out via parameter of reference. When
 * matching successfully, it call next() to get to the next token for further 
 * parsing.
 */
static int hex(int *val);
static int astring(char **str);
static int match(char c);

static int findtype(char *op) {
  int type = 0;

  while (*op && type < NA) {
    type = trans[type][*op - 'a'];
    ++op;
  }

  return type;
}

static void next() {
  while (*p == ' ' || *p == '\t')
    ++p;

  /* If it comes accross '#', which means the rest of the line are just comment,
   * read until it reach the '\n' symbol
   */
  if (*p == '#')
    while (*p != '\n')
      ++p;  
  tk = p;
}

static int title() {
  int val;
  char *name = NULL;

  if (hex(&val) && lbl_name(&name) && match(':') && match('\n')) {
     printf("Parsing routine: %s @0x%x\n", name, val);
     free(name);
     return 1;
  }
  return 0;
}

static int ins(INS *i) {
  int addr;
  char *op = NULL;

  if (hex(&addr) && match(':') && hex(NULL) && astring(&op) && arg_list(i) && 
      match('\n')) {
    i->addr = addr;
    i->op = op;
    i->type = findtype(op);
    i->is_leader = 0;
    char **tmp = realloc(i->argv, i->argc * sizeof(char *));  // shrink
    if (tmp)
      i->argv = tmp;
    return 1;
  }
  return 0;
}

static int arg_list(INS *i) {
  i->argc = 0;
  i->lbl_name = NULL;

  if (*tk == '\n') {
    i->argv = NULL;
    return 1;
  } else {
    i->argv = malloc(MAX_ARGC * sizeof(char *));  // argv not empty for i
    return arg(i) && more_args(i);
  }
}

static int arg(INS *i) {
  char *arg_str = NULL, *name = NULL;

  if (astring(&arg_str) && lbl(&name)) {
    if (i->argc < MAX_ARGC)
      i->argv[(i->argc)++] = arg_str;
    i->lbl_name = name;
    return 1;
  }
  free(i->argv);
  return 0;
}

static int more_args(INS *i) {
  if (*tk == ',') {
    return match(',') && arg(i) && more_args(i);
  } else if (*tk == '\n') {
    return 1;
  }
  free(i->argv);
  return 0;
}

static int lbl(char **name) {
  if (*tk == '<') {
    return lbl_name(name);
  } else if (*tk == '\n' || *tk == ',') {
    return 1;
  }
  return 0;
}

static int hex(int *val) {
  int tmp = 0;

  while (*p) {
    if (*p >= '0' && *p <= '9')
      tmp = tmp * 16 + (*p - '0');
    else if (*p >= 'a' && *p <= 'f')
      tmp = tmp * 16 + (*p - 'a' + 10);  // 0xa is 10 in dec
    else
      break;
    ++p;
  }

  if (p == tk)  // Did not parse anything
    return 0;

  if (val)
    *val = tmp;
  next();
  return 1;
}

static int lbl_name(char **name) {
  return match('<') && astring(name) && match('>');
}

static int astring(char **str) {
  // Find the string w/o delimiter
  while (*p > ' ' && *p != '<' && *p != '>' && *p != ',' && *p != ':')
    ++p;

  int len = p - tk;
  if (!len) {
    if (str)
      *str = NULL;
    return 0;
  }

  if (str) {
    *str = calloc(len + 1, sizeof(char));
    strncpy(*str, tk, len);
  }
  next();
  return 1;
}

static int match(char c) {
  if (*tk != c)
    return 0;
  ++p;
  next();
  return 1;
}

INS** riscv_parse(char* filename, int *ret_sz) {
  fp = fopen(filename, "r");

  if (!fp) {
    *ret_sz = 0;
    return NULL;
  }

  fgets(buf, BUF_SZ, fp);
  p = buf;
  next();

  if(!title()) {
    printf("File: %s contains no title\n", filename);
  }

  int max_ret_sz = 32, ins_idx = 0;
  INS **ret_ins = malloc(max_ret_sz * sizeof(INS *));
  while (fgets(buf, BUF_SZ, fp)) {
    p = buf;
    next();

    INS *i = malloc(sizeof(INS));

    /* Start parsing an instruction from the non-terminal Ins. If the line does
     * not derive a Ins, then skip it.
     */
    if (!ins(i)) {
      free(i);
      continue;
    }

    /* The ret_ins is implemented using dynamic tables. When it if full, we
     * reallocate it with doubled size.
     */
    if (ins_idx == max_ret_sz) {
      max_ret_sz *= 2;
      INS **tmp = realloc(ret_ins, max_ret_sz * sizeof(INS *));
      if (!tmp) {
        printf("Failed reallocating memory for size: %d\n", max_ret_sz);
        break;  // If failed, just return what we have got so far
      }
      ret_ins = tmp;
    }
    ret_ins[ins_idx++] = i;
  }

  *ret_sz = ins_idx;
  INS **tmp = realloc(ret_ins, *ret_sz * sizeof(INS));  // shrink
  if (tmp)
    ret_ins = tmp;
  fclose(fp);
  return ret_ins;
}
