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



clean: clean-gcc clean-zig clean-zig-cc clean-zig-build-cc clean-rust
build: build-gcc build-zig build-zig-cc build-zig-build-cc build-rust



analyze: analyze-gcc analyze-zig analyze-zig-cc analyze-zig-build-cc analyze-rust
	ls -l gcc_nano_blink gcc_nano_blink.hex gcc_nano_blink_manual gcc_nano_blink_manual.hex \
	zig_nano_blink_manual zig_nano_blink_manual.hex \
	zig_cc_nano_blink zig_cc_nano_blink.hex zig_cc_nano_blink_manual zig_cc_nano_blink_manual.hex \
	zig_build_cc_nano_blink zig_build_cc_nano_blink.hex \
	rust_nano_blink rust_nano_blink.hex rust_nano_blink_manual rust_nano_blink_manual.hex



build-gcc: clean-gcc compile-gcc to-exe-gcc to-hex-gcc

compile-gcc: nano_blink.c nano_blink_manual.c
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
	size gcc_nano_blink gcc_nano_blink.hex gcc_nano_blink_manual gcc_nano_blink_manual.hex



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
	rm -f zig_nano_blink*

upload-zig: zig_nano_blink_manual.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:zig_nano_blink_manual.hex

analyze-zig: zig_nano_blink_manual
	size zig_nano_blink_manual zig_nano_blink_manual.hex



# long way around for zig cc: zig->obj->exe->hex
# https://github.com/ziglang/zig/issues/10372

build-zig-cc: clean-zig-cc compile-zig-cc to-exe-zig-cc to-hex-zig-cc

compile-zig-cc: nano_blink.c nano_blink_manual.c
	zig cc -I /usr/avr/include/ -O3 -DF_CPU=16000000UL -mmcu=atmega328p -s -c nano_blink.c -target avr-freestanding-none -mcpu=atmega328p -o zig_cc_nano_blink.o
	zig cc -I /usr/avr/include/ -O3 -DF_CPU=16000000UL -mmcu=atmega328p -s -c nano_blink_manual.c -target avr-freestanding-none -mcpu=atmega328p -o zig_cc_nano_blink_manual.o

to-exe-zig-cc:  zig_cc_nano_blink.o zig_cc_nano_blink_manual.o
	avr-gcc -mmcu=atmega328p zig_cc_nano_blink.o -o zig_cc_nano_blink
	avr-gcc -mmcu=atmega328p zig_cc_nano_blink_manual.o -o zig_cc_nano_blink_manual

to-hex-zig-cc: zig_cc_nano_blink_manual zig_cc_nano_blink_manual
	avr-objcopy -O ihex -R .eeprom zig_cc_nano_blink zig_cc_nano_blink.hex
	avr-objcopy -O ihex -R .eeprom zig_cc_nano_blink_manual zig_cc_nano_blink_manual.hex

clean-zig-cc:
	rm -f zig_cc_nano_blink*

upload-zig-cc: zig_cc_nano_blink_manual.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:zig_cc_nano_blink_manual.hex

analyze-zig-cc: zig_cc_nano_blink zig_cc_nano_blink_manual
	size zig_cc_nano_blink zig_cc_nano_blink.hex zig_cc_nano_blink_manual zig_cc_nano_blink_manual.hex



build-zig-build-cc: clean-zig-build-cc compile-zig-build-cc

compile-zig-build-cc: nano_blink.c nano_blink_manual.c
	cd nano-blink-zig-cc && zig build
	cp nano-blink-zig-cc/zig-out/zig_cc_nano_blink.elf ./zig_build_cc_nano_blink
	cp nano-blink-zig-cc/zig-out/zig_cc_nano_blink.hex ./zig_build_cc_nano_blink.hex

clean-zig-build-cc:
	rm -f zig_build_cc_nano_blink*
	cd nano-blink-zig-cc && rm -fr zig*

upload-zig-build-cc: zig_build_cc_nano_blink.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:zig_build_cc_nano_blink.hex

analyze-zig-build-cc: zig_build_cc_nano_blink
	size zig_build_cc_nano_blink zig_build_cc_nano_blink.hex



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
	rm -f rust_nano_blink*
	cd nano-blink-rust && cargo clean
	cd nano-blink-manual-rust && cargo clean

upload-rust: rust_nano_blink.hex
	avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyUSB0 -b 57600 -U flash:w:rust_nano_blink_manual.hex

analyze-rust: rust_nano_blink
	size rust_nano_blink rust_nano_blink.hex rust_nano_blink_manual rust_nano_blink_manual.hex

