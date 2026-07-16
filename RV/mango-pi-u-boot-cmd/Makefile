MKIMAGE ?= ../smaeul-u-boot/tools/mkimage

all: boot.scr

boot.scr: boot.cmd
	$(MKIMAGE) -C none -A riscv -T script -d boot.cmd boot.scr

clean:
	rm -f boot.scr

.PHONY: all clean
