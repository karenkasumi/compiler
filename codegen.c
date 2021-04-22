#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C.h"

void gen_lval(Node *node){
    if(node->kind != ND_LVAR) error("aaaa\n");
    printf("  mov rax, rbp\n");
    printf("  sub rax, %d\n", node->offset);
    printf("  push rax\n");
}

int reflag=0;
void gen(Node *node) {
    switch(node->kind){
    case ND_NUM:
        printf("  push %d\n", node->val);
        return;
    case ND_LVAR:
        gen_lval(node);
        printf("  pop rax\n");
        printf("  mov rax, [rax]\n");
        printf("  push rax\n");
        return;
    case ND_ASSIGN:
        gen_lval(node->lhs);
        gen(node->rhs);

        printf("  pop rdi\n");
        printf("  pop rax\n");
        printf("  mov [rax], rdi\n");
        printf("  push rdi\n");
        return;
    case ND_RETURN:
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  pop rdi\n");
        printf("  mov rsp, rbp\n");
        printf("  pop rbp\n");
        if(reflag){
        printf("  push rax\n");
        printf("  push rdi\n");
        }
        printf("  ret\n");
        return;
    case ND_IF:
        reflag = 0;
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .LendXXX\n");
        gen(node->rhs->rhs);
        printf(".LendXXX:\n");
        return;
    case ND_IF_ELSE:
        reflag = 0;
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .LelseXXX\n");
        gen(node->rhs->rhs);
        printf("  jmp .LendXXX\n");
        printf(".LelseXXX:\n");
        gen(node->rhs->lhs);
        printf(".LendXXX:\n");
        return;
    case ND_WHILE:
        reflag = 0;
        printf(".LbeginXXX:\n");
        gen(node->lhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .LendXXX\n");
        gen(node->rhs);
        printf("  jmp .LbeginXXX\n");
        printf(".LendXXX:\n");
        return;
    case ND_FOR:
        reflag = 0;
        gen(node->lhs->lhs->lhs);
        printf(".LbeginXXX:\n");
        gen(node->lhs->lhs->rhs);
        printf("  pop rax\n");
        printf("  cmp rax, 0\n");
        printf("  je .LendXXX\n");
        gen(node->rhs);
        gen(node->lhs->rhs);
        printf("  jmp .LbeginXXX\n");
        printf(".LendXXX:\n");
        return;
    case ND_BLOCK:
        {int i=0;
        while(node->block[i]){
          gen(node->block[i]);
          printf("  pop rax\n");
          i++;
        }}
        return;
    case ND_FUNC_DEF:
        printf("  jmp .Lend%d\n", node->gloloc);
        printf(".Lbegin%d:\n", node->gloloc);
        {int i = 0;
        while(node->argu[i]->kind == ND_LVAR){
          gen(node->argu[i]);
          printf("  pop rax\n");
          i++;
        }}
        if(node->rhs->kind == ND_BLOCK){
        int i=0;
          while(node->rhs->block[i]){
            reflag = 1;
            gen(node->rhs->block[i]);
            printf("  pop rax\n");
            i++;
          }
        }else{
          reflag = 1;
          gen(node->rhs);
        }
        printf("  push rax\n");
        printf(".Lend%d:\n", node->gloloc);
        return;
    case ND_FUNC:
        printf("  push rbp\n");
        printf("  mov rbp, rsp\n");
        printf("  sub rsp, 208\n");
        {int i = 0;
        while(node->argu[i]){
          gen(node->argu[i]);
          printf("  pop rdi\n");
          printf("  mov rax, rbp\n");
          printf("  sub rax, %d\n",(i+1)*8);
          printf("  mov [rax], rdi\n");
          i++;
        }}
        printf("  call .Lbegin%d\n", node->gloloc);
        printf("  pop rax\n");
        printf("  push rax\n");

        return;
         
    }
    

  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
  case ND_ADD:
    printf("  add rax, rdi\n");
    break;
  case ND_SUB:
    printf("  sub rax, rdi\n");
    break;
  case ND_MUL:
    printf("  imul rax, rdi\n");
    break;
  case ND_DIV:
    printf("  cqo\n");
    printf("  idiv rdi\n");
    break;
  case ND_equal:
    printf("  cmp rax, rdi\n");
    printf("  sete al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_nequal:
    printf("  cmp rax, rdi\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_big:
    printf("  cmp rax, rdi\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_ebig:
    printf("  cmp rax, rdi\n");
     printf("  setne al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_small:
    printf("  cmp rdi, rax\n");
    printf("  setl al\n");
    printf("  movzb rax, al\n");
    break;
  case ND_esmall:      
    printf("  cmp rdi, rax\n");
    printf("  setne al\n");
    printf("  movzb rax, al\n");
    break; 

  }


    printf("  push rax\n");
}