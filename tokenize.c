#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "scc.h"

// Buffer for tokenized result.
// Assumption: the max number of tokens are 100.
Vector *tokens;

// Index of tokens
int pos;

Token *new_token(int ty, char *input);
Token *new_token_num(char *input, int val);

void tokenize(char *p) {
  tokens = new_vector();

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    // "=="
    if(*p == '=' && *(p+1) == '='){
      vec_push(tokens, new_token(TK_EQUAL, p));
      p = p + 2;
      continue;
    }

    // "!="
    if(*p == '!' && *(p+1) == '='){
      vec_push(tokens, new_token(TK_NOT_EQUAL, p));
      p = p + 2;
      continue;
    }

    // >=
    if(*p == '>' && *(p+1) == '='){
      vec_push(tokens, new_token(TK_GREATER_EQUAL, p));
      p = p+2;
      continue;
    }

    // <=
    if(*p == '<' && *(p+1) == '='){
      vec_push(tokens, new_token(TK_LESS_EQUAL, p));
      p = p+2;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '=' || *p == ';' || *p == '<' || *p == '>' ||
        *p == '&' || *p == '^' || *p == '|') {
      vec_push(tokens, new_token(*p, p));
      p++;
      continue;
    }

    if (isdigit(*p)) {
      vec_push(tokens, new_token_num(p, strtol(p, &p, 10)));
      continue;
    }

    if('a' <= *p && *p <= 'z'){
      vec_push(tokens, new_token(TK_IDENT, p));
      p++;
      continue;
    }

    fprintf(stderr, "Cannot tokenize: %s\n", p);
    exit(1);
  }

  vec_push(tokens, new_token(TK_EOF, p));
}

Token *new_token(int ty, char *input){
  Token *token = calloc(1, sizeof(Token));
  token->ty = ty;
  token->input = input;
  return token;
}

Token *new_token_num(char *input, int val){
  Token *token = calloc(1, sizeof(Token));
  token->ty = TK_NUM;
  token->input = input;
  token->val = val;
  return token;
}
