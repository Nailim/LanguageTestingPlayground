.PHONY: help
.PHONY: clean
.PHONY: build
.PHONY: analyze

help:
	@echo Help ...
	@echo Run \"make clean\" to clean
	@echo Run \"make build\" to build
	@echo Run \"make analyze\" to print out obj sizes
	@echo Run \"make upload-XXX\" to upload to Arduino Nano board - edit USB port if necessary

clean: clean-gcc clean-zig clean-rust
build: build-gcc build-zig build-rust

analyze: analyze-gcc analyze-zig analyze-rust
	ls -l gcc_nano_blink gcc_nano_blink.hex gcc_nano_blink_manual gcc_nano_blink_manual.hex \
	zig_nano_blink_manual zig_nano_blink_manual.hex \
	rust_nano_blink rust_nano_blink.hex rust_nano_blink_manual rust_nano_blink_manual.hex

build-gcc: clean-gcc compile-gcc to-exe-gcc to-hex-gcc

compile-gcc: nano_blink.c
	avr-gcc -O3 -DF_CPU=16000000UL -mmcu=atmega328p -c -o gcc_nano_blink.o nano_blink.c
	avr-gcc -O3 -DF_CPU=16000000UL -mmcu=atmega328p -c -o gcc_nano_blink_manual.o nano_blink_manual.c

to-exe-gcc:  gcc_nano_blink.o
	avr-gcc -mmcu=atmega328p gcc_nano_blink.o -o gcc_nano_blink
	avr-gcc -mmcu=atmega328p gcc_nano_blink_manual.o -o gcc_nano_blink_manual

to-hex-gcc: gcc_nano_blink
	avr-objcopy -O ihex -R .eeprom gcc_nano_blink gcc_nano_blink.hex
	avr-objcopy -O ihex -R .eeprom gcc_nano_blink_manual gcc_nano_blink_manual.hex

clean-gcc:
	rm -f gcc_nano_blink*

upload-gcc: gcc_nano_blink.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:gcc_nano_blink.hex

analyze-gcc: gcc_nano_blink
	size gcc_nano_blink
	size gcc_nano_blink_manual



# long way around for zig: zig->obj->exe->hex
# https://github.com/ziglang/zig/issues/10372

build-zig: clean-zig compile-zig to-exe-zig to-hex-zig

compile-zig: nano_blink_manual.zig
	zig build-obj -fstrip -OReleaseSmall -target avr-freestanding-none -mcpu=atmega328p nano_blink_manual.zig --name zig_nano_blink_manual

to-exe-zig:  zig_nano_blink_manual.o
	avr-gcc -mmcu=atmega328p zig_nano_blink_manual.o -o zig_nano_blink_manual

to-hex-zig: zig_nano_blink_manual
	avr-objcopy -O ihex -R .eeprom zig_nano_blink_manual zig_nano_blink_manual.hex

clean-zig:
#	rm -f zig_nano_blink*.hex
	rm -f zig_nano_blink*
#	rm -f zig_nano_blink*.o

upload-zig: zig_nano_blink_manual.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:zig_nano_blink_manual.hex

analyze-zig: zig_nano_blink_manual
	size zig_nano_blink_manual


# use crate for rust
# https://copyprogramming.com/howto/getting-started-with-embedded-development-using-rust-and-arduino

build-rust: clean-rust compile-rust to-hex-rust

compile-rust:
	cd nano-blink-rust && cargo build --release
	cp nano-blink-rust/target/avr-atmega328p/release/nano-blink-rust.elf ./rust_nano_blink
	
	cd nano-blink-manual-rust && cargo build --release
	cp nano-blink-manual-rust/target/avr-atmega328p/release/nano-blink-manual-rust.elf ./rust_nano_blink_manual

to-hex-rust: rust_nano_blink
	avr-objcopy -O ihex -R .eeprom rust_nano_blink rust_nano_blink.hex
	avr-objcopy -O ihex -R .eeprom rust_nano_blink_manual rust_nano_blink_manual.hex

clean-rust:
#	rm -f rust_nano_blink*.hex
	rm -f rust_nano_blink*
	cd nano-blink-rust && cargo clean
	cd nano-blink-manual-rust && cargo clean

upload-rust: rust_nano_blink.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:rust_nano_blink_manual.hex

analyze-rust: rust_nano_blink
	size rust_nano_blink
	size rust_nano_blink_manual

