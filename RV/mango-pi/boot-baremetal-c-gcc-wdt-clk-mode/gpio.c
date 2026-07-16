#include "gpio.h"
#include "mmio.h"

/*
 * D1 PIO (GPIO) controller, base 0x02000000. Each port bank (PA, PB, PC,
 * PD, PE, PF, ...) occupies a 0x30-byte stride starting at PB (index 1):
 * bank_base = GPIO_BASE + index * 0x30, with Pn_CFG0..3 at +0x00.. +0x0C,
 * Pn_DATA at +0x10, Pn_DRV0/1 at +0x14/+0x18, Pn_PUL0/1 at +0x1C/+0x20.
 *
 * These two addresses (bank D, index 3: 3*0x30 + 0x08 = 0x98 for CFG2,
 * 3*0x30 + 0x10 = 0xA0 for DATA) and the pin-18 bit were worked out and
 * tested on real Nezha D1 hardware in
 * ~/Projects/RiscV/MangoBoot/lichee-rv-samples/hello02_m/main.c
 * (the on-board user LED, silkscreened "F2" on the board, is wired to
 * this SoC pin).
 */
#define GPIO_BASE 0x02000000UL
#define PD_CFG2   (GPIO_BASE + 0x0098) /* PD16..PD23 config */
#define PD_DATA   (GPIO_BASE + 0x00A0) /* PD port data register */

#define LED_PIN        18
#define LED_BIT        (1u << LED_PIN)
#define LED_CFG_SHIFT  ((LED_PIN - 16) * 4) /* CFG2 covers pins 16-23 */
#define LED_CFG_MASK   (0xFu << LED_CFG_SHIFT)
#define LED_CFG_OUTPUT (0x1u << LED_CFG_SHIFT) /* function 1 = GPIO output */

void gpio_led_init(void)
{
	unsigned int cfg = reg_read32(PD_CFG2);
	cfg &= ~LED_CFG_MASK;
	cfg |= LED_CFG_OUTPUT;
	reg_write32(PD_CFG2, cfg);
}

void gpio_led_set(int on)
{
	unsigned int data = reg_read32(PD_DATA);
	if (on)
		data |= LED_BIT;
	else
		data &= ~LED_BIT;
	reg_write32(PD_DATA, data);
}
