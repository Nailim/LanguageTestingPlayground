# Mango Pi Test

Programing test of RiscV [Mango Pi](https://mangopi.org/mqpro) board.

# Requirements

## compiler

* install dependency (fedora)
    
    - sudo dnf install autoconf automake python3 libmpc-devel mpfr-devel gmp-devel gawk  bison flex texinfo patchutils gcc gcc-c++ zlib-devel expat-devel

* build/install riscv compiler

    - git clone https://github.com/riscv-collab/riscv-gnu-toolchain.git
    - cd riscv-gnu-toolchain
    - git checkout 2024.02.02
    - (make dir for compiler - /opt/rv/riscv)
    - ./configure --prefix=/opt/rc/riscv    # build with newlib
    - make
    - add to path in .bashrc: export PATH=$PATH:/opt/rv/riscv/bin

## xfel

* build/install xfel

    - git clone https://github.com/xboot/xfel.git
    - make
    - add to path in .bashrc: export PATH=$PATH:/opt/rv/xfel

### xfel usage

* check board version

    - sudo /opt/rv/xfel/xfel version

* blink LED (LCD_PWM on PD18)

    - configure PD18 as output (d1-h_user_manual_v1.0.pdf - PD_CFG2 - page 1113)
        - sudo /opt/rv/xfel/xfel write32 0x02000098 0x0FFFF1FF
    - toggle pin state HIGH (d1-h_user_manual_v1.0.pdf - PD_DATA - page 1093)
        - sudo /opt/rv/xfel/xfel write32 0x020000A0 0x00040000
    - toggle pin state LOW (d1-h_user_manual_v1.0.pdf - PD_DATA - page 1093)
        - sudo /opt/rv/xfel/xfel write32 0x020000A0 0x00000000

