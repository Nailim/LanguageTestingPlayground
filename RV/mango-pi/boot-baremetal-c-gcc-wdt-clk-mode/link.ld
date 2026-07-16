OUTPUT_ARCH(riscv)
ENTRY(_start)

/*
 * 0x41000000 = kernel_addr_r in U-Boot's sunxi-common.h, i.e. DRAM base
 * 0x40000000 + 0x1000000. The branch that applies is ">=64MB DRAM", not
 * ">=256MB DRAM" - board/sunxi/Kconfig pins SUNXI_MINIMUM_DRAM_MB to 64 for
 * TARGET_SUN20I_D1 regardless of the board's actual 1GB of physical DRAM.
 * Keep this in sync with boot.cmd's load address.
 */
MEMORY
{
    RAM (rwx) : ORIGIN = 0x41000000, LENGTH = 16M
}

SECTIONS
{
    . = ORIGIN(RAM);

    .text : {
        *(.text.start)
        *(.text*)
    } > RAM

    .rodata : {
        *(.rodata*)
    } > RAM

    .data : {
        *(.data*)
    } > RAM

    .bss (NOLOAD) : {
        _bss_start = .;
        *(.bss*)
        *(COMMON)
        _bss_end = .;
    } > RAM

    . = ALIGN(16);
    . = . + 0x4000; /* 16KB stack */
    _stack_top = .;
}
