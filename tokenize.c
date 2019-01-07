#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "scc.h"

// Buffer for tokenized result.
// Assumption: the max number of tokens are 100.
Token tokens[100];

// Index of tokens
int pos;

void tokenize(char *p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    // "=="
    if(*p == '=' && *(p+1) == '='){
      //this is "=="
      tokens[i].ty = TK_EQUAL;
      tokens[i].input = p;
      i++;
      p = p + 2;
      continue;
    }

    // "!="
    if(*p == '!' && *(p+1) == '='){
      tokens[i].ty = TK_NOT_EQUAL;
      tokens[i].input = p;
      i++;
      p = p + 2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '=' || *p == ';') {
      tokens[i].ty = *p;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    if (isdigit(*p)) {
      tokens[i].ty = TK_NUM;
      tokens[i].input = p;
      tokens[i].val = strtol(p, &p, 10);
      i++;
      continue;
    }

    if('a' <= *p && *p <= 'z'){
      tokens[i].ty = TK_IDENT;
      tokens[i].input = p;
      i++;
      p++;
      continue;
    }

    fprintf(stderr, "Cannot tokenize: %s\n", p);
    exit(1);
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}
