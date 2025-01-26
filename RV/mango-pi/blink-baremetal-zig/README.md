# Mango Pi baremetal blink test with xfel

Horribly updated from C version

## compile

- zig build

## install

- sudo /opt/rv/xfel/xfel write 0x45000 zig-out/bin/blink.bin
- sudo /opt/rv/xfel/xfel exec 0x45000
