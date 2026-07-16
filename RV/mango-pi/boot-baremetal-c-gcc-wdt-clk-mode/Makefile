CROSS_COMPILE ?= riscv64-unknown-elf-
CC      := $(CROSS_COMPILE)gcc
OBJCOPY := $(CROSS_COMPILE)objcopy
OBJDUMP := $(CROSS_COMPILE)objdump

XFEL ?= xfel

CFLAGS  := -march=rv64imafdc -mabi=lp64d -mcmodel=medany \
           -Os -ffreestanding -fno-builtin -Wall -Wextra
LDFLAGS := -T link.ld -nostdlib -static

OBJS := start.o main.o uart.o gpio.o wdt.o

all: main.bin

main.elf: $(OBJS) link.ld
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(OBJS)

main.bin: main.elf
	$(OBJCOPY) -O binary $< $@

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

disasm: main.elf
	$(OBJDUMP) -d main.elf

smode_stub.elf: smode_stub.o
	$(CC) $(CFLAGS) -Ttext=0x45000 -nostdlib -static -e _start -o $@ $<

smode_stub.bin: smode_stub.elf
	$(OBJCOPY) -O binary $< $@

# Board must already be in FEL mode (USB-OTG cable, not just serial)
fel: main.bin
	$(XFEL) ddr d1
	$(XFEL) write 0x41000000 main.bin
	$(XFEL) exec 0x41000000

# Same as `fel`, but drops to S-mode before jumping to main.bin (see
# smode_stub.S), so the M-mode-only CSR probe in start.S actually gets
# exercised the same way it would when chainloaded via U-Boot - without
# needing a full U-Boot boot cycle to test it.
fel-smode: main.bin smode_stub.bin
	$(XFEL) ddr d1
	$(XFEL) write 0x41000000 main.bin
	$(XFEL) write 0x45000 smode_stub.bin
	$(XFEL) exec 0x45000

clean:
	rm -f *.o main.elf main.bin smode_stub.elf smode_stub.bin

.PHONY: all clean disasm fel fel-smode
