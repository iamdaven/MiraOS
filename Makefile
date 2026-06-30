ZIG      ?= zig
ASM      ?= C:/msys64/usr/bin/nasm.exe
XORRISO  ?= C:/msys64/usr/bin/xorriso.exe

TARGET   = x86_64-freestanding-none
CC       = $(ZIG) cc -target $(TARGET)
LD       = $(ZIG) cc -target $(TARGET)

CFLAGS   = -ffreestanding -fno-stack-check -fno-stack-protector -mno-red-zone \
           -mcmodel=kernel -Wall -Wextra -Werror \
           -I. -Iarch/x86_64 -Idrivers -Ifs -Iui -Ikernel -O2
ASMFLAGS = -f elf64
LDFLAGS  = -nostdlib -Wl,-T,linker.ld -Wl,-z,max-page-size=0x1000

BOOT_ASM = boot/boot.asm
ASM_SRCS = arch/x86_64/isr.asm arch/x86_64/syscall_entry.asm

C_SRCS   = kernel/main.c \
           kernel/panic.c \
           kernel/mem.c \
           kernel/heap.c \
           kernel/syscall.c \
           arch/x86_64/gdt.c \
           arch/x86_64/idt.c \
           arch/x86_64/isr.c \
           arch/x86_64/paging.c \
           drivers/driver.c \
           drivers/timer.c \
           drivers/keyboard.c \
           drivers/framebuffer.c \
           fs/vfs.c \
           fs/ramfs.c \
           ui/ui.c \
           ui/shell.c \
           ui/widget.c \
           ui/gfx.c \
           ui/text.c \
           ui/input.c

BOOT_OBJ = build/boot.o
ASM_OBJS = build/arch/x86_64/isr_asm.o build/arch/x86_64/syscall_entry.o
C_OBJS   = $(C_SRCS:%.c=build/%.o)
OBJS     = $(BOOT_OBJ) $(ASM_OBJS) $(C_OBJS)

ISO      = miraos.iso
KERNEL   = build/kernel.elf
GRUB_DIR = tools/grub

.PHONY: all iso clean dirs grub

all: iso

iso: $(ISO)

grub:
	@bash tools/fetch-grub.sh

$(ISO): $(KERNEL) grub.cfg grub
	rm -rf iso
	mkdir -p iso/boot/grub
	cp $(KERNEL) iso/boot/kernel.elf
	cp grub.cfg iso/boot/grub/grub.cfg
	cp -r $(GRUB_DIR)/i386-pc iso/boot/grub/
	$(XORRISO) -return_with SORRY 0 -as mkisofs -R -J -joliet-long \
		-b boot/grub/i386-pc/cdboot.img \
		-no-emul-boot -boot-load-size 8 -boot-info-table \
		--grub2-boot-info \
		--grub2-mbr $(GRUB_DIR)/i386-pc/boot_hybrid.img \
		-o $(ISO) iso

$(KERNEL): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

build/boot.o: boot/boot.asm | dirs
	$(ASM) $(ASMFLAGS) -o $@ $<

build/arch/x86_64/isr_asm.o: arch/x86_64/isr.asm | dirs
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) -o $@ $<

build/arch/x86_64/syscall_entry.o: arch/x86_64/syscall_entry.asm | dirs
	@mkdir -p $(dir $@)
	$(ASM) $(ASMFLAGS) -o $@ $<

build/%.o: %.c | dirs
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

dirs:
	@mkdir -p build/arch/x86_64 build/boot build/kernel build/drivers build/fs build/ui

clean:
	rm -rf build iso $(ISO)

run: iso
	qemu-system-x86_64 -cdrom $(ISO) -m 256M -serial stdio
