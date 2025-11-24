section .text
default rel
bits 64
global asmMCO2

asmMCO2:
    push rbp
    mov rbp, rsp
    
    ; save non-volatile registers
    push rsi
    push rdi 
    push r12
    push r13
    push r14

    ; arguments
    ; rcx = n (loop count)
    ; rdx = x1 pointer
    ; r8  = x2 pointer
    ; r9  = y1 pointer
    
    ; y2 and z are on the stack.
    ; [rbp + 48] = 5th Argument (y2)
    ; [rbp + 56] = 6th Argument (z)
    
    mov r10, rcx        ; n (loop limit)
    mov r11, rdx        ; x1 base address
    mov r12, r8         ; x2 base address
    mov r13, r9         ; y1 base address
    mov r14, [rbp + 48] ; y2 base address (from stack)
    mov rsi, [rbp + 56] ; z  base address (from stack)

    xor rbx, rbx        ; rbx = 0 (loop counter i)

loop_start:
    cmp rbx, r10        ; compare i with n
    jge loop_end        ; if i >= n, exit loop



    ;calculate (x2 - x1)^2 
    movss xmm0, [r12 + rbx*4]   ; xmm0 = x2[i]
    subss xmm0, [r11 + rbx*4]   ; xmm0 = x2[i] - x1[i]
    mulss xmm0, xmm0            ; xmm0 = (x2 - x1)^2

    ;calculate (y2 - y1)^2
    movss xmm1, [r14 + rbx*4]   ; xmm1 = y2[i]
    subss xmm1, [r13 + rbx*4]   ; xmm1 = y2[i] - y1[i]
    mulss xmm1, xmm1            ; xmm1 = (y2 - y1)^2

    ; combine then root
    addss xmm0, xmm1            ; xmm0 = (diffX^2) + (diffY^2)
    sqrtss xmm0, xmm0           ; xmm0 = sqrt( ... )

    ;store result
    movss [rsi + rbx*4], xmm0   ; z[i] = xmm0

    inc rbx                     ; i++
    jmp loop_start

loop_end:
    pop r14
    pop r13
    pop r12
    pop rdi
    pop rsi
    pop rbp
    
    xor rax,rax
    ret