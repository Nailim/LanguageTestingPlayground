# Mango Pi baremetal poke LEd test

RiscV 32bit assembly test with GCC:

Poke a register with an LED on Mango Pi

## compile

- riscv32-unknown-elf-gcc -nostdlib -march=rv32ima -mabi=ilp32 -T link.ld -o main.elf start.s
- riscv32-unknown-elf-objcopy -O binary main.elf main.bin

## install

- sudo /opt/rv/xfel/xfel write 0x45000 main.bin
- sudo /opt/rv/xfel/xfel exec 0x45000
