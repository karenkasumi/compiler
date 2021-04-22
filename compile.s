.intel_syntax noprefix
.globl main
main:
  push rbp
  mov rbp, rsp
  sub rsp, 208
  jmp .Lend1
.Lbegin1:
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  mov rax, rbp
  sub rax, 8
  push rax
  pop rax
  mov rax, [rax]
  push rax
  mov rax, rbp
  sub rax, 16
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rdi
  pop rax
  cmp rax, rdi
  sete al
  movzb rax, al
  push rax
  pop rax
  cmp rax, 0
  je .LendXXX
  mov rax, rbp
  sub rax, 24
  push rax
  push 1
  pop rdi
  pop rax
  mov [rax], rdi
  push rdi
.LendXXX:
  pop rax
  mov rax, rbp
  sub rax, 24
  push rax
  pop rax
  mov rax, [rax]
  push rax
  pop rax
  pop rdi
  mov rsp, rbp
  pop rbp
  push rax
  push rdi
  ret
  pop rax
  push rax
.Lend1:
  pop rax
  push rbp
  mov rbp, rsp
  sub rsp, 208
  push 1
  pop rdi
  mov rax, rbp
  sub rax, 8
  mov [rax], rdi
  push 1
  pop rdi
  mov rax, rbp
  sub rax, 16
  mov [rax], rdi
  call .Lbegin1
  pop rax
  push rax
  pop rax
  mov rsp, rbp
  pop rbp
  ret
