; Stage 1 Bootloader - 2048 bytes (4 sectors)
; El Torito no-emulation bootable CD-ROM boot image
; Contains complete bootloader in one file

BITS 16
ORG 0x7C00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00
    sti

    ; Save boot drive number
    mov [boot_drive], dl

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

    ; Clear screen (VGA text mode: 0xB8000)
    mov edi, 0xB8000
    mov ecx, 2000
    mov eax, 0x07200720
    rep stosd

    ; Print message in protected mode
    mov esi, protected_msg
    mov edi, 0xB8000
    call print_string_32

    ; Load kernel ELF from ISO
    ; Kernel is at LBA 32 (after bootloader)
    mov dword [kernel_lba], 32
    call load_kernel

    ; Jump to kernel (linked at 1MB)
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

; Load kernel ELF file from CD-ROM
load_kernel:
    pusha
    mov esi, [kernel_lba]
    mov edi, 0x100000
    mov ecx, 256
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

; Read LBA from CD-ROM using INT 13h AH 42h
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
    mov dl, [boot_drive]
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

; 16-bit print string (BIOS teletype)
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

; 32-bit print string (VGA text mode)
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

; GDT (Global Descriptor Table)
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

; Variables
boot_drive db 0
kernel_lba dd 32

; DAP for disk reads
dap:
    times 16 db 0

; Pad to exactly 2048 bytes (4 sectors)
times 2048-($-$$) db 0