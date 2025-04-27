# Mango Pi baremetal poke LEd test

RiscV 32bit assembly test with Plan9 assembler and linker on 9front:

Poke a register with an LED on Mango Pi


## compile

ia start.s
il -H 1 -T0x42000000 -E _start -o main.bin start.i

## install

- sudo /opt/rv/xfel/xfel write 0x45000 main.bin
- sudo /opt/rv/xfel/xfel exec 0x45000

## notes

Requires RiscV [assembler/compiler/linker](https://github.com/Nailim/9front-riscv-overlay) for 9front.

