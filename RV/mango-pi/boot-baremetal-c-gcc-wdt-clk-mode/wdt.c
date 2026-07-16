#include "wdt.h"
#include "mmio.h"

/*
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
#define MAIN_WDT_BASE  0x020500a0u
#define RISCV_WDT_BASE 0x06011000u

#define WDT_KEY 0x16aa0000u
#define WDT_CTRL_OFFSET 0x10u
#define WDT_CFG_OFFSET  0x14u
#define WDT_MODE_OFFSET 0x18u

#define WDT_CTRL_RELOAD ((1u << 0) | (0x0a57u << 1)) /* kick/reload */
#define WDT_MODE_EN (1u << 0)
#define WDT_RESET_MASK 0x03u
#define WDT_RESET_VAL  0x01u
#define WDT_TIMEOUT_SHIFT 4u
#define WDT_TIMEOUT_MASK  0xfu
#define WDT_MAX_TIMEOUT 16u /* seconds */

/* seconds -> register value, same table as wdt_timeout_map in
 * drivers/watchdog/sunxi_wdt.c */
static const unsigned char wdt_timeout_map[1 + WDT_MAX_TIMEOUT] = {
	[0] = 0x0, [1] = 0x1, [2] = 0x2, [3] = 0x3, [4] = 0x4,
	[5] = 0x5, [6] = 0x6, [7] = 0x7, [8] = 0x7, [9] = 0x8,
	[10] = 0x8, [11] = 0x9, [12] = 0x9, [13] = 0xa, [14] = 0xa,
	[15] = 0xb, [16] = 0xb,
};

static void wdt_feed_at(unsigned long base)
{
	reg_write32(base + WDT_CTRL_OFFSET, WDT_CTRL_RELOAD);
}

static void wdt_disable_at(unsigned long base)
{
	reg_write32(base + WDT_MODE_OFFSET, WDT_KEY);
}

static void wdt_enable_at(unsigned long base, unsigned int timeout_seconds)
{
	unsigned int val;

	if (timeout_seconds > WDT_MAX_TIMEOUT)
		timeout_seconds = WDT_MAX_TIMEOUT;

	/* enable system reset on expiry */
	val = reg_read32(base + WDT_CFG_OFFSET);
	val &= ~WDT_RESET_MASK;
	val |= WDT_RESET_VAL;
	val |= WDT_KEY;
	reg_write32(base + WDT_CFG_OFFSET, val);

	/* set timeout and enable */
	val = reg_read32(base + WDT_MODE_OFFSET);
	val &= ~(WDT_TIMEOUT_MASK << WDT_TIMEOUT_SHIFT);
	val |= wdt_timeout_map[timeout_seconds] << WDT_TIMEOUT_SHIFT;
	val |= WDT_MODE_EN;
	val |= WDT_KEY;
	reg_write32(base + WDT_MODE_OFFSET, val);

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
