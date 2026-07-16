#include "wdt.h"

void PUT32 ( unsigned int, unsigned int );
unsigned int GET32 ( unsigned int );
void dummy ( unsigned int );

/*
 * Port of rv_baremetal-boot-linux-wdt-clk-mode's main.c/uart.c/gpio.c
 * onto objtype=riscv64. gpio/uart carried over unchanged from
 * rv_baremetal-boot-plan9-riscv64-uart (already hardware-verified);
 * wdt.c is new this folder - see its own comments for the one real
 * source change (no C99 designated initializers).
 *
 * riscv_wdt is left armed by the boot chain (U-Boot/SPL), same as the
 * GCC version's comment notes - so main() only feeds it, matching that
 * version's active (non-commented-out) call. wdt_main_* and
 * wdt_riscv_enable/disable are exercised separately, not from the
 * steady-state loop, to keep this first test's blast radius small: if
 * feeding riscv_wdt doesn't work, the board resets on its own on a
 * fixed timer, which is itself the test result.
 */

#define GPIO_BASE 0x02000000
#define PD_CFG2   (GPIO_BASE + 0x0098) /* PD16..PD23 config */
#define PD_DATA   (GPIO_BASE + 0x00A0) /* PD port data register */

#define LED_PIN        18
#define LED_BIT        (1 << LED_PIN)
#define LED_CFG_SHIFT  ((LED_PIN - 16) * 4) /* CFG2 covers pins 16-23 */
#define LED_CFG_MASK   (0xF << LED_CFG_SHIFT)
#define LED_CFG_OUTPUT (0x1 << LED_CFG_SHIFT) /* function 1 = GPIO output */

#define UART0_BASE 0x02500000
#define UART_THR (UART0_BASE + (0 * 4)) /* transmit holding register */
#define UART_LSR (UART0_BASE + (5 * 4)) /* line status register */
#define UART_LSR_THRE (1 << 5) /* transmit holding register empty */

void gpio_led_init(void)
{
	unsigned int cfg;

	cfg = GET32(PD_CFG2);
	cfg &= ~LED_CFG_MASK;
	cfg |= LED_CFG_OUTPUT;
	PUT32(PD_CFG2, cfg);
}

void gpio_led_set(int on)
{
	unsigned int data;

	data = GET32(PD_DATA);
	if(on)
		data |= LED_BIT;
	else
		data &= ~LED_BIT;
	PUT32(PD_DATA, data);
}

void uart_putc(int c)
{
	while(!(GET32(UART_LSR) & UART_LSR_THRE))
		;
	PUT32(UART_THR, c & 0xFF);
}

void uart_puts(char *s)
{
	while(*s){
		if(*s == '\n')
			uart_putc('\r');
		uart_putc(*s++);
	}
}

void delay(unsigned int count)
{
	unsigned int i;

	for(i = 0; i < count; i++)
		dummy(i);
}

int main(void)
{
	int led_on;

	gpio_led_init();
	uart_puts("Hello from bare-metal RISC-V (Plan9 riscv64) on the Nezha D1!\n");
	uart_puts("Feeding riscv_wdt each tick - a reset means feeding failed.\n");

	led_on = 0;
	while(1){
		delay(20000000);
		led_on = !led_on;
		gpio_led_set(led_on);
		uart_puts(led_on ? "wdt9 kick (led on)\n" : "wdt9 kick (led off)\n");

		wdt_riscv_feed();
	}
	return(0);
}
