#include "wdt.h"

void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );

/*
 * Port of rv_baremetal-boot-linux-wdt-clk-mode's wdt.c onto
 * objtype=riscv64, using PUT32/GET32 instead of mmio.h's volatile
 * pointers. The one real source change: the original wdt_timeout_map
 * used C99 designated initializers ([0] = 0x0, ...), which ic (an
 * ANSI/K&R-era compiler) doesn't support - rewritten as a plain
 * positional array, same values in the same order.
 *
 * D1 has multiple watchdog instances sharing the same IP block/register
 * layout as U-Boot's sunxi_wdt driver (drivers/watchdog/sunxi_wdt.c,
 * sun20i_wdt_reg): CTRL/CFG/MODE at +0x10/+0x14/+0x18 from each
 * watchdog's base, gated by a fixed key that must be present in
 * CFG/MODE writes.
 *
 * Bases from arch/riscv/dts/sun20i-d1.dtsi:
 *  - "wdt" (compatible ...-wdt-reset, the main system-reset watchdog):
 *    0x020500a0
 *  - "riscv_wdt" (the C906 core's own watchdog, left enabled/no status
 *    override in the dtsi): 0x06011000
 *
 * Either one, left armed and unserviced, resets the whole board (D1 is
 * single-core, so a core reset takes the whole system down too) - each
 * gets its own enable/disable/feed below so the two can be tested and
 * toggled independently.
 */
#define MAIN_WDT_BASE  0x020500a0
#define RISCV_WDT_BASE 0x06011000

#define WDT_KEY 0x16aa0000
#define WDT_CTRL_OFFSET 0x10
#define WDT_CFG_OFFSET  0x14
#define WDT_MODE_OFFSET 0x18

#define WDT_CTRL_RELOAD ((1 << 0) | (0x0a57 << 1)) /* kick/reload */
#define WDT_MODE_EN (1 << 0)
#define WDT_RESET_MASK 0x03
#define WDT_RESET_VAL  0x01
#define WDT_TIMEOUT_SHIFT 4
#define WDT_TIMEOUT_MASK  0xf
#define WDT_MAX_TIMEOUT 16 /* seconds */

/* seconds -> register value, same table as wdt_timeout_map in
 * drivers/watchdog/sunxi_wdt.c, indices 0..16 in order */
static unsigned char wdt_timeout_map[1 + WDT_MAX_TIMEOUT] = {
	0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x7, 0x8,
	0x8, 0x9, 0x9, 0xa, 0xa, 0xb, 0xb,
};

static void wdt_feed_at(unsigned int base)
{
	PUT32(base + WDT_CTRL_OFFSET, WDT_CTRL_RELOAD);
}

static void wdt_disable_at(unsigned int base)
{
	PUT32(base + WDT_MODE_OFFSET, WDT_KEY);
}

static void wdt_enable_at(unsigned int base, unsigned int timeout_seconds)
{
	unsigned int val;

	if(timeout_seconds > WDT_MAX_TIMEOUT)
		timeout_seconds = WDT_MAX_TIMEOUT;

	/* enable system reset on expiry */
	val = GET32(base + WDT_CFG_OFFSET);
	val &= ~WDT_RESET_MASK;
	val |= WDT_RESET_VAL;
	val |= WDT_KEY;
	PUT32(base + WDT_CFG_OFFSET, val);

	/* set timeout and enable */
	val = GET32(base + WDT_MODE_OFFSET);
	val &= ~(WDT_TIMEOUT_MASK << WDT_TIMEOUT_SHIFT);
	val |= wdt_timeout_map[timeout_seconds] << WDT_TIMEOUT_SHIFT;
	val |= WDT_MODE_EN;
	val |= WDT_KEY;
	PUT32(base + WDT_MODE_OFFSET, val);

	wdt_feed_at(base);
}

void wdt_main_enable(unsigned int timeout_seconds)
{
	wdt_enable_at(MAIN_WDT_BASE, timeout_seconds);
}

void wdt_main_disable(void)
{
	wdt_disable_at(MAIN_WDT_BASE);
}

void wdt_main_feed(void)
{
	wdt_feed_at(MAIN_WDT_BASE);
}

void wdt_riscv_enable(unsigned int timeout_seconds)
{
	wdt_enable_at(RISCV_WDT_BASE, timeout_seconds);
}

void wdt_riscv_disable(void)
{
	wdt_disable_at(RISCV_WDT_BASE);
}

void wdt_riscv_feed(void)
{
	wdt_feed_at(RISCV_WDT_BASE);
}
