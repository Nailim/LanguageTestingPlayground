#RISCVGNU ?= riscv32-unknown-elf
RISCVGNU ?= riscv64-unknown-elf

#MARCH = -march=rv32i
MARCH = -march=rv64i

AOPS = --warn --fatal-warnings $(MARCH)
#COPS = -Wall -O2 -ffreestanding $(MARCH) -mabi=ilp32
COPS = -Wall -O2 -ffreestanding $(MARCH) -mabi=lp64
LOPS = -nostdlib 

all : main.bin

clean:
	rm -f *.bin
	rm -f *.o
	rm -f *.elf
	rm -f *.list
	rm -f *.srec

start.o : start.s
	$(RISCVGNU)-as $(AOPS) start.s -o start.o

main.o : main.c
	$(RISCVGNU)-gcc $(COPS) -c main.c -o main.o

main.bin :               memmap.ld start.o main.o
	$(RISCVGNU)-ld $(LOPS) -T memmap.ld start.o main.o -o main.elf
	$(RISCVGNU)-objdump -Mnumeric -D main.elf > main.list
	$(RISCVGNU)-objcopy --srec-forceS3 main.elf -O srec main.srec
	$(RISCVGNU)-objcopy -O binary main.elf main.bin
