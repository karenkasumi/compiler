.intel_syntax noprefix
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 0
  pop rax
  cmp rax, 0
  je .LelseXXX
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  jmp .LendXXX
.LelseXXX:
  push 1
  pop rax
  mov rsp, rbp
  pop rbp
  ret
.LendXXX:
  push 0
  push 0
  pop rax
  mov rsp, rbp
  pop rbp
  ret
  pop rdi
  pop rax
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
