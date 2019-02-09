#include "scc.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Buffer for tokenized result.
// Assumption: the max number of tokens are 100.
Vector *tokens;

// Index of tokens
int pos;

Token *new_token(int ty, char *input);
Token *new_token_num(char *input, int val);
Token *new_token_ident(char *input, char *name, int length);
int consume(int ty);

void tokenize(char *p) {
  tokens = new_vector();

  while (*p) {
    if (isspace(*p)) {
      p++;
      continue;
    }

    // "=="
    if (*p == '=' && *(p + 1) == '=') {
      vec_push(tokens, new_token(TK_EQUAL, p));
      p = p + 2;
      continue;
    }

    // "!="
    if (*p == '!' && *(p + 1) == '=') {
      vec_push(tokens, new_token(TK_NOT_EQUAL, p));
      p = p + 2;
      continue;
    }

    // >=
    if (*p == '>' && *(p + 1) == '=') {
      vec_push(tokens, new_token(TK_GREATER_EQUAL, p));
      p = p + 2;
      continue;
    }

    // <=
    if (*p == '<' && *(p + 1) == '=') {
      vec_push(tokens, new_token(TK_LESS_EQUAL, p));
      p = p + 2;
      continue;
    }

    // &&
    if (*p == '&' && *(p + 1) == '&') {
      vec_push(tokens, new_token(TK_AND, p));
      p = p + 2;
      continue;
    }

    // ||
    if (*p == '|' && *(p + 1) == '|') {
      vec_push(tokens, new_token(TK_OR, p));
      p = p + 2;
      continue;
    }

    if (memcmp(p, "if", 2) == 0){
      vec_push(tokens, new_token(TK_IF, p));
      p = p + 2;
      continue;
    }

    if (memcmp(p, "else", 4) == 0){
      vec_push(tokens, new_token(TK_ELSE, p));
      p = p + 4;
      continue;
    }

    if (memcmp(p, "while", 5) == 0){
      vec_push(tokens, new_token(TK_WHILE, p));
      p = p + 5;
      continue;
    }

    if (memcmp(p, "for", 3) == 0){
      vec_push(tokens, new_token(TK_FOR, p));
      p = p + 3;
      continue;
    }

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' ||
        *p == ')' || *p == '=' || *p == ';' || *p == '<' || *p == '>' ||
        *p == '&' || *p == '^' || *p == '|' || *p == '{' || *p == '}' ||
        *p == ',') {
      vec_push(tokens, new_token(*p, p));
      p++;
      continue;
    }

    if (isdigit(*p)) {
      vec_push(tokens, new_token_num(p, strtol(p, &p, 10)));
      continue;
    }

    if ('a' <= *p && *p <= 'z') {
      char name[100];
      int i = 0;
      while (('a' <= *p && *p <= 'z') ||
             ('0' <= *p && *p <= '9')) {
        name[i] = *p;
        p++;
        i++;
      }
      vec_push(tokens, new_token_ident(p, name, i));
      continue;
    }

    fprintf(stderr, "Cannot tokenize: %s\n", p);
    exit(1);
  }

  vec_push(tokens, new_token(TK_EOF, p));
}

Token *new_token(int ty, char *input) {
  Token *token = calloc(1, sizeof(Token));
  token->ty = ty;
  token->input = input;
  return token;
}

Token *new_token_num(char *input, int val) {
  Token *token = calloc(1, sizeof(Token));
  token->ty = TK_NUM;
  token->input = input;
  token->val = val;
  return token;
}

Token *new_token_ident(char *input, char *name, int length) {
  Token *token = calloc(1, sizeof(Token));
  token->ty = TK_IDENT;
  token->input = input;
  memcpy(token->name, name, length);
  token->name[length] = '\0';
  return token;
}
