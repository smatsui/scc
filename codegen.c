#include "scc.h"
#include <stdio.h>

void gen_lval(Node *node) {
  if (node->ty == ND_IDENT) {
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", ((int)map_get(idents, node->name)) * 8);
    printf("  push rax\n");
    return;
  }
  error("lval is not a variable");
}

void gen_func(Func* func){
  printf("_%s:\n", func->name);

  // prologue
  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, %d\n", 8 * idents->keys->len);

  if (func->lhs != NULL) {
    gen_lval(func->lhs);
    printf("  pop rax\n");
    printf("  mov [rax], rdi\n");
    printf("  push rdi\n");
  }

  if (func->rhs != NULL) {
    gen_lval(func->rhs);
    printf("  pop rax\n");
    printf("  mov [rax], rsi\n");
    printf("  push rdi\n");
  }

  for(int i=0; i<func->code->len; i++) {
    gen((Node *)func->code->data[i]);
    printf("  pop rax\n");
  }

  // epilogue
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
}

void gen(Node *node) {
  if (node == NULL) {
    return;
  }

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

  if (node->ty == ND_FUNC) {
    gen(node->lhs);
    gen(node->rhs);
    if (node->lhs != NULL) {
      printf("  pop rdi\n");
    }
    if (node->rhs != NULL) {
      printf("  pop rsi\n");
    }
    printf("  call _%s\n", node->name);
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

  if (node->ty == ND_IF) {
    static int if_id = 0;
    static int else_id = 0;
    static int finish_id = 0;
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 1\n");
    printf("  push rax\n");
    printf("  je if%d\n", if_id);
    if(node->else_body != NULL) {
      printf("  jmp else%d\n", else_id);
      printf("  if%d:\n", if_id);
      gen(node->body);
      printf("  jmp finish%d\n", finish_id);
      printf("  else%d:\n", else_id);
      gen(node->else_body);
      printf("  jmp finish%d\n", finish_id);
      printf("  finish%d:\n", finish_id);
    }else {
      printf("  jmp finish%d\n", finish_id);
      printf("  if%d:\n", if_id);
      gen(node->body);
      printf("  jmp finish%d\n", finish_id);
      printf("  finish%d:\n", finish_id);
    }
    if_id++;
    else_id++;
    finish_id++;
    return;
  }

  if (node->ty == ND_WHILE) {
    printf("  begin:\n");
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 1\n");
    printf("  jne end\n");
    gen(node->body);
    printf("  jmp begin\n");
    printf("  end:\n");
    return;
  }

  if (node->ty == ND_FOR) {
    gen(node->init);
    printf("  begin:\n");
    gen(node->cond);
    printf("  pop rax\n");
    printf("  cmp rax, 1\n");
    printf("  jne end\n");
    gen(node->inc);
    gen(node->body);
    printf("  jmp begin\n");
    printf("  end:\n");
    return;
  }

  if (node->ty == ND_RETURN) {
    gen(node->lhs);
    printf("  pop rax\n");
    // epilogue
    printf("  mov rsp, rbp\n");
    printf("  pop rbp\n");
    printf("  ret\n");
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
