#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C.h"

Token *token;
LVar *locals[9];
int main(int argc, char **argv) {
  if (argc != 2) {
    error("dddd");
    return 1;
  }
  for(int i=0;i<10;i++){
  locals[i] = calloc(10,sizeof(LVar));
  }
  token = tokenize(argv[1]);
  
  program();
  
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");

  printf("  push rbp\n");
  printf("  mov rbp, rsp\n");
  printf("  sub rsp, 208\n");

  // 先頭の式から順にコード生成
  for (int i = 0; code[i]; i++) {
    gen(code[i]);

    // 式の評価結果としてスタックに一つの値が残っている
    // はずなので、スタックが溢れないようにポップしておく
    printf("  pop rax\n");
  }

  // エピローグ
  // 最後の式の結果がRAXに残っているのでそれが返り値になる
  printf("  mov rsp, rbp\n");
  printf("  pop rbp\n");
  printf("  ret\n");
  return 0;
}

