; Stage 2 Bootloader
; Loads in protected mode, sets up paging, and jumps to kernel

BITS 16
ORG 0x1000

stage2_start:
    ; Set up data segments
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x9000
    sti

    ; Print loading message
    mov si, loading_msg
    call print_string_16

    ; Enable A20 line
    call enable_a20

    ; Load Global Descriptor Table
    lgdt [gdt_descriptor]

    ; Enable protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to flush CPU pipeline and switch to protected mode
    jmp 0x08:protected_mode_entry

BITS 32
protected_mode_entry:
    ; Set up data segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9000

    ; Clear screen
    mov edi, 0xB8000
    mov ecx, 2000
    mov eax, 0x07200720
    rep stosd

    ; Print message in protected mode
    mov esi, protected_msg
    mov edi, 0xB8000
    call print_string_32

    ; Load kernel ELF
    call load_kernel

    ; Jump to kernel
    jmp 0x08:0x100000

; Enable A20 line using keyboard controller
enable_a20:
    pusha
.wait:
    in al, 0x64
    test al, 0x02
    jnz .wait
    mov al, 0xD1
    out 0x64, al
.wait2:
    in al, 0x64
    test al, 0x02
    jnz .wait2
    mov al, 0xDF
    out 0x60, al
    popa
    ret

; Load kernel ELF file
load_kernel:
    pusha
    mov esi, kernel_start_lba
    mov edi, 0x100000
    mov ecx, 0x100
.load_loop:
    push ecx
    push esi
    call read_lba
    pop esi
    pop ecx
    add edi, 512
    add esi, 1
    loop .load_loop
    popa
    ret

; Read LBA from CD-ROM
read_lba:
    pusha
    mov word [dap], 0x0010
    mov word [dap+2], 0x0001
    mov word [dap+4], 0x0000
    mov word [dap+6], 0x0000
    mov word [dap+8], 0x0000
    mov dword [dap+10], esi
    mov dword [dap+14], 0x00000000
    mov ah, 0x42
    mov dl, 0xE0
    mov si, dap
    int 0x13
    jc .error
    popa
    ret
.error:
    mov esi, error_msg
    call print_string_32
    hlt
    jmp $

; 16-bit print string
print_string_16:
    pusha
.next_char_16:
    lodsb
    test al, al
    jz .done_16
    mov ah, 0x0E
    int 0x10
    jmp .next_char_16
.done_16:
    popa
    ret

; 32-bit print string
print_string_32:
    pusha
.next_char_32:
    lodsb
    test al, al
    jz .done_32
    mov [edi], al
    inc edi
    mov byte [edi], 0x07
    inc edi
    jmp .next_char_32
.done_32:
    popa
    ret

; GDT
gdt_start:
    dd 0x00000000
    dd 0x00000000
    dd 0x00CF9A00
    dd 0x0000FFFF
    dd 0x00CF9200
    dd 0x0000FFFF
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

; Messages
loading_msg db 'MiraOS Bootloader - Loading...', 0
protected_msg db 'Protected mode enabled. Loading kernel...', 0
error_msg db 'Boot error!', 0
kernel_start_lba dd 0x0000002F

; DAP for disk reads
dap:
    times 16 db 0

; Padding
times 2048-($-$$) db 0