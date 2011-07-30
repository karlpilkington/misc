[SECTION .text]; 

extern printf
extern puts
extern gethostname
global main

main:
  push ebp
  mov ebp,esp; bottom of stack becomes top of new stack frame
  push ebx
  push esi  ; save sacred registers
  push edi
  ; end boilerplate

  push 100            ; length of buf
  push buf
  call gethostname
  mov ecx, eax
  jecxz success

 fail:
  push dword failed
  call puts
  jmp done

 success:
  push buf
  push format
  call printf

 done:
  ; resume boilerplate
  pop edi
  pop esi
  pop ebx
  mov esp,ebp 
  pop ebp
  ret

[SECTION .data]
format: db "hostname is %s",10,0
failed: db "gethostname failed",10,0

[SECTION .bss]
buf: resb 100

