.intel_syntax noprefix
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 1
  push 1
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .LendXXX
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.LendXXX:
  pop rax
  mov rsp, rbp
  pop rbp
  ret
