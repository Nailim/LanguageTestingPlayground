#include "uart.h"
#include "gpio.h"
#include "wdt.h"

#define WDT_TIMEOUT_SECONDS 4

static void delay(unsigned int count)
{
	for (volatile unsigned int i = 0; i < count; i++)
		;
}

void main(void)
{
	// Uncomment to redefine the watchdog timeout
	// wdt_main_enable(WDT_TIMEOUT_SECONDS);
	// wdt_riscv_enable(WDT_TIMEOUT_SECONDS);

	// Uncomment (and comment the enable/feed pair) to disable the watchdog
	// wdt_main_disable();
	// wdt_riscv_disable();

	gpio_led_init();
	uart_puts("Hello from bare-metal RISC-V on the Nezha D1!\n");

	int led_on = 0;
	while (1) {
		delay(20000000);
		led_on = !led_on;
		gpio_led_set(led_on);
		uart_puts(led_on ? "tick (led on)\n" : "tick (led off)\n");

		// wdt_main_feed();
		wdt_riscv_feed();	// Set by booting process - needs feeding or disabling
	}
}
