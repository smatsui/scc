#include <stdlib.h>
#include "scc.h"

// Buffer for parsed node.
Node *code[100];

Node *assign();
Node *expr();
Node *mul();
Node *term();


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

Node *term() {
  if (tokens[pos].ty == TK_NUM) {
    return new_node_num(tokens[pos++].val);
  }
  if (tokens[pos].ty == TK_IDENT) {
    return new_node_ident(tokens[pos++].input[0]);
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
  return 0;
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
    return node;
  }
  if (tokens[pos].ty != ';') {
    error("semicolon not found: %s", tokens[pos].input);
  }
  pos++;
  return lhs;
}

void program() {
  int i = 0;
  while(tokens[pos].ty != TK_EOF){
    code[i] = assign();
    i++;
  }
  code[i] = NULL;
}
