#!/usr/bin/bash
riscv64-elf-gcc -march=rv64imacd_zicsr -mabi=lp64 -c -mcmodel=medany kernel.c -o kernel.o -ffreestanding
riscv64-elf-as -march=rv64imacd_zicsr -mabi=lp64 -c entry.S -o entry.o
riscv64-elf-ld -T linker.ld -nostdlib kernel.o entry.o -o kernel.elf
riscv64-unknown-elf-objcopy -O binary kernel.elf kernel.bin

echo Info for future me, this is not actual qemu output.
echo To enter monitor, press C-a c
echo Actual qemu starts beyond this line
echo -----------------------------

qemu-system-riscv64 \
  -M virt \
  -cpu rv64,pmp=false,h=false,s=false \
  -m 16M \
  -nographic \
  -bios none \
  -kernel kernel.bin \
  -device loader,file=kernel.bin,addr=0x80000000 \
  -serial mon:stdio
