[SECTION .text]; 

extern printf
global main

main:
  push ebp
  mov ebp,esp; bottom of stack becomes top of new stack frame
  push ebx
  push esi  ; save sacred registers
  push edi
  ; end boilerplate

  push dword hello    ; args go last-first
  push dword format   ; printf format
  call printf
  
  ; resume boilerplate
  pop edi
  pop esi
  pop ebx
  mov esp,ebp 
  pop ebp
  ret

[SECTION .data]
hello: db "Hello, world!",10,0
format: db "%s",0

