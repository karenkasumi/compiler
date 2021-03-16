#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "C.h"

Token *token;
int main(int argc, char **argv) {
  if (argc != 2) {
    error("ˆø”‚ÌŒÂ”‚ª³‚µ‚­‚ ‚è‚Ü‚¹‚ñ");
    return 1;
  }

  
  token = tokenize(argv[1]);
  Node *node = expr();
  
  printf(".intel_syntax noprefix\n");
  printf(".globl main\n");
  printf("main:\n");


gen(node);


  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}