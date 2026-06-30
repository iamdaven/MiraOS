; GRUB-compatible first-stage bootloader
; Sets up basic state and loads the kernel

[BITS 32]
[ORG 0x0]

stage1_start:
    ; Set up data segments
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    sti

    ; Load stage2 from disk
    mov si, load_msg
    call print_string

    ; Read stage2 from disk (simplified - in real impl would use BIOS int 13h)
    ; For now, just halt
    hlt

load_msg:
    db "Loading MiraOS...", 0

print_string:
    pusha
    mov ah, 0x0E
.print_loop:
    lodsb
    test al, al
    jz .print_done
    int 0x10
    jmp .print_loop
.print_done:
    popa
    ret

times 510-($-$$) db 0
dw 0xAA55