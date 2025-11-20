section .data
    ; hold addresses of the start of each list
    x1 dq 0x0000_0000_0000_0000
    x2 dq 0x0000_0000_0000_0000
    y1 dq 0x0000_0000_0000_0000
    y2 dq 0x0000_0000_0000_0000
    n dq 0x0000_0000_0000_0000; length of lists
    z dq 0x0000_0000_0000_0000; address of vector z
    msg db "Z -> ",0
    msg2 db "%f, ", 0
    msgTest db "%d ", 0
section .text
default rel
bits 64
global asmMCO2

extern printf

; i will lay out all my assumptions
asmMCO2:
    ;write your code here
    ; rcx; rdx; r8; r9 -> n, x1, x2, y1
    mov qword [n], rcx
    mov qword [x1], rdx
    mov qword [x2], r8
    mov qword [y1], r9
    
    ; 1. we assume that these all have the correct values
    
    ; debug info
    ;sub rsp, 8*4
    ;lea rcx, [msgTest]
    ;mov r12, qword [x1]
    ;mov rdx, [r12]
    ;call printf
    ;add rsp, 8*4
    
    ; fifth parameter is y2
    mov r11, qword [rsp+40]
    mov qword [y2], r11
    
    ; 2. we assume this also has the correct value
    
    ;mov r11, rcx
    ;shl r11, 2  ; r11 becomes n * 4
    ;sub rsp, r11 ; allocate n * 4 bytes to stack
    
    mov r11, rcx
    shl r11, 2        ; r11 = n * 4
    ; round up to nearest multiple of 16 for stack alignment
    add r11, 15
    and r11, 0xFFFFFFFFFFFFFFF0
    sub rsp, r11
    
    ; 3. we assume that this fixes alignment and we dont have
    ; to bother with it anymore
    
    ; the address of z is where our stack pointer currently is
    mov rax, rsp
    mov qword [z], rax ;
    ; 4. we assume that z will hold the address of the first
    ; element of the dynamically sized z-array created within
    ; this assembly as we go along in the loop
    
    xor rax, rax
    
    mov r14, 0 ; we use r14 as our counter
    loop1:
        mov rax, [x2]
        movss xmm1, [rax + r14 * 4]
        
        ; debug print value
        ;sub rsp, 8*4
        ;lea rcx, [msg2]
        ;movss xmm0, xmm1
        ;cvtss2sd xmm0, xmm0
        ;mov rdx, dword [rax + r14 * 4]
        ;call printf
        ;add rsp, 8*4
        
        mov rax, [x1]
        movss xmm2, [rax + r14 * 4]
        
        mov rax, [y2]
        movss xmm3, [rax + r14 * 4]

        mov rax, [y1]
        movss xmm4, [rax + r14 * 4]
        
        ; 5. we assume that xmm registers are all assigned correctly
        
        subss xmm1, xmm2
        mulss xmm1, xmm1
        
        subss xmm3, xmm4
        mulss xmm3, xmm3
        
        addss xmm1, xmm3
        sqrtss xmm1, xmm1
        
        mov rdx, qword [z]
        movss dword [rdx + r14 * 4], xmm1
        
        ; 6. we assume that the index is correct, since we assumed
        ; that z is the address, and that z should be the start
        
        inc r14
        
        cmp r14, qword [n]
        jl loop1
    
    add rsp, r11    ; restore stack pointer
    
    sub rsp, 8*4 ; allocate for the shadow space
    lea rcx, [msg]
    call printf
    add rsp, 8*4
    
    mov r14, 0
    loop2:
    
        ; debug info
        ;sub rsp, 8*4
        ;lea rcx, [msgTest]
        ;mov rdx, r14
        ;call printf
        ;add rsp, 8*4
        
        sub rsp, 8*4
        lea rcx, [msg2]
        mov r13, qword [z]
        movss xmm0, [r13 + r14 * 4]
        cvtss2sd xmm0, xmm0
        
        ; 7. we assume that this is the correct way to print
        ; floats from assembly
        ; 8. we assume we are addressing correctly, since we assume
        ; that z has the correct address
        
        call printf
        add rsp, 8*4
        
        inc r14
        cmp r14, qword[n]
        jl loop2
    
    xor rax, rax
    ret