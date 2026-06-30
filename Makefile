ZIG      ?= zig
ASM      ?= C:/msys64/usr/bin/nasm.exe
XORRISO  ?= C:/msys64/usr/bin/xorriso.exe

TARGET   = x86_64-freestanding-none
CC       = $(ZIG) cc -target $(TARGET)
LD       = $(ZIG) cc -target $(TARGET)

CFLAGS   = -ffreestanding -fno-stack-check -fno-stack-protector -mno-red-zone \
           -mcmodel=kernel -Wall -Wextra -Werror \
           -I. -Iarch/x86_64 -Idrivers -Ifs -Iui -Ikernel -Ilib -Inet -O2
ASMFLAGS = -f elf64
LDFLAGS  = -nostdlib -Wl,-T,linker.ld -Wl,-z,max-page-size=0x1000

BOOT_ASM = boot/boot.asm
STAGE1_ASM = boot/stage1.asm
ASM_SRCS = arch/x86_64/isr.asm arch/x86_64/syscall_entry.asm

C_SRCS   = kernel/main.c \
           kernel/panic.c \
           kernel/mem.c \
           kernel/heap.c \
           kernel/syscall.c \
           kernel/process.c \
           kernel/elf.c \
           arch/x86_64/gdt.c \
           arch/x86_64/idt.c \
           arch/x86_64/isr.c \
           arch/x86_64/paging.c \
           drivers/driver.c \
           drivers/timer.c \
           drivers/keyboard.c \
           drivers/framebuffer.c \
           drivers/mouse.c \
           drivers/pic.c \
           drivers/ata.c \
           fs/vfs.c \
           fs/ramfs.c \
           lib/ds.c \
           lib/mem.c \
           lib/string.c \
           lib/cxxrt.c \
           net/net.c \
           ui/ui.c \
           ui/shell.c \
           ui/widget.c \
           ui/window.c \
           ui/gfx.c \
           ui/text.c \
           ui/input.c

BOOT_OBJ = build/boot.o
STAGE1_OBJ = build/stage1.bin
ASM_OBJS = build/arch/x86_64/isr_asm.o build/arch/x86_64/syscall_entry.o
C_OBJS   = $(C_SRCS:%.c=build/%.o)
OBJS     = $(BOOT_OBJ) $(ASM_OBJS) $(C_OBJS)

ISO      = miraos.iso
KERNEL   = build/kernel.elf

.PHONY: all iso clean dirs

all: iso

iso: $(ISO)

$(ISO): $(KERNEL) $(STAGE1_OBJ)
	rm -rf iso
	mkdir -p iso/boot
	cp $(KERNEL) iso/boot/kernel.elf
	cp $(STAGE1_OBJ) iso/boot/stage1.bin
	$(XORRISO) -return_with SORRY 0 -as mkisofs -R -J -joliet-long \
		-b boot/stage1.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		-o $(ISO) iso

$(KERNEL): $(OBJS) linker.ld
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

build/boot.o: boot/boot.asm | dirs
	$(ASM) $(ASMFLAGS) -o $@ $<

build/stage1.bin: boot/stage1.asm | dirs
	$(ASM) -f bin -o $@ $<

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
	@mkdir -p build/arch/x86_64 build/boot build/kernel build/drivers build/fs build/ui build/lib build/net

clean:
	rm -rf build iso $(ISO)

run: iso
	qemu-system-x86_64 -cdrom $(ISO) -m 256M -serial stdio