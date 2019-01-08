#include <stdlib.h>
#include <stdio.h>
#include "scc.h"

// Buffer for parsed node.
Node *code[100];

Node *assign();
Node *equ();
Node *cmp();
Node *expr();
Node *mul();
Node *term();

void print_tree(Node *node){
  if(node == NULL){
    return;
  }
  if(node->ty == ND_NUM){
    printf("%d\n", node->val);
  }else if(node->ty == ND_IDENT){
    printf("%c\n", node->name);
  }else if(node->ty == ND_EQUAL){
    printf("==\n");
  }else if(node->ty == ND_NOT_EQUAL){
    printf("!=\n");
  }else{
    printf("%c\n", (char)node->ty);
  }
  print_tree(node->lhs);
  print_tree(node->rhs);
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

Node *cmp() {
  Node *lhs = expr();
  if (tokens[pos].ty == '>') {
    pos++;
    return new_node('>', lhs, cmp());
  }
  if (tokens[pos].ty == '<') {
    pos++;
    return new_node('<', lhs, cmp());
  }
  if (tokens[pos].ty == TK_GREATER_EQUAL) {
    pos++;
    return new_node(ND_GREATER_EQUAL, lhs, cmp());
  }
  if (tokens[pos].ty == TK_LESS_EQUAL) {
    pos++;
    return new_node(ND_LESS_EQUAL, lhs, cmp());
  }
  return lhs;
}

Node *equ() {
  Node *lhs = cmp();
  if (tokens[pos].ty == TK_EQUAL) {
    pos++;
    return new_node(ND_EQUAL, lhs, equ());
  }
  if (tokens[pos].ty == TK_NOT_EQUAL) {
    pos++;
    return new_node(ND_NOT_EQUAL, lhs, equ());
  }
  return lhs;
}

Node *assign() {
  Node *lhs = equ();
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
