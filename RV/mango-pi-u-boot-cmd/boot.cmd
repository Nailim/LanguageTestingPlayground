# kernel_addr_r is a U-Boot env var pointing at a scratch DRAM address for
# loading binary images (normally the kernel). For Nezha D1 (DRAM base
# 0x40000000) it resolves to 0x41000000 - see KERNEL_ADDR_R in the U-Boot
# source at u-boot source (origin https://github.com/smaeul/u-boot.git)
# file include/configs/sunxi-common.h, ">=64MB DRAM" in d1-2022-10-31 tag
# (the D1/Nezha board-support tag/commit). board/sunxi/Kconfig pins
# SUNXI_MINIMUM_DRAM_MB to 64 for TARGET_SUN20I_D1, so the ">=256MB DRAM"
# branch never applies here even though the board actually has 1GB DRAM.
# main.bin's linker script (link.ld) places code at this same address, so
# they must be kept in sync if the board/DRAM config ever changes.


echo "Loading main.bin ..."
fatload mmc 0:1 ${kernel_addr_r} main.bin
go ${kernel_addr_r}
