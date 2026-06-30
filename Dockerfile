FROM ubuntu:24.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y \
    make nasm xorriso grub-pc-bin grub-common \
    gcc-x86-64-elf binutils-x86-64-elf \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY . .

RUN make iso
