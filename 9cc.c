#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

// Token type
enum {
  TK_NUM = 256, //Number token
  TK_EOF,
};

typedef struct {
  int ty;       //token type
  int val;      //value for TK_NUM type
  char *input;  //for error message
} Token;

enum {
  ND_NUM = 256,
};

typedef struct Node {
  int ty;
  struct Node *lhs;
  struct Node *rhs;
  int val;
} Node;

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

    if (*p == '+' || *p == '-' || *p == '*' || *p == '/') {
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

    fprintf(stderr, "Cannot tokenize: %s\n", p);
    exit(1);
  }

  tokens[i].ty = TK_EOF;
  tokens[i].input = p;
}

void error(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(stderr, format, args);
  va_end(args);
  exit(1);
}

Node *new_node(int ty, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->ty = ty;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->ty = ND_NUM;
  node->val = val;
  return node;
}

void print_tree(Node* node){
  if(node == NULL){
    return;
  }
  printf("  ");
  print_tree(node->lhs);
  if(node->ty == ND_NUM){
    printf("%d\n", node->val);
  }else{
    printf("  ");
    printf("%c\n", (char)node->ty);
  }
  print_tree(node->rhs);
}

Node *term(){
  if (tokens[pos].ty == TK_NUM) {
    return new_node_num(tokens[pos++].val);
  }
  error("not a number or a left parenthesis: %s", tokens[pos].input);
}

Node *mul(){
  Node *lhs = term();
  if(tokens[pos].ty == '*'){
    pos++;
    return new_node('*', lhs, mul());
  }
  if(tokens[pos].ty == '/'){
    pos++;
    return new_node('/', lhs, mul());
  }
  return lhs;
}

Node *expr(){
  Node *lhs = mul();
  if (tokens[pos].ty == '+'){
    pos++;
    return new_node('+', lhs, expr());
  }
  if (tokens[pos].ty == '-'){
    pos++;
    return new_node('-', lhs, expr());
  }
  return lhs;
}

void gen(Node *node){
  if(node->ty == ND_NUM){
    printf("  push %d\n", node->val);
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch(node->ty){
  case '+':
    printf("  add rax, rdi\n");
    break;
  case '-':
    printf("  sub rax, rdi\n");
    break;
  case '*':
    printf("  mul rdi\n");
    break;
  case '/':
    printf("  mov rdx, 0\n");
    printf("  div rdi\n");
    break;
  }

  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "Invalid number of arguments\n");
    return 1;
  }

  tokenize(argv[1]);
  Node *node = expr();

  printf(".intel_syntax noprefix\n");
  printf(".global _main\n");
  printf("_main:\n");

  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");

  return 0;
}
