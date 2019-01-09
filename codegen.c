#include "scc.h"
#include <stdio.h>

void gen_lval(Node *node) {
  if (node->ty == ND_IDENT) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", ((int)map_get(idents, &node->name) + 1) * 8);
    printf("  push rax\n");
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
  case ND_EQUAL:
    printf("  cmp rdi, rax\n");
    printf("  sete al\n");
    printf("  movzx rax, al\n");
    break;
  case ND_NOT_EQUAL:
    printf("  cmp rdi, rax\n");
    printf("  setne al\n");
    printf("  movzx rax, al\n");
    break;
  case '>':
    printf("  cmp rdi, rax\n");
    printf("  setl al\n");
    printf("  movzx rax, al\n");
    break;
  case '<':
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzx rax, al\n");
    break;
  case ND_GREATER_EQUAL:
    printf("  cmp rdi, rax\n");
    printf("  setle al\n");
    printf("  movzx rax, al\n");
    break;
  case ND_LESS_EQUAL:
    printf("  cmp rax, rdi\n");
    printf("  setle al\n");
    printf("  movzx rax, al\n");
    break;
  case '&':
    printf("and rax, rdi\n");
    break;
  case '^':
    printf("xor rax, rdi\n");
    break;
  case '|':
    printf("or rax, rdi\n");
    break;
  case ND_AND:
    printf("and rax, rdi\n");
    break;
  case ND_OR:
    printf("or rax, rdi\n");
    break;
  }

  printf("  push rax\n");
}
