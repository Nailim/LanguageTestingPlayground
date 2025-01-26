# Mango Pi Test

Programming test of RiscV [Mango Pi](https://mangopi.org/mqpro) board.

# Requirements

## compiler

* install dependency (fedora)
    
    - $ sudo dnf install autoconf automake python3 libmpc-devel mpfr-devel gmp-devel gawk  bison flex texinfo patchutils gcc gcc-c++ zlib-devel expat-devel

* build/install riscv compiler

    - $ git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git
    - $ cd riscv-gnu-toolchain
    - $ git checkout 2024.02.02
    - make dir for compiler - /opt/rv/riscv
        - $ ...
    - $ ./configure --prefix=/opt/rc/riscv    # build with newlib
    - $ make
    - add to path in .bashrc:
        - $ export PATH=$PATH:/opt/rv/riscv/bin

## xfel (boot0)

Needed to upload and run binaries through "serial" over USB connection supported by the CPU.

* build/install xfel

    - $ git clone https://github.com/xboot/xfel.git
    - $ make
    - add to path in .bashrc: $ export PATH=$PATH:/opt/rv/xfel

### xfel usage

A few quick examples.

* check board version

    - $ sudo /opt/rv/xfel/xfel version

* blink LED (LCD_PWM on PD18)

    - configure PD18 as output (d1-h_user_manual_v1.0.pdf - PD_CFG2 - page 1113)
        - $ sudo /opt/rv/xfel/xfel write32 0x02000098 0x0FFFF1FF
    - toggle pin state HIGH (d1-h_user_manual_v1.0.pdf - PD_DATA - page 1093)
        - $ sudo /opt/rv/xfel/xfel write32 0x020000A0 0x00040000
    - toggle pin state LOW (d1-h_user_manual_v1.0.pdf - PD_DATA - page 1093)
        - $ sudo /opt/rv/xfel/xfel write32 0x020000A0 0x00000000

* upload and run a program

    - $ sudo /opt/rv/xfel/xfel write 0x45000 main.bin
    - $ sudo /opt/rv/xfel/xfel exec 0x45000

## U-Boot (boot1)

Needed to run binaries from SD card or serial through real serial interface.

### OpenSBI (Supervisor Binary Interface)

Dependency for building U-Boot.

* install riscv64-linux-gnu-gcc (fedora)

    - $ sudo dnf install gcc-riscv64-linux-gnu # let dnf install dependencies

* build OpenSBI

    - somewhere in a project folder
    - $ git clone https://github.com/riscv-software-src/opensbi
    - $ pushd opensbi
    - $ make CROSS_COMPILE=riscv64-linux-gnu- PLATFORM=generic FW_PIC=y
    - $ popd

### U-Boot (maeul fork)

* install swig and dependencies (fedora)

    - $ sudo dnf install swig openssl-devel-engine

* build U-Boot

    - somewhre in a project folder, same level as OpenSBI
    - $ git clone https://github.com/smaeul/u-boot.git smaeul-u-boot
    - $ pushd smaeul-u-boot
    - $ git checkout d1-2022-10-31riscv64-linux-gnu-gcc
    - $ make CROSS_COMPILE=riscv64-linux-gnu- nezha_defconfig
    - $ make CROSS_COMPILE=riscv64-linux-gnu- OPENSBI=../opensbi/build/platform/generic/firmware/fw_dynamic.bin
    - $ popd

* install U-Boot on an SD card

    - $ export card=/dev/sdb
    - $ export p=""
    - $ dd if=/dev/zero of=${card} bs=1k count=1023 seek=1
    - $ sudo dd if=/dev/zero of=${card} bs=1k count=1023 seek=1
    - $ sudo dd if=smaeul-u-boot/u-boot-sunxi-with-spl.bin of=${card} bs=1024 seek=8

### U-Boot usage

* connect uart bridge to PC

    CuteCom or minicom

    Note for minicom: $ minicom -s # and configure the right device

* connect uart pins to uart bridge:

    Note: First pin is on the inner side by the DS card.

    - PIN23 - GND - outer side, 3rd down
    - PIN24 - TX0 - outer side, 4th down
    - PIN25 - RX0 - outer side, 5th down

* insert card & power up with USBC


### Test U-Boot through uart

On connecting power there should be U-Boot and OpenSBI boot messages.

* Write character to uart interface (this will return a U character)

    - => mw 0x02500000 0x55

* Use other commands (partitioned SD card with FAT partition) ...

    - => mmc info
    - => mmc list
    - => mmc dev 0
    - => mmc part
    - => fatinfo mmc 0:1
    - => fatls mmc 0:1


* Upload and run srec file over uart
    
    - => loads
    
    - (in minicom)
    - CTRL+a then s then select ascii (or CTRL+a, z, s), select file blink-baremetal/main.srec
    - Press spade 2x to enter dir
    
    - => go 0x45000


* Load file from FAT partition in U-Boot

    Be careful how u-boot is flashed, do that, then create a partition with fdisk. Check https://linux-sunxi.org/Bootable_SD_card#Bootloader

        - (copy blink-baremetal-main.bin to fat partition)
        - => fatls mmc 0:1
        - => fatload mmc 0:1 0x42000000 main.bin
        - => go 0x42000000

* Load file from FAT partition in U-Boot on boot with a script

    Create file boot.cmd:

        echo "Script from /boot"
        fatload mmc 0:1 0x42000000 main.bin
        go 0x42000000

    (in terminal)
    - $ ./smaeul-u-boot/tools/mkimage -T script -d boot.cmd boot.src

    Copy to fat partition into boot folder: /boot/boot.src


* Configure u-boot to run script by default (hardcoded, improve later so it is automatic)

    - => setenv bootcmd load mmc 0:1 0x41900000 /boot/boot.src; source 0x41900000
    - => saveenv
    - =>reset

# Links

A collection of links ...

    https://github.com/dwelch67/lichee-rv-samples
    https://linux-sunxi.org/Allwinner_Nezha
    https://linux-sunxi.org/Bootable_SD_card#Bootloader
    https://krinkinmu.github.io/2023/11/19/u-boot-boot-script.html

    https://www.ringtailsoftware.co.uk/zig-baremetal/
    https://maldus512.medium.com/zig-bare-metal-programming-on-stm32f103-booting-up-b0ecdcf0de35