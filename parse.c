#include "scc.h"
#include <stdio.h>
#include <stdlib.h>

// Buffer for parsed node.
Vector *code;

// Buffer for variables
Map *idents;

Node *statement();
Node *assign();
Node *bit_or();
Node *bit_xor();
Node *bit_and();
Node *equ();
Node *cmp();
Node *add();
Node *mul();
Node *term();

int consume(int ty) {
  if (((Token *)tokens->data[pos])->ty != ty) {
    return 0;
  }
  pos++;
  return 1;
}

void print_tree(Node *node) {
  if (node == NULL) {
    return;
  }
  if (node->ty == ND_NUM) {
    printf("%d\n", node->val);
  } else if (node->ty == ND_IDENT) {
    printf("%c\n", node->name);
  } else if (node->ty == ND_EQUAL) {
    printf("==\n");
  } else if (node->ty == ND_NOT_EQUAL) {
    printf("!=\n");
  } else if (node->ty == ND_IF) {
    printf("if\n");
  } else {
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

Node *new_node_ident(char name) {
  static int ident_id = 1;
  Node *node = calloc(1, sizeof(Node));
  node->ty = ND_IDENT;
  node->name = name;
  if(!map_get(idents, &node->name)){
    map_put(idents, &node->name, (void *)ident_id++);
  }
  return node;
}

Node *term() {
  if (((Token *)tokens->data[pos])->ty == TK_NUM) {
    return new_node_num(((Token *)tokens->data[pos++])->val);
  }
  if (((Token *)tokens->data[pos])->ty == TK_IDENT) {
    return new_node_ident(((Token *)tokens->data[pos++])->input[0]);
  }
  if (consume('(')) {
    Node *node = add();
    if (consume(')')) {
      return node;
    }
    error("there is no right parenthesis: %s",
          ((Token *)tokens->data[pos])->input);
  }
  error("not a number or a left parenthesis: %s",
        ((Token *)tokens->data[pos])->input);
  return 0;
}

Node *mul() {
  Node *lhs = term();
  if (consume('*')) {
    return new_node('*', lhs, mul());
  }
  if (consume('/')) {
    return new_node('/', lhs, mul());
  }
  return lhs;
}

Node *add() {
  Node *lhs = mul();
  if (consume('+')) {
    return new_node('+', lhs, add());
  }
  if (consume('-')) {
    return new_node('-', lhs, add());
  }
  return lhs;
}

Node *cmp() {
  Node *lhs = add();
  if (consume('>')) {
    return new_node('>', lhs, cmp());
  }
  if (consume('<')) {
    return new_node('<', lhs, cmp());
  }
  if (consume(TK_GREATER_EQUAL)) {
    return new_node(ND_GREATER_EQUAL, lhs, cmp());
  }
  if (consume(TK_LESS_EQUAL)) {
    return new_node(ND_LESS_EQUAL, lhs, cmp());
  }
  return lhs;
}

Node *equ() {
  Node *lhs = cmp();
  if (consume(TK_EQUAL)) {
    return new_node(ND_EQUAL, lhs, equ());
  }
  if (consume(TK_NOT_EQUAL)) {
    return new_node(ND_NOT_EQUAL, lhs, equ());
  }
  return lhs;
}

Node *bit_and() {
  Node *lhs = equ();
  if (consume('&')) {
    return new_node('&', lhs, bit_and());
  }
  return lhs;
}

Node *bit_xor() {
  Node *lhs = bit_and();
  if (((Token *)tokens->data[pos])->ty == '^') {
    pos++;
    return new_node('^', lhs, bit_xor());
  }
  return lhs;
}

Node *bit_or() {
  Node *lhs = bit_xor();
  if (consume('|')) {
    return new_node('|', lhs, bit_or());
  }
  return lhs;
}

Node *logical_and() {
  Node *lhs = bit_or();
  if (consume(TK_AND)) {
    return new_node(ND_AND, lhs, logical_and());
  }
  return lhs;
}

Node *logical_or() {
  Node *lhs = logical_and();
  if (consume(TK_OR)) {
    return new_node(ND_OR, lhs, logical_or());
  }
  return lhs;
}

Node *assign() {
  Node *lhs = logical_or();
  if (consume('=')) {
    Node *node = new_node('=', lhs, assign());
    return node;
  }
  if (consume(';')) {
    return lhs;
  }
  error("semicolon not found: %s", ((Token *)tokens->data[pos])->input);
  return 0;
}

Node *stmt() {
  if (consume(TK_IF)) {
    if (!consume('(')){
      error("left paren not found: %s",((Token *)tokens->data[pos])->input);
    }
    Node *cond = equ();
    if (!consume(')')){
      error("right parent not found: %s",((Token *)tokens->data[pos])->input);
    }
    return new_node(ND_IF, cond, stmt());
  }
  return assign();
}

void program() {
  code = new_vector();
  idents = new_map();
  while (((Token *)tokens->data[pos])->ty != TK_EOF) {
    Node *node = stmt();
    vec_push(code, node);
    //    print_tree(node);
  }
}
