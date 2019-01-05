#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Token type
enum {
  TK_NUM = 256, // Number token
  TK_IDENT,     // variable
  TK_EOF,
};

typedef struct {
  int ty;      // token type
  int val;     // value for TK_NUM type
  char *input; // for error message
} Token;

enum {
  ND_NUM = 256,
  ND_IDENT,
};

typedef struct Node {
  int ty;            // operator or ND_NUM
  struct Node *lhs;  // left-hand side
  struct Node *rhs;  // right-hand side
  int val;           // for ND_NUM
  char name;         // for ND_IDENT
} Node;

// Buffer for tokenized result.
// Assumption: the max number of tokens are 100.
Token tokens[100];

// Index of tokens
int pos;

// Buffer for parsed node.
Node *code[100];

Node *program();
Node *assign();
Node *expr();
Node *mul();
Node *term();

void tokenize(char *p) {
  int i = 0;
  while (*p) {
    if (isspace(*p)) {
      p++;
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

Node *new_node_ident(char name){
  Node *node = calloc(1, sizeof(Node));
  node->ty = ND_IDENT;
  node->name = name;
  return node;
}

void print_tree(Node *node) {
  static int depth;

  if (node == NULL) {
    return;
  }

  depth++;

  print_tree(node->lhs);
  if (node->ty == ND_NUM) {
    for(int i=0; i<depth; i++){
      printf("  ");
    }
    printf("%d\n", node->val);
    depth--;
  } else {
    for(int i=0; i<depth; i++){
      printf("  ");
    }
    printf("%c\n", (char)node->ty);
  }
  print_tree(node->rhs);
  depth--;
}

Node *term() {
  if (tokens[pos].ty == TK_NUM) {
    return new_node_num(tokens[pos++].val);
  }
  if (tokens[pos].ty == TK_IDENT) {
    return new_node_ident(tokens[pos++].val);
  }
  if (tokens[pos].ty == '(') {
    pos++;
    Node *node = expr();
    if (tokens[pos].ty != ')') {
      error("there is no right parenthesis: %s", tokens[pos].input);
    }
    pos++;
    return node;
  }
  error("not a number or a left parenthesis: %s", tokens[pos].input);
}

Node *mul() {
  Node *lhs = term();
  if (tokens[pos].ty == '*') {
    pos++;
    return new_node('*', lhs, mul());
  }
  if (tokens[pos].ty == '/') {
    pos++;
    return new_node('/', lhs, mul());
  }
  return lhs;
}

Node *expr() {
  Node *lhs = mul();
  if (tokens[pos].ty == '+') {
    pos++;
    return new_node('+', lhs, expr());
  }
  if (tokens[pos].ty == '-') {
    pos++;
    return new_node('-', lhs, expr());
  }
  return lhs;
}

Node *assign() {
  Node *lhs = expr();
  if (tokens[pos].ty == '=') {
    pos++;
    Node* node = new_node('=', lhs, assign());
    if (tokens[pos].ty != ';') {
      error("semicolon not found: %s", tokens[pos].input);
    }
    pos++;
    return node;
  }
  if (tokens[pos].ty != ';') {
    error("semicolon not found: %s", tokens[pos].input);
  }
  return lhs;
}

Node *program() {
  return assign();
}

void gen_lval(Node *node){
  if (node->ty == ND_IDENT){
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n",
           ('z' - node->name + 1) * 8);
    printf("push rax\n");
    return;
  }
  error("lval is not a variable");
}

void gen(Node *node) {
  if (node->ty == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  if (node->ty == ND_IDENT) {
    gen_lval(node);
    printf("  pop rax\n");
    printf("  mov rax, [rax]\n");
    printf("  push rax\n");
    return;
  }

  if (node->ty == '=') {
    gen_lval(node->lhs);
    gen(node->rhs);

    printf("  pop rdi\n");
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
    return;
  }

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->ty) {
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
  Node* node = program();

  printf(".intel_syntax noprefix\n");
  printf(".global _main\n");
  printf("_main:\n");

  //prolog
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  gen(node);

  printf("  pop rax\n");

  // epilog
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");

  return 0;
}
