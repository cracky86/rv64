#!/usr/bin/bash
riscv64-elf-gcc -Os -c -mcmodel=medany kernel.c -o main.o -ffreestanding
riscv64-elf-as -c entry.S -o entry.o
riscv64-elf-ld -Os -T linker.ld -nostdlib main.o entry.o -o main.elf
riscv64-unknown-elf-objcopy -O binary main.elf kernel.bin

echo Info for future me, this is not actual qemu output.
echo To enter monitor, press C-a c
echo Actual qemu starts beyond this line
echo -----------------------------

qemu-system-riscv64 \
  -machine virt \
  -m 16M \
  -nographic \
  -bios none \
  -kernel kernel.bin \
  -device loader,file=kernel.bin,addr=0x80000000 \
  -serial mon:stdio
