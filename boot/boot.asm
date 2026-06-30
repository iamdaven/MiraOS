section .multiboot
align 8
header_start:
    dd 0xe85250d6
    dd 0
    dd header_end - header_start
    dd -(0xe85250d6 + 0 + (header_end - header_start))

    align 8
    dw 0
    dw 0
    dd 8

    align 8
    dw 5
    dw 0
    dd 12
    dd 1024
    dd 768
    dd 32

header_end:

section .bss
align 16
stack_bottom:
    resb 32768
stack_top:

section .text
global _start
extern kernel_main

_start:
    mov rsp, stack_top
    cld
    mov rdi, rax
    mov rsi, rbx
    call kernel_main
    cli
.hang:
    hlt
    jmp .hang
