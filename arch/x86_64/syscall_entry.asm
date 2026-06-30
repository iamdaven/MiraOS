global syscall_entry

extern syscall_dispatch

syscall_entry:
    push rcx
    push r11
    push rax
    push rbx
    push rbp
    push rdi
    push rsi
    push rdx
    push r8
    push r9
    push r10
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    call syscall_dispatch

    mov rax, rax

    pop r15
    pop r14
    pop r13
    pop r12
    pop r10
    pop r9
    pop r8
    pop rdx
    pop rsi
    pop rdi
    pop rbp
    pop rbx
    pop rax
    pop r11
    pop rcx

    sysretq
