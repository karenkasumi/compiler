.intel_syntax noprefix
.globl main
main:
  push 10
  push 10
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  ret
