section .text
global syscall_entry
extern syscall_dispatch

syscall_entry:
    swapgs
    mov [gs:0], rsp
    mov rsp, [gs:8]
    push 0x23
    push qword [gs:0]
    push r11
    push 0x1B
    push rcx
    mov r10, rdx
    mov rdx, rsi
    mov rsi, rdi
    mov rdi, rax
    call syscall_dispatch
    pop rcx
    add rsp, 24
    pop r11
    mov rsp, [gs:0]
    swapgs
    sysretq
