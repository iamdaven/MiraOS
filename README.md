# MiraOS

MiraOS is a modern operating system built from scratch for x86_64. It has a complete kernel with process management, a filesystem, a networking stack, and a graphical interface with a full shell.

## Features

**Kernel**

The kernel supports the x86_64 architecture with GDT, IDT, and ISR handling. It manages physical memory with a bitmap allocator and handles virtual memory through 4 level paging. There's a heap allocator with a slab allocator for small objects, a syscall interface, and a process manager that uses round robin scheduling. It can load ELF binaries, talks to storage through an ATA/IDE driver, supports PS/2 keyboard and mouse input, and renders graphics through a double buffered framebuffer driver.

**Filesystem**

A virtual filesystem layer sits on top of a RAM filesystem (ramfs) used for persistent storage.

**Networking**

There's a basic networking stack that handles Ethernet frames and MAC addresses.

**User Interface**

The UI looks and feels like a modern desktop, with panels and widgets. The graphical shell (msh.mex) comes with over 100 built in commands, supports text rendering, and handles both mouse and keyboard input through double buffered rendering.

**Shell Commands**

The shell covers the basics you'd expect. File operations like ls, cat, mkdir, touch, cp, mv, rm, and rmdir. System info commands like uname, hostname, whoami, id, uptime, and date. Process management with ps, kill, jobs, fg, bg, and wait. Text processing tools like echo, grep, sed, awk, head, tail, wc, sort, and cut. Network tools including ping, ifconfig, ssh, scp, wget, and curl (currently stubbed). Plus plenty more.

## Building

You'll need the Zig compiler (used for C compilation), NASM, xorriso for building the ISO, and QEMU for testing.

```bash
# Build the ISO
make

# Build and run in QEMU
make run

# Clean build artifacts
make clean
```

On Windows, use the included `build-iso.bat` script.

## Architecture

```
boot/
  boot.asm        Second stage bootloader
  stage1.asm      First stage bootloader (GRUB compatible)

arch/x86_64/
  cpu.h/c         Low level CPU I/O and control
  gdt.h/c         Global Descriptor Table
  idt.h/c         Interrupt Descriptor Table
  isr.h/c/asm     Interrupt Service Routines
  paging.h/c      Virtual memory management
  syscall_entry.asm  System call entry point

kernel/
  main.c          Kernel entry point
  mem.h/c         Physical memory manager
  heap.h/c        Kernel heap allocator
  syscall.h/c     System call dispatcher
  process.h/c     Process management and scheduler
  elf.h/c         ELF binary loader
  panic.h/c       Kernel panic handler

drivers/
  driver.h/c      Driver subsystem framework
  framebuffer.h/c Graphics framebuffer
  keyboard.h/c    PS/2 keyboard
  mouse.h/c       PS/2 mouse
  timer.h/c       PIT timer
  pic.h/c         PIC interrupt controller
  ata.h/c         ATA/IDE disk driver

fs/
  vfs.h/c         Virtual filesystem layer
  ramfs.h/c       RAM filesystem implementation

net/
  net.h/c         Network stack core

ui/
  ui.h/c          UI framework and widget system
  shell.h/c       Full featured shell (msh.mex)
  widget.h/c      UI widgets (panels, buttons, textfields)
  gfx.h/c         Graphics primitives
  text.h/c        Text rendering
  input.h/c       Input handling

lib/
  ds.h/c          Data structures library
  cxxrt.h/c       C++ runtime support
```

## Boot Process

GRUB loads stage1.bin and kernel.elf. Stage1 sets up basic CPU state and jumps into the kernel. From there the kernel initializes the GDT, physical memory manager, paging, heap allocator, IDT, ISR, syscalls, drivers (framebuffer, keyboard, mouse, timer, ATA), the ramfs filesystem, the network stack, and finally the UI and shell. Once everything's up, the kernel enters its idle loop, handling input and rendering the UI.

## Memory Layout

The kernel loads at 1MB physical. The heap starts at virtual address 0x40000000. The stack grows downward from high memory, and each process gets a 1MB stack of its own.

## System Calls

The kernel exposes exit, read, write, open, close, draw_rect, draw_text, and get_ticks.

## Development Status

MiraOS is actively developed and already has preemptive multitasking, virtual memory, ELF loading, a storage driver, a basic network framework, a modern UI with a working shell, and over 100 shell commands.

## License

GPL 3.0